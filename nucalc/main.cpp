// =============================================================================
// NuCalc — a tiny, fast pop-up calculator for Windows.
//
// Usage: press NumLock to pop the window up under the cursor's monitor,
// type an expression (numpad works even with the NumLock LED off), hit
// Enter to evaluate and save to history, press NumLock again to hide.
//
// -----------------------------------------------------------------------
// ARCHITECTURE
//
// Single translation unit, single window, no UI framework — plain Win32 +
// GDI, plus one child EDIT control for text input. Everything is drawn by
// hand (custom-painted popup, borderless, DWM drop shadow) to keep the
// binary small and startup instant. The whole app is event-driven off the
// standard Win32 message loop; there is no background thread.
//
// Building blocks, in the order they appear in this file:
//
//   1. Theming        - a small Theme{} struct with two presets (dark /
//                        light); CLR_* macros index the active theme so
//                        the rest of the code never branches on it.
//   2. Globals         - all mutable state lives in g_*-prefixed statics
//                        (window/control handles, GDI objects, history
//                        buffer, live-preview state, layout rects). No
//                        classes/instances — one calculator, one process
//                        (enforced by a named mutex in wWinMain).
//   3. Text/format helpers - ASCII narrowing of the edit text for
//                        tinyexpr, number formatting (plain, for the
//                        clipboard/history file, and "pretty", with
//                        digit grouping for on-screen display).
//   4. Expression evaluation - EvalW() bridges UTF-16 edit-box text to
//                        tinyexpr's C API (see tinyexpr.h/.c — an
//                        embedded recursive-descent math parser). The
//                        `ans` variable is bound to the last result.
//   5. History          - an in-memory ring-ish array (HistEntry[]),
//                        persisted as tab-separated UTF-8 lines under
//                        %APPDATA%\NuCalc\history.txt.
//   6. Settings/registry - small key/value helpers over
//                        HKCU\Software\NuCalc (window position, theme,
//                        first-run flag) plus a Run-key autostart toggle.
//   7. Tray icon         - a 32x32 icon is synthesized at runtime (no
//                        .ico resource needed) and shown via Shell_NotifyIcon;
//                        left-click toggles the window, right-click opens
//                        the context menu.
//   8. Layout            - Layout() computes every on-screen rectangle
//                        (history rows, input box, theme toggle, result
//                        line) from current DPI and history length, then
//                        resizes/repositions the popup and the child
//                        EDIT control to match. All hit-testing and
//                        painting read from these cached rects instead
//                        of recomputing geometry.
//   9. Painting          - Paint() renders everything into an offscreen
//                        bitmap (double-buffered) each WM_PAINT: history
//                        list, scrollbar thumb, input surface, the
//                        theme-toggle glyph, and the result/hint line.
//  10. Show/hide          - alpha-fade animation driven by a timer
//                        (TIMER_FADE), plus the foreground-activation
//                        dance (ForceFG) needed to reliably steal focus
//                        from whatever app was active when NumLock was
//                        pressed.
//  11. Input logic       - live-eval-as-you-type (UpdateLive), Enter to
//                        commit (Commit), Up/Down history recall
//                        (NavHistory), and numpad-scancode decoding so
//                        digits work regardless of the NumLock LED
//                        state. EditProc() subclasses the EDIT control
//                        to intercept these keys before the default
//                        edit-control handling sees them.
//  12. Global hotkey hook - a WH_KEYBOARD_LL hook (LLKeyboard) is the
//                        only way to catch a bare NumLock press/release
//                        system-wide without registering it as an
//                        exclusive hotkey (which would fight the OS's
//                        own NumLock toggle). It swallows both the
//                        key-down (posts WM_APP_TOGGLE) and the matching
//                        key-up, so the NumLock LED/state never changes.
//  13. Context menu       - the small right-click menu (Show/Hide,
//                        autostart, clear history, quit).
//  14. Window procedure   - WndProc() wires all of the above together:
//                        painting, mouse hit-testing against the cached
//                        layout rects (click a history row to reuse it,
//                        click the result to copy it, drag the
//                        borderless client area via WM_NCHITTEST
//                        returning HTCAPTION), DPI-change handling, and
//                        clean shutdown (persist window position/theme,
//                        unhook the keyboard hook, remove the tray icon).
//  15. Entry point        - wWinMain() wires up the single-instance
//                        guard, DPI awareness, theme/position bootstrap,
//                        window/control/icon creation, and finally the
//                        classic GetMessage/DispatchMessage loop.
// =============================================================================

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinyexpr.h"

// ---------------------------------------------------------------- constants

#define APP_NAME      L"NuCalc"
#define WND_CLASS     L"NuCalcWnd"

// --- (1) theming: two color presets selected by g_theme; CLR_* macros
// resolve to the active preset's field so drawing code stays theme-agnostic.
struct Theme {
    COLORREF bg, surf, border, text, dim, accent, err, hover;
};
static const Theme g_themes[2] = {
    // dark
    { RGB(24,25,38),    RGB(32,34,52),   RGB(49,52,74),    RGB(202,211,245),
      RGB(122,128,159), RGB(138,173,244), RGB(237,135,150), RGB(35,38,58) },
    // light
    { RGB(239,241,245), RGB(255,255,255), RGB(206,211,224), RGB(56,62,88),
      RGB(140,146,168), RGB(30,102,245),  RGB(210,15,57),   RGB(226,230,239) },
};
static int g_theme = 0;   // 0 = dark, 1 = light

#define CLR_BG        (g_themes[g_theme].bg)
#define CLR_SURF      (g_themes[g_theme].surf)
#define CLR_BORDER    (g_themes[g_theme].border)
#define CLR_TEXT      (g_themes[g_theme].text)
#define CLR_DIM       (g_themes[g_theme].dim)
#define CLR_ACCENT    (g_themes[g_theme].accent)
#define CLR_ERR       (g_themes[g_theme].err)
#define CLR_HOVER     (g_themes[g_theme].hover)

#define WM_APP_TOGGLE (WM_APP + 1)   // wParam 1 = force show
#define WM_APP_TRAY   (WM_APP + 2)

#define ID_EDIT        1
#define IDM_TOGGLE     100
#define IDM_AUTOSTART  101
#define IDM_CLEARHIST  102
#define IDM_QUIT       103

#define TIMER_FADE     1
#define TIMER_UI       2
#define TIMER_NUMLOCK  3

#define MAXHIST        300
#define MAXVIS         8
#define MAXEXPR        240

struct HistEntry {
    wchar_t expr[MAXEXPR];
    wchar_t disp[72];     // formatted result for display
    double  val;
};

// --- (2) globals: all mutable state for the one-and-only calculator
// instance. Grouped by what they drive: window/control handles & GDI
// objects, the history buffer, live-preview state, fade/animation state,
// the remembered window anchor, and the cached layout rects used by both
// painting and hit-testing.
// ---------------------------------------------------------------- globals

static HINSTANCE g_inst;
static HWND      g_hwnd, g_edit;
static WNDPROC   g_editProc0;
static HHOOK     g_hook;
static HICON     g_icon;
static UINT      g_dpi = 96;
static UINT      g_msgTaskbarCreated;

static HFONT g_fHist, g_fInput, g_fResult, g_fSmall;
static HBRUSH g_brSurf;
static int   g_editH = 24;

static HistEntry g_hist[MAXHIST];
static int    g_histCount = 0;
static int    g_scroll = 0;          // 0 = newest visible
static double g_ans = 0.0;

static int  g_navPos = -1;           // -1 = live input, 0 = newest history…
static wchar_t g_draft[MAXEXPR];
static bool g_settingText = false;

enum { LIVE_EMPTY, LIVE_OK, LIVE_STALE };
static int     g_live = LIVE_EMPTY;
static wchar_t g_liveDisp[72], g_livePlain[64];

static ULONGLONG g_errUntil = 0, g_copiedUntil = 0;

static int  g_alpha = 255, g_alphaTarget = 255;
static bool g_hiding = false;

static int g_anchorX = 100, g_anchorBottom = 500;   // window bottom-left

// layout rects, recomputed by Layout() and consumed by both Paint() and
// the mouse hit-testing in WndProc()
static RECT g_rcInput, g_rcResult, g_rcRows, g_rcTheme;
static bool g_hoverTheme = false;
static RECT g_rowRc[MAXVIS];
static int  g_rowEntry[MAXVIS];
static int  g_visRows = 0;
static int  g_hoverRow = -1;
static bool g_tracking = false;

static bool g_swallowNLUp = false;
static wchar_t g_histPath[MAX_PATH];

// --- (3) text/format helpers: ASCII narrowing for tinyexpr's C-string
// API, and the two number formatters (clipboard-safe plain vs.
// digit-grouped display).
// ---------------------------------------------------------------- helpers

static int S(int v) { return MulDiv(v, g_dpi, 96); }

static void widen(const char *a, wchar_t *w, int cap) {
    int i = 0;
    for (; a[i] && i < cap - 1; ++i) w[i] = (wchar_t)(unsigned char)a[i];
    w[i] = 0;
}

static void trimw(wchar_t *s) {
    int n = (int)wcslen(s), b = 0;
    while (s[b] == L' ' || s[b] == L'\t') ++b;
    while (n > b && (s[n-1] == L' ' || s[n-1] == L'\t')) --n;
    memmove(s, s + b, (n - b) * sizeof(wchar_t));
    s[n - b] = 0;
}

// %.12g, "-0" fixed up; plain ASCII (clipboard-safe)
static void FmtPlain(double v, wchar_t *out, int cap) {
    char b[64];
    if (v != v) { widen("nan", out, cap); return; }
    snprintf(b, sizeof b, "%.12g", v);
    if (!strcmp(b, "-0")) strcpy(b, "0");
    widen(b, out, cap);
}

// Display variant: thin-space digit grouping, pretty infinity
static void FmtDisplay(double v, wchar_t *out, int cap) {
    wchar_t p[64];
    FmtPlain(v, p, 64);
    if (!wcscmp(p, L"inf"))  { lstrcpynW(out, L"\x221E", cap);  return; }
    if (!wcscmp(p, L"-inf")) { lstrcpynW(out, L"-\x221E", cap); return; }
    if (wcschr(p, L'e') || wcschr(p, L'n')) { lstrcpynW(out, p, cap); return; }

    const wchar_t *dot = wcschr(p, L'.');
    int len = (int)wcslen(p);
    int intEnd = dot ? (int)(dot - p) : len;
    int intBeg = (p[0] == L'-') ? 1 : 0;
    int digits = intEnd - intBeg;
    if (digits <= 3) { lstrcpynW(out, p, cap); return; }

    int o = 0;
    for (int i = 0; i < len && o < cap - 2; ++i) {
        out[o++] = p[i];
        if (i >= intBeg && i < intEnd - 1) {
            int rem = intEnd - 1 - i;
            if (rem % 3 == 0) out[o++] = L'\x2009';   // thin space
        }
    }
    out[o] = 0;
}

// --- (4) expression evaluation: bridges the UTF-16 edit-box text to
// tinyexpr's C API. Only ASCII expressions are supported; a few Unicode
// math symbols (×, ÷, −, thin/no-break space) are normalized to their
// ASCII equivalents first so users can paste "nicer" expressions.
static bool EvalW(const wchar_t *w, double *out, int *errPos) {
    char a[512];
    int j = 0;
    for (int i = 0; w[i] && j < 508; ++i) {
        wchar_t c = w[i];
        if (c == 0x00D7) c = L'*';          // ×
        else if (c == 0x00F7) c = L'/';     // ÷
        else if (c == 0x2212) c = L'-';     // −
        else if (c == 0x2009 || c == 0x00A0) c = L' ';
        if (c > 127) return false;
        char ch = (char)c;
        if (ch >= 'A' && ch <= 'Z') ch += 32;
        a[j++] = ch;
    }
    a[j] = 0;
    if (!j) return false;

    te_variable vars[] = { {"ans", &g_ans, TE_VARIABLE, 0} };
    int err = 0;
    te_expr *e = te_compile(a, vars, 1, &err);
    if (!e) { if (errPos) *errPos = err; return false; }
    *out = te_eval(e);
    te_free(e);
    return true;
}

static void StartUiTimer() { SetTimer(g_hwnd, TIMER_UI, 120, NULL); }

static void CopyText(const wchar_t *s) {
    if (!s || !*s) return;
    if (!OpenClipboard(g_hwnd)) return;
    EmptyClipboard();
    size_t bytes = (wcslen(s) + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (h) {
        memcpy(GlobalLock(h), s, bytes);
        GlobalUnlock(h);
        SetClipboardData(CF_UNICODETEXT, h);
    }
    CloseClipboard();
    g_copiedUntil = GetTickCount64() + 900;
    StartUiTimer();
    InvalidateRect(g_hwnd, NULL, FALSE);
}

// --- (5) history: an append-only array capped at MAXHIST (oldest entry
// dropped once full), persisted as tab-separated "expr\tvalue" UTF-8
// lines under %APPDATA%\NuCalc\history.txt. Loaded once at startup;
// rewritten in full after every successful evaluation.
// ---------------------------------------------------------------- history

static void HistAdd(const wchar_t *expr, double val) {
    if (g_histCount == MAXHIST) {
        memmove(g_hist, g_hist + 1, (MAXHIST - 1) * sizeof(HistEntry));
        --g_histCount;
    }
    HistEntry *e = &g_hist[g_histCount++];
    lstrcpynW(e->expr, expr, MAXEXPR);
    e->val = val;
    FmtDisplay(val, e->disp, 72);
}

static void HistSave() {
    HANDLE f = CreateFileW(g_histPath, GENERIC_WRITE, 0, NULL,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    for (int i = 0; i < g_histCount; ++i) {
        char line[1024];
        char eu[768];
        WideCharToMultiByte(CP_UTF8, 0, g_hist[i].expr, -1, eu, sizeof eu, NULL, NULL);
        int n = snprintf(line, sizeof line, "%s\t%.17g\r\n", eu, g_hist[i].val);
        DWORD wr;
        WriteFile(f, line, (DWORD)n, &wr, NULL);
    }
    CloseHandle(f);
}

static void HistLoad() {
    HANDLE f = CreateFileW(g_histPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD size = GetFileSize(f, NULL);
    if (size > 0 && size < 512 * 1024) {
        char *buf = (char *)malloc(size + 1);
        DWORD rd = 0;
        if (buf && ReadFile(f, buf, size, &rd, NULL)) {
            buf[rd] = 0;
            char *p = buf;
            while (*p) {
                char *nl = strchr(p, '\n');
                if (nl) *nl = 0;
                char *cr = strchr(p, '\r');
                if (cr) *cr = 0;
                char *tab = strchr(p, '\t');
                if (tab) {
                    *tab = 0;
                    wchar_t we[MAXEXPR];
                    MultiByteToWideChar(CP_UTF8, 0, p, -1, we, MAXEXPR);
                    we[MAXEXPR - 1] = 0;
                    if (we[0]) HistAdd(we, strtod(tab + 1, NULL));
                }
                if (!nl) break;
                p = nl + 1;
            }
        }
        free(buf);
    }
    CloseHandle(f);
    if (g_histCount) g_ans = g_hist[g_histCount - 1].val;
}

// --- (6) settings/registry: tiny DWORD get/set wrappers over
// HKCU\Software\NuCalc (window position, theme choice, first-run flag),
// plus a separate helper that toggles the app's entry in the standard
// Run key for "start with Windows".
// ---------------------------------------------------------------- settings

static const wchar_t *REG_KEY = L"Software\\NuCalc";
static const wchar_t *RUN_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

static DWORD RegGetD(const wchar_t *name, DWORD def) {
    DWORD v = def, sz = sizeof v;
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &k) == ERROR_SUCCESS) {
        RegQueryValueExW(k, name, NULL, NULL, (BYTE *)&v, &sz);
        RegCloseKey(k);
    }
    return v;
}

static void RegSetD(const wchar_t *name, DWORD v) {
    HKEY k;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, NULL, 0, KEY_WRITE, NULL, &k, NULL) == ERROR_SUCCESS) {
        RegSetValueExW(k, name, 0, REG_DWORD, (const BYTE *)&v, sizeof v);
        RegCloseKey(k);
    }
}

static bool AutostartGet() {
    HKEY k;
    bool on = false;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_READ, &k) == ERROR_SUCCESS) {
        on = RegQueryValueExW(k, APP_NAME, NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
        RegCloseKey(k);
    }
    return on;
}

static void AutostartSet(bool on) {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUN_KEY, 0, KEY_SET_VALUE, &k) != ERROR_SUCCESS) return;
    if (on) {
        wchar_t path[MAX_PATH + 2] = L"\"";
        GetModuleFileNameW(NULL, path + 1, MAX_PATH);
        wcscat(path, L"\"");
        RegSetValueExW(k, APP_NAME, 0, REG_SZ, (const BYTE *)path,
                       (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
    } else {
        RegDeleteValueW(k, APP_NAME);
    }
    RegCloseKey(k);
}

// --- (7) tray icon: the 32x32 icon is synthesized pixel-by-pixel at
// runtime (rounded square + "=" glyph) so the app needs no .ico resource
// or resource compiler step; shown via Shell_NotifyIcon.
// ---------------------------------------------------------------- icon & tray

static HICON MakeIcon(int sz) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = sz;
    bi.bmiHeader.biHeight = -sz;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void *bits = NULL;
    HBITMAP bmp = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!bmp) return NULL;
    DWORD *px = (DWORD *)bits;

    const DWORD accent = 0xFF8AADF4, dark = 0xFF181926;
    int m = sz / 16, r = sz / 4;
    for (int y = 0; y < sz; ++y) {
        for (int x = 0; x < sz; ++x) {
            bool in = x >= m && x < sz - m && y >= m && y < sz - m;
            if (in) {
                int cx = -1, cy = -1;
                if (x < m + r && y < m + r)           { cx = m + r;      cy = m + r; }
                else if (x >= sz-m-r && y < m + r)    { cx = sz-m-r-1;   cy = m + r; }
                else if (x < m + r && y >= sz-m-r)    { cx = m + r;      cy = sz-m-r-1; }
                else if (x >= sz-m-r && y >= sz-m-r)  { cx = sz-m-r-1;   cy = sz-m-r-1; }
                if (cx >= 0) {
                    int dx = x - cx, dy = y - cy;
                    if (dx * dx + dy * dy > r * r) in = false;
                }
            }
            px[y * sz + x] = in ? accent : 0;
        }
    }
    // "=" glyph
    int bx0 = sz * 9 / 32, bx1 = sz * 23 / 32, bh = sz > 24 ? sz / 10 : 2;
    int y1 = sz * 12 / 32, y2 = sz * 19 / 32;
    for (int y = 0; y < sz; ++y) {
        if ((y >= y1 && y < y1 + bh) || (y >= y2 && y < y2 + bh))
            for (int x = bx0; x < bx1; ++x) px[y * sz + x] = dark;
    }

    HBITMAP mask = CreateBitmap(sz, sz, 1, 1, NULL);
    ICONINFO ii = { TRUE, 0, 0, mask, bmp };
    HICON ic = CreateIconIndirect(&ii);
    DeleteObject(bmp);
    DeleteObject(mask);
    return ic;
}

static void TrayAdd(bool balloon) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof nid;
    nid.hWnd = g_hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = g_icon;
    wcscpy(nid.szTip, APP_NAME L" \x2014 NumLock to show/hide");
    Shell_NotifyIconW(NIM_ADD, &nid);
    if (balloon) {
        nid.uFlags = NIF_INFO;
        wcscpy(nid.szInfo, L"Press NumLock any time to show or hide the calculator.");
        wcscpy(nid.szInfoTitle, APP_NAME);
        nid.dwInfoFlags = NIIF_INFO;
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }
}

static void TrayRemove() {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof nid;
    nid.hWnd = g_hwnd;
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

// --- (8) layout: Layout() is the single source of truth for on-screen
// geometry. It sizes the popup from the current history length and DPI,
// repositions/resizes the window and the child EDIT control, and caches
// every rect (history rows, input box, theme toggle, result line) that
// Paint() and the WndProc() hit-testing code read back later.
// ---------------------------------------------------------------- layout

static void MakeFonts() {
    if (g_fHist) { DeleteObject(g_fHist); DeleteObject(g_fInput); DeleteObject(g_fResult); DeleteObject(g_fSmall); }
    LOGFONTW lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    wcscpy(lf.lfFaceName, L"Segoe UI");

    lf.lfHeight = -S(15); lf.lfWeight = FW_NORMAL;   g_fHist   = CreateFontIndirectW(&lf);
    lf.lfHeight = -S(21); lf.lfWeight = FW_NORMAL;   g_fInput  = CreateFontIndirectW(&lf);
    lf.lfHeight = -S(24); lf.lfWeight = FW_SEMIBOLD; g_fResult = CreateFontIndirectW(&lf);
    lf.lfHeight = -S(12); lf.lfWeight = FW_NORMAL;   g_fSmall  = CreateFontIndirectW(&lf);

    HDC dc = GetDC(g_hwnd);
    HGDIOBJ of = SelectObject(dc, g_fInput);
    TEXTMETRICW tm;
    GetTextMetricsW(dc, &tm);
    g_editH = tm.tmHeight + S(2);
    SelectObject(dc, of);
    ReleaseDC(g_hwnd, dc);

    if (g_edit) SendMessageW(g_edit, WM_SETFONT, (WPARAM)g_fInput, TRUE);
}

static int CalcVis() {
    int v = g_histCount < MAXVIS ? g_histCount : MAXVIS;
    return v;
}

static void ClampAnchor(int W, int H) {
    POINT pt = { g_anchorX + W / 2, g_anchorBottom - H / 2 };
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof mi };
    GetMonitorInfoW(mon, &mi);
    RECT wa = mi.rcWork;
    if (g_anchorX + W > wa.right)  g_anchorX = wa.right - W - S(8);
    if (g_anchorX < wa.left)       g_anchorX = wa.left + S(8);
    if (g_anchorBottom > wa.bottom)    g_anchorBottom = wa.bottom - S(8);
    if (g_anchorBottom - H < wa.top)   g_anchorBottom = wa.top + H + S(8);
}

static void Layout() {
    int pad = S(14), rowH = S(26), inputH = S(46), resH = S(40);
    int vis = CalcVis();
    int sepGap = vis ? S(12) : 0;
    int W = S(380);
    int H = S(10) + vis * rowH + sepGap + inputH + S(6) + resH + S(10);

    if (g_scroll > g_histCount - vis) g_scroll = g_histCount - vis;
    if (g_scroll < 0) g_scroll = 0;

    int y = S(10);
    g_rcRows = { S(8), y, W - S(8), y + vis * rowH };
    int base = g_histCount - vis - g_scroll;
    for (int i = 0; i < vis; ++i) {
        g_rowRc[i] = { S(8), y, W - S(8), y + rowH };
        g_rowEntry[i] = base + i;
        y += rowH;
    }
    g_visRows = vis;
    y += sepGap;

    g_rcInput = { pad, y, W - pad, y + inputH };
    g_rcTheme = { g_rcInput.right - S(36), y + (inputH - S(22)) / 2,
                  g_rcInput.right - S(14), y + (inputH - S(22)) / 2 + S(22) };
    y += inputH + S(6);
    g_rcResult = { pad, y, W - pad, y + resH };

    ClampAnchor(W, H);
    SetWindowPos(g_hwnd, HWND_TOPMOST, g_anchorX, g_anchorBottom - H, W, H,
                 SWP_NOACTIVATE);
    MoveWindow(g_edit,
               g_rcInput.left + S(12),
               g_rcInput.top + (inputH - g_editH) / 2,
               g_rcInput.right - g_rcInput.left - S(24) - S(28),
               g_editH, TRUE);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

// --- (9) painting: Paint() draws the whole client area into an
// offscreen bitmap each WM_PAINT (double-buffered via BitBlt in
// WndProc's WM_PAINT handler, so there's no flicker). Reads the layout
// rects computed by Layout() and the theme colors selected by g_theme.
// ---------------------------------------------------------------- painting

static void FillRR(HDC dc, const RECT *r, int rad, COLORREF fill, COLORREF border) {
    HBRUSH b = CreateSolidBrush(fill);
    HPEN p = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, p);
    RoundRect(dc, r->left, r->top, r->right, r->bottom, rad * 2, rad * 2);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(b);
    DeleteObject(p);
}

static void Paint(HDC dc, const RECT *client) {
    int W = client->right, H = client->bottom;
    SetBkMode(dc, TRANSPARENT);

    HBRUSH bg = CreateSolidBrush(CLR_BG);
    FillRect(dc, client, bg);
    DeleteObject(bg);

    ULONGLONG now = GetTickCount64();

    // history rows
    SelectObject(dc, g_fHist);
    for (int i = 0; i < g_visRows; ++i) {
        const HistEntry *e = &g_hist[g_rowEntry[i]];
        RECT r = g_rowRc[i];
        if (i == g_hoverRow) FillRR(dc, &r, S(5), CLR_HOVER, CLR_HOVER);

        SIZE rs;
        GetTextExtentPoint32W(dc, e->disp, (int)wcslen(e->disp), &rs);
        RECT rr = { r.right - S(12) - rs.cx, r.top, r.right - S(12), r.bottom };
        SetTextColor(dc, CLR_TEXT);
        DrawTextW(dc, e->disp, -1, &rr, DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX);

        RECT er = { r.left + S(12), r.top, rr.left - S(16), r.bottom };
        SetTextColor(dc, CLR_DIM);
        DrawTextW(dc, e->expr, -1, &er,
                  DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    }

    // scroll indicator
    if (g_histCount > g_visRows && g_visRows > 0) {
        int trackTop = g_rcRows.top, trackBot = g_rcRows.bottom;
        int trackH = trackBot - trackTop;
        int thumbH = trackH * g_visRows / g_histCount;
        if (thumbH < S(12)) thumbH = S(12);
        int maxScroll = g_histCount - g_visRows;
        int off = (trackH - thumbH) * (maxScroll - g_scroll) / maxScroll;
        RECT t = { W - S(6), trackTop + off, W - S(3), trackTop + off + thumbH };
        FillRR(dc, &t, S(1), CLR_BORDER, CLR_BORDER);
    }

    // separator
    if (g_visRows) {
        RECT s = { S(14), g_rcRows.bottom + S(5), W - S(14), g_rcRows.bottom + S(6) };
        HBRUSH sb = CreateSolidBrush(CLR_BORDER);
        FillRect(dc, &s, sb);
        DeleteObject(sb);
    }

    // input surface
    bool errFlash = now < g_errUntil;
    FillRR(dc, &g_rcInput, S(8), CLR_SURF, errFlash ? CLR_ERR : CLR_BORDER);

    // theme toggle: sun when dark (→ light), crescent when light (→ dark)
    {
        int cx = (g_rcTheme.left + g_rcTheme.right) / 2;
        int cy = (g_rcTheme.top + g_rcTheme.bottom) / 2;
        COLORREF col = g_hoverTheme ? CLR_TEXT : CLR_DIM;
        HBRUSH b = CreateSolidBrush(col);
        HPEN p = CreatePen(PS_SOLID, S(1) > 1 ? S(1) : 1, col);
        HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, p);
        if (g_theme == 0) {
            int r1 = S(4);
            Ellipse(dc, cx - r1, cy - r1, cx + r1, cy + r1);
            for (int i = 0; i < 8; ++i) {
                double a = i * 3.14159265 / 4.0;
                double c = cos(a), s = sin(a);
                MoveToEx(dc, cx + (int)(c * (r1 + S(2))), cy + (int)(s * (r1 + S(2))), NULL);
                LineTo(dc, cx + (int)(c * (r1 + S(4))), cy + (int)(s * (r1 + S(4))));
            }
        } else {
            int r1 = S(6);
            Ellipse(dc, cx - r1, cy - r1, cx + r1, cy + r1);
            HBRUSH sb = CreateSolidBrush(CLR_SURF);
            HPEN sp = CreatePen(PS_SOLID, 1, CLR_SURF);
            SelectObject(dc, sb);
            SelectObject(dc, sp);
            Ellipse(dc, cx - r1 + S(4), cy - r1 - S(2), cx + r1 + S(4), cy + r1 - S(2));
            SelectObject(dc, b);
            SelectObject(dc, p);
            DeleteObject(sb);
            DeleteObject(sp);
        }
        SelectObject(dc, ob);
        SelectObject(dc, op);
        DeleteObject(b);
        DeleteObject(p);
    }

    // result line
    RECT res = g_rcResult;
    if (now < g_copiedUntil) {
        SelectObject(dc, g_fSmall);
        SetTextColor(dc, CLR_ACCENT);
        RECT c = { res.left + S(4), res.top, res.right, res.bottom };
        DrawTextW(dc, L"copied \x2713", -1, &c,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    } else if (errFlash) {
        SelectObject(dc, g_fSmall);
        SetTextColor(dc, CLR_ERR);
        RECT c = { res.left + S(4), res.top, res.right, res.bottom };
        DrawTextW(dc, L"syntax error", -1, &c,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    }

    if (g_live != LIVE_EMPTY && g_liveDisp[0]) {
        SelectObject(dc, g_fResult);
        SetTextColor(dc, g_live == LIVE_OK ? CLR_ACCENT : CLR_DIM);
        SIZE vs;
        GetTextExtentPoint32W(dc, g_liveDisp, (int)wcslen(g_liveDisp), &vs);
        RECT vr = { res.right - S(4) - vs.cx, res.top, res.right - S(4), res.bottom };
        DrawTextW(dc, g_liveDisp, -1, &vr,
                  DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX);
        SetTextColor(dc, CLR_DIM);
        SIZE es;
        GetTextExtentPoint32W(dc, L"=", 1, &es);
        RECT er = { vr.left - es.cx - S(10), res.top, vr.left - S(10), res.bottom };
        DrawTextW(dc, L"=", 1, &er, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    } else if (g_live == LIVE_EMPTY && now >= g_copiedUntil && now >= g_errUntil) {
        SelectObject(dc, g_fSmall);
        SetTextColor(dc, CLR_DIM);
        wchar_t hint[128];
        if (g_histCount) {
            wchar_t a[64];
            FmtDisplay(g_ans, a, 64);
            wchar_t tmp[96];
            lstrcpynW(tmp, a, 96);
            wsprintfW(hint, L"ans = %s      \x00B7      NumLock hides", tmp);
        } else {
            wcscpy(hint, L"Type an expression \x2014 Enter saves \x00B7 NumLock hides");
        }
        RECT c = { res.left + S(4), res.top, res.right, res.bottom };
        DrawTextW(dc, hint, -1, &c, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    }

    // window border
    RECT br = *client;
    HBRUSH bb = CreateSolidBrush(CLR_BORDER);
    FrameRect(dc, &br, bb);
    DeleteObject(bb);
    (void)H;
}

// flips the active theme, persists the choice, and repaints (also
// refreshes g_brSurf, the brush WM_CTLCOLOREDIT hands back for the
// EDIT control's background)
static void ToggleTheme() {
    g_theme ^= 1;
    RegSetD(L"theme", (DWORD)g_theme);
    if (g_brSurf) DeleteObject(g_brSurf);
    g_brSurf = CreateSolidBrush(CLR_SURF);
    InvalidateRect(g_edit, NULL, TRUE);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

// --- (10) show/hide: alpha-fade animation (TIMER_FADE, driven from
// WndProc's WM_TIMER handler) plus the foreground-activation dance
// needed to reliably steal keyboard focus from whatever window was
// active when NumLock was pressed.
// ---------------------------------------------------------------- show/hide

static void StartFade() { SetTimer(g_hwnd, TIMER_FADE, 16, NULL); }

static void FocusInput() {
    SetFocus(g_edit);
    SendMessageW(g_edit, EM_SETSEL, 0, -1);
}

static void ForceFG() {
    HWND fg = GetForegroundWindow();
    if (fg == g_hwnd) return;
    DWORD tid = fg ? GetWindowThreadProcessId(fg, NULL) : 0;
    DWORD my = GetCurrentThreadId();
    if (tid && tid != my) AttachThreadInput(my, tid, TRUE);
    SetForegroundWindow(g_hwnd);
    BringWindowToTop(g_hwnd);
    if (tid && tid != my) AttachThreadInput(my, tid, FALSE);
    if (GetForegroundWindow() != g_hwnd) {
        keybd_event(VK_MENU, 0, 0, 0);
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
        SetForegroundWindow(g_hwnd);
    }
}

static void ShowCalc() {
    g_hiding = false;
    Layout();
    if (!IsWindowVisible(g_hwnd)) {
        g_alpha = 0;
        SetLayeredWindowAttributes(g_hwnd, 0, 0, LWA_ALPHA);
        ShowWindow(g_hwnd, SW_SHOWNA);
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    ForceFG();
    FocusInput();
    g_alphaTarget = 255;
    StartFade();
}

static void SaveAnchor() {
    RECT r;
    GetWindowRect(g_hwnd, &r);
    g_anchorX = r.left;
    g_anchorBottom = r.bottom;
    RegSetD(L"x", (DWORD)g_anchorX);
    RegSetD(L"bottom", (DWORD)g_anchorBottom);
}

static void HideCalc() {
    if (!IsWindowVisible(g_hwnd) || g_hiding) return;
    SaveAnchor();
    g_hiding = true;
    g_alphaTarget = 0;
    StartFade();
}

// --- (11) input logic: live-eval-as-you-type, Enter-to-commit, Up/Down
// history recall, and numpad-scancode decoding (so digits keep working
// even when the NumLock LED is off, since the global hook below eats
// the NumLock keystroke before Windows can toggle it). EditProc()
// subclasses the EDIT control to intercept these before the default
// edit-control window procedure sees them.
// ---------------------------------------------------------------- input logic

static void SetEditText(const wchar_t *s) {
    g_settingText = true;
    SetWindowTextW(g_edit, s);
    g_settingText = false;
    int n = (int)wcslen(s);
    SendMessageW(g_edit, EM_SETSEL, n, n);
    SendMessageW(g_edit, EM_SCROLLCARET, 0, 0);
}

static void UpdateLive() {
    wchar_t t[MAXEXPR];
    GetWindowTextW(g_edit, t, MAXEXPR);
    trimw(t);
    g_errUntil = 0;
    if (!t[0]) {
        g_live = LIVE_EMPTY;
        g_liveDisp[0] = 0;
        g_livePlain[0] = 0;
    } else {
        double v;
        if (EvalW(t, &v, NULL)) {
            g_live = LIVE_OK;
            FmtDisplay(v, g_liveDisp, 72);
            FmtPlain(v, g_livePlain, 64);
        } else if (g_liveDisp[0]) {
            g_live = LIVE_STALE;
        } else {
            g_live = LIVE_EMPTY;
        }
    }
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void Commit() {
    wchar_t t[MAXEXPR];
    GetWindowTextW(g_edit, t, MAXEXPR);
    trimw(t);
    if (!t[0]) { HideCalc(); return; }

    double v;
    int errPos = 0;
    if (!EvalW(t, &v, &errPos)) {
        g_errUntil = GetTickCount64() + 1400;
        if (errPos > 0) {
            SendMessageW(g_edit, EM_SETSEL, errPos - 1, errPos - 1);
            SendMessageW(g_edit, EM_SCROLLCARET, 0, 0);
        }
        StartUiTimer();
        InvalidateRect(g_hwnd, NULL, FALSE);
        return;
    }

    HistAdd(t, v);
    g_ans = v;
    HistSave();
    g_scroll = 0;
    g_navPos = -1;
    g_draft[0] = 0;
    SetEditText(L"");
    g_live = LIVE_EMPTY;
    g_liveDisp[0] = 0;
    g_livePlain[0] = 0;
    Layout();
}

static void NavHistory(int dir) {
    if (!g_histCount) return;
    if (g_navPos == -1 && dir > 0)
        GetWindowTextW(g_edit, g_draft, MAXEXPR);
    int np = g_navPos + dir;
    if (np < -1) np = -1;
    if (np > g_histCount - 1) np = g_histCount - 1;
    if (np == g_navPos) return;
    g_navPos = np;
    if (np == -1) SetEditText(g_draft);
    else SetEditText(g_hist[g_histCount - 1 - np].expr);
    UpdateLive();
}

// numpad scancodes → chars (works even when NumLock LED is off,
// since we swallow the NumLock key globally)
static wchar_t NumpadChar(UINT vk, UINT sc, bool extended) {
    if (extended) return 0;
    switch (vk) {
        case VK_HOME: case VK_UP: case VK_PRIOR: case VK_LEFT:
        case VK_CLEAR: case VK_RIGHT: case VK_END: case VK_DOWN:
        case VK_NEXT: case VK_INSERT: case VK_DELETE:
            break;
        default:
            return 0;
    }
    switch (sc) {
        case 0x47: return L'7';
        case 0x48: return L'8';
        case 0x49: return L'9';
        case 0x4B: return L'4';
        case 0x4C: return L'5';
        case 0x4D: return L'6';
        case 0x4F: return L'1';
        case 0x50: return L'2';
        case 0x51: return L'3';
        case 0x52: return L'0';
        case 0x53: return L'.';
    }
    return 0;
}

static LRESULT CALLBACK EditProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_KEYDOWN: {
        bool ext = (l >> 24) & 1;
        UINT sc = (l >> 16) & 0xFF;
        switch (w) {
        case VK_RETURN:
            Commit();
            return 0;
        case VK_ESCAPE:
            HideCalc();
            return 0;
        case VK_UP:
            if (ext) { NavHistory(+1); return 0; }
            break;
        case VK_DOWN:
            if (ext) { NavHistory(-1); return 0; }
            break;
        case 'A':
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                SendMessageW(h, EM_SETSEL, 0, -1);
                return 0;
            }
            break;
        case 'C':
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                DWORD s0 = 0, s1 = 0;
                SendMessageW(h, EM_GETSEL, (WPARAM)&s0, (LPARAM)&s1);
                if (s0 == s1) {   // nothing selected → copy result
                    CopyText(g_livePlain[0] ? g_livePlain : NULL);
                    return 0;
                }
            }
            break;
        }
        wchar_t np = NumpadChar((UINT)w, sc, ext);
        if (np) {
            wchar_t s[2] = { np, 0 };
            SendMessageW(h, EM_REPLACESEL, TRUE, (LPARAM)s);
            return 0;
        }
        break;
    }
    case WM_CHAR: {
        UINT sc = (l >> 16) & 0xFF;
        wchar_t c = (wchar_t)w;
        if (c == L'\r' || c == L'\n' || c == 27) return 0;
        if (c == 1) return 0;                       // ctrl+a handled
        if (c == L'=') { Commit(); return 0; }
        if (c == L',' && sc == 0x53)                 // numpad decimal in comma locales
            return CallWindowProcW(g_editProc0, h, m, L'.', l);
        if (c == 0x7F) {                             // ctrl+backspace: delete word
            DWORD s0 = 0, s1 = 0;
            SendMessageW(h, EM_GETSEL, (WPARAM)&s0, (LPARAM)&s1);
            wchar_t t[MAXEXPR];
            GetWindowTextW(h, t, MAXEXPR);
            int i = (int)s0;
            while (i > 0 && t[i-1] == L' ') --i;
            while (i > 0 && t[i-1] != L' ') --i;
            SendMessageW(h, EM_SETSEL, i, s1);
            SendMessageW(h, EM_REPLACESEL, TRUE, (LPARAM)L"");
            return 0;
        }
        break;
    }
    }
    return CallWindowProcW(g_editProc0, h, m, w, l);
}

// --- (12) global hotkey hook: WH_KEYBOARD_LL is the only way to catch
// a bare NumLock press/release system-wide without registering it as an
// exclusive hotkey (which would fight/replace the OS's own NumLock
// toggle). Both the key-down (which posts WM_APP_TOGGLE to the main
// window) and the matching key-up are swallowed, so a bare NumLock press
// never toggles the real NumLock state by itself; Ctrl/Shift/Alt+NumLock
// pass through untouched. The OS toggle can still end up off (whatever
// it was at process start, via that modifier passthrough, or via any
// other path we don't control), so EnsureNumLockOn() is called at
// startup, on every show/hide, and from a low-frequency TIMER_NUMLOCK
// tick that runs for the app's whole lifetime, forcing it back on with
// a synthetic keypress — so the numpad always types digits system-wide
// even while the calculator is hidden in the tray.
// ---------------------------------------------------------------- hook

// Forces the real (OS-tracked) NumLock state on by injecting a synthetic
// keypress when needed. Injected events carry LLKHF_INJECTED, so
// LLKeyboard() below lets them fall through to CallNextHookEx() instead
// of swallowing them — that's what lets this actually flip the OS state
// (and the keyboard LED), unlike a genuine bare NumLock press.
static void EnsureNumLockOn() {
    if (!(GetKeyState(VK_NUMLOCK) & 1)) {
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}

static LRESULT CALLBACK LLKeyboard(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT *k = (const KBDLLHOOKSTRUCT *)l;
        if (k->vkCode == VK_NUMLOCK && !(k->flags & LLKHF_INJECTED)) {
            if (w == WM_KEYDOWN || w == WM_SYSKEYDOWN) {
                bool mod = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                           (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                           (k->flags & LLKHF_ALTDOWN);
                if (!mod) {
                    g_swallowNLUp = true;
                    PostMessageW(g_hwnd, WM_APP_TOGGLE, 0, 0);
                    return 1;
                }
            } else if ((w == WM_KEYUP || w == WM_SYSKEYUP) && g_swallowNLUp) {
                g_swallowNLUp = false;
                return 1;
            }
        }
    }
    return CallNextHookEx(g_hook, code, w, l);
}

// --- (13) context menu: the small right-click menu shown from the tray
// icon or by right-clicking empty space in the popup.
// ---------------------------------------------------------------- menu

static void ShowMenu() {
    HMENU m = CreatePopupMenu();
    bool vis = IsWindowVisible(g_hwnd) && !g_hiding;
    AppendMenuW(m, MF_STRING, IDM_TOGGLE, vis ? L"Hide\tNumLock" : L"Show\tNumLock");
    AppendMenuW(m, MF_STRING | (AutostartGet() ? MF_CHECKED : 0), IDM_AUTOSTART,
                L"Start with Windows");
    AppendMenuW(m, MF_STRING, IDM_CLEARHIST, L"Clear history");
    AppendMenuW(m, MF_SEPARATOR, 0, NULL);
    AppendMenuW(m, MF_STRING, IDM_QUIT, L"Quit " APP_NAME);
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hwnd);
    TrackPopupMenu(m, TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hwnd, NULL);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(m);
}

// --- (14) window procedure: ties everything above together. Handles
// double-buffered painting, mouse hit-testing against the layout rects
// cached by Layout() (click a history row to reuse its expression,
// click the result to copy it, click the theme toggle), dragging the
// borderless window by returning HTCAPTION for empty client area in
// WM_NCHITTEST, DPI-change re-layout, and clean shutdown (persist
// window position/theme, unhook the keyboard hook, remove the tray icon).
// ---------------------------------------------------------------- wndproc

static int RowAt(POINT pt) {
    for (int i = 0; i < g_visRows; ++i)
        if (PtInRect(&g_rowRc[i], pt)) return i;
    return -1;
}

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == g_msgTaskbarCreated) { TrayAdd(false); return 0; }

    switch (m) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT rc;
        GetClientRect(h, &rc);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
        HGDIOBJ ob = SelectObject(mem, bmp);
        Paint(mem, &rc);
        BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, ob);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;

    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)w;
        SetTextColor(dc, CLR_TEXT);
        SetBkColor(dc, CLR_SURF);
        return (LRESULT)g_brSurf;
    }

    case WM_COMMAND:
        if (LOWORD(w) == ID_EDIT && HIWORD(w) == EN_CHANGE) {
            if (!g_settingText) g_navPos = -1;
            UpdateLive();
            return 0;
        }
        switch (LOWORD(w)) {
        case IDM_TOGGLE:
            PostMessageW(h, WM_APP_TOGGLE, 0, 0);
            return 0;
        case IDM_AUTOSTART:
            AutostartSet(!AutostartGet());
            return 0;
        case IDM_CLEARHIST:
            g_histCount = 0;
            g_scroll = 0;
            g_navPos = -1;
            HistSave();
            Layout();
            return 0;
        case IDM_QUIT:
            DestroyWindow(h);
            return 0;
        }
        break;

    case WM_APP_TOGGLE:
        EnsureNumLockOn();
        if (w == 1) { ShowCalc(); return 0; }
        if (IsWindowVisible(h) && !g_hiding) {
            if (GetForegroundWindow() == h) HideCalc();
            else {
                ForceFG();
                FocusInput();
                g_alphaTarget = 255;
                StartFade();
            }
        } else {
            ShowCalc();
        }
        return 0;

    case WM_APP_TRAY:
        if (LOWORD(l) == WM_LBUTTONUP) PostMessageW(h, WM_APP_TOGGLE, 0, 0);
        else if (LOWORD(l) == WM_RBUTTONUP || LOWORD(l) == WM_CONTEXTMENU) ShowMenu();
        return 0;

    case WM_TIMER:
        if (w == TIMER_FADE) {
            int step = 51;
            if (g_alpha < g_alphaTarget) {
                g_alpha += step;
                if (g_alpha > g_alphaTarget) g_alpha = g_alphaTarget;
            } else {
                g_alpha -= step;
                if (g_alpha < g_alphaTarget) g_alpha = g_alphaTarget;
            }
            SetLayeredWindowAttributes(h, 0, (BYTE)g_alpha, LWA_ALPHA);
            if (g_alpha == g_alphaTarget) {
                KillTimer(h, TIMER_FADE);
                if (g_alpha == 0) {
                    ShowWindow(h, SW_HIDE);
                    g_hiding = false;
                }
            }
            return 0;
        }
        if (w == TIMER_UI) {
            ULONGLONG now = GetTickCount64();
            InvalidateRect(h, NULL, FALSE);
            if (now > g_errUntil && now > g_copiedUntil) KillTimer(h, TIMER_UI);
            return 0;
        }
        if (w == TIMER_NUMLOCK) {
            EnsureNumLockOn();
            return 0;
        }
        break;

    case WM_ACTIVATE:
        if (IsWindowVisible(h) && !g_hiding) {
            g_alphaTarget = (LOWORD(w) == WA_INACTIVE) ? 235 : 255;
            StartFade();
        }
        return 0;

    case WM_SETFOCUS:
        SetFocus(g_edit);
        return 0;

    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProcW(h, m, w, l);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
            ScreenToClient(h, &pt);
            if (PtInRect(&g_rcInput, pt) || PtInRect(&g_rcResult, pt) ||
                RowAt(pt) >= 0)
                return HTCLIENT;
            return HTCAPTION;
        }
        return hit;
    }

    case WM_SETCURSOR: {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(h, &pt);
        if (RowAt(pt) >= 0 || PtInRect(&g_rcTheme, pt) ||
            (PtInRect(&g_rcResult, pt) && g_live != LIVE_EMPTY)) {
            SetCursor(LoadCursorW(NULL, IDC_HAND));
            return TRUE;
        }
        break;
    }

    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        int r = RowAt(pt);
        if (r != g_hoverRow) {
            g_hoverRow = r;
            InvalidateRect(h, &g_rcRows, FALSE);
        }
        bool ht = PtInRect(&g_rcTheme, pt) != 0;
        if (ht != g_hoverTheme) {
            g_hoverTheme = ht;
            InvalidateRect(h, &g_rcTheme, FALSE);
        }
        if (!g_tracking) {
            TRACKMOUSEEVENT tme = { sizeof tme, TME_LEAVE, h, 0 };
            TrackMouseEvent(&tme);
            g_tracking = true;
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        g_tracking = false;
        if (g_hoverRow != -1) {
            g_hoverRow = -1;
            InvalidateRect(h, &g_rcRows, FALSE);
        }
        if (g_hoverTheme) {
            g_hoverTheme = false;
            InvalidateRect(h, &g_rcTheme, FALSE);
        }
        return 0;

    case WM_MOUSEWHEEL: {
        if (g_histCount > g_visRows) {
            int delta = GET_WHEEL_DELTA_WPARAM(w);
            g_scroll += (delta > 0) ? 1 : -1;
            if (g_scroll < 0) g_scroll = 0;
            if (g_scroll > g_histCount - g_visRows)
                g_scroll = g_histCount - g_visRows;
            Layout();
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        int r = RowAt(pt);
        if (r >= 0) {
            g_navPos = -1;
            SetEditText(g_hist[g_rowEntry[r]].expr);
            SetFocus(g_edit);
            UpdateLive();
            return 0;
        }
        if (PtInRect(&g_rcTheme, pt)) { ToggleTheme(); return 0; }
        if (PtInRect(&g_rcInput, pt)) { SetFocus(g_edit); return 0; }
        if (PtInRect(&g_rcResult, pt) && g_livePlain[0]) {
            CopyText(g_livePlain);
            return 0;
        }
        break;
    }

    case WM_RBUTTONUP: {
        POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        int r = RowAt(pt);
        if (r >= 0) {
            wchar_t p[64];
            FmtPlain(g_hist[g_rowEntry[r]].val, p, 64);
            CopyText(p);
            return 0;
        }
        ShowMenu();
        return 0;
    }

    case WM_EXITSIZEMOVE:
        SaveAnchor();
        return 0;

    case WM_DPICHANGED: {
        g_dpi = HIWORD(w);
        const RECT *r = (const RECT *)l;
        g_anchorX = r->left;
        g_anchorBottom = r->bottom;
        MakeFonts();
        Layout();
        return 0;
    }

    case WM_DISPLAYCHANGE:
        Layout();
        return 0;

    case WM_CLOSE:
        HideCalc();
        return 0;

    case WM_QUERYENDSESSION:
        SaveAnchor();
        RegSetD(L"shown", 1);
        return TRUE;

    case WM_DESTROY:
        SaveAnchor();
        RegSetD(L"shown", 1);
        TrayRemove();
        if (g_hook) UnhookWindowsHookEx(g_hook);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

// --- (15) entry point: single-instance guard (a second launch just
// forwards WM_APP_TOGGLE to the running instance and exits), per-monitor
// DPI awareness, theme/position bootstrap from the registry (falling
// back to the system light/dark setting and a sensible default
// position), window/control/icon creation, installing the global
// keyboard hook, and the classic GetMessage/DispatchMessage loop.
// ---------------------------------------------------------------- main

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int) {
    g_inst = inst;

    CreateMutexW(NULL, TRUE, L"NuCalcSingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND prev = FindWindowW(WND_CLASS, NULL);
        if (prev) PostMessageW(prev, WM_APP_TOGGLE, 1, 0);
        return 0;
    }

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // history file path
    wchar_t dir[MAX_PATH];
    if (GetEnvironmentVariableW(L"APPDATA", dir, MAX_PATH)) {
        wcscat(dir, L"\\NuCalc");
        CreateDirectoryW(dir, NULL);
        wcscpy(g_histPath, dir);
        wcscat(g_histPath, L"\\history.txt");
    } else {
        wcscpy(g_histPath, L"nucalc_history.txt");
    }

    // theme: saved choice, else follow the system apps-theme setting
    DWORD th = RegGetD(L"theme", 0xFFFFFFFF);
    if (th == 0xFFFFFFFF) {
        DWORD light = 0, sz = sizeof light;
        HKEY k;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                0, KEY_READ, &k) == ERROR_SUCCESS) {
            RegQueryValueExW(k, L"AppsUseLightTheme", NULL, NULL, (BYTE *)&light, &sz);
            RegCloseKey(k);
        }
        g_theme = light ? 1 : 0;
    } else {
        g_theme = th ? 1 : 0;
    }

    g_icon = MakeIcon(32);
    g_brSurf = CreateSolidBrush(CLR_SURF);
    g_msgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {};
    wc.style = CS_DROPSHADOW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hIcon = g_icon;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = WND_CLASS;
    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        WND_CLASS, APP_NAME, WS_POPUP,
        0, 0, 100, 100, NULL, NULL, inst, NULL);
    if (!g_hwnd) return 1;
    SetLayeredWindowAttributes(g_hwnd, 0, 255, LWA_ALPHA);

    DWORD corner = 2;   // DWMWCP_ROUND (Win11; harmless no-op on Win10)
    DwmSetWindowAttribute(g_hwnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/,
                          &corner, sizeof corner);

    g_dpi = GetDpiForWindow(g_hwnd);

    g_edit = CreateWindowExW(0, L"EDIT", L"",
                             WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT,
                             0, 0, 10, 10, g_hwnd, (HMENU)(INT_PTR)ID_EDIT, inst, NULL);
    g_editProc0 = (WNDPROC)SetWindowLongPtrW(g_edit, GWLP_WNDPROC, (LONG_PTR)EditProc);
    SendMessageW(g_edit, EM_SETLIMITTEXT, MAXEXPR - 8, 0);

    MakeFonts();
    HistLoad();

    // position: saved, or centered in upper part of the primary work area
    bool firstRun = RegGetD(L"shown", 0) == 0;
    RECT wa;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    g_anchorX = (int)RegGetD(L"x", (DWORD)((wa.left + wa.right - S(380)) / 2));
    g_anchorBottom = (int)RegGetD(L"bottom", (DWORD)(wa.top + (wa.bottom - wa.top) * 42 / 100));

    Layout();
    TrayAdd(firstRun);
    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LLKeyboard, GetModuleHandleW(NULL), 0);
    EnsureNumLockOn();
    SetTimer(g_hwnd, TIMER_NUMLOCK, 1000, NULL);  // keep re-asserting for app's lifetime

    ShowCalc();

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_icon) DestroyIcon(g_icon);
    return (int)msg.wParam;
}
