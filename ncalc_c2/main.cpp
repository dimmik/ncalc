#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <shellapi.h> // For system tray
#include <thread>     // For keyboard hook thread
#include <atomic>     // For atomic boolean for NumLock state
#include "expression_evaluator.h" // Include the new header for expression evaluation

// ============================================================
// [CUSTOMIZATION POINT]: Application configuration constants
// ============================================================
const char* CLASS_NAME = "NCalcWindowClass";
const char* WINDOW_TITLE = "NCalc";
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 600;

// Custom message for tray icon
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_ICON 1000

// Context menu IDs
#define IDM_RESTORE 1001
#define IDM_EXIT 1002

// Control IDs
#define IDC_INPUT 101
#define IDC_HISTORY 102
#define IDC_BUTTON_0 200
#define IDC_BUTTON_1 201
#define IDC_BUTTON_2 202
#define IDC_BUTTON_3 203
#define IDC_BUTTON_4 204
#define IDC_BUTTON_5 205
#define IDC_BUTTON_6 206
#define IDC_BUTTON_7 207
#define IDC_BUTTON_8 208
#define IDC_BUTTON_9 209
#define IDC_BUTTON_PLUS 210
#define IDC_BUTTON_MINUS 211
#define IDC_BUTTON_MUL 212
#define IDC_BUTTON_DIV 213
#define IDC_BUTTON_SQ 214
#define IDC_BUTTON_SQRT 215
#define IDC_BUTTON_LPAREN 216
#define IDC_BUTTON_RPAREN 217
#define IDC_BUTTON_EQUAL 218
#define IDC_BUTTON_C 219
#define IDC_BUTTON_CE 220
#define IDC_BUTTON_DOT 221


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateUI(HWND hwnd, HINSTANCE hInstance);
void EvaluateExpression();
void SaveHistory();
void LoadHistory();
void AddTrayIcon(HWND hwnd);
void RemoveTrayIcon(HWND hwnd);
void ShowContextMenu(HWND hwnd);
void SetNumLock(bool state); // Forward declaration for SetNumLock

HHOOK g_keyboardHook = NULL;
std::atomic<bool> g_numLockState(false); // true if NumLock is ON

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void InstallKeyboardHook();
void UninstallKeyboardHook();

HWND g_hwndInput;
HWND g_hwndHistory;
HWND g_hwndMain; // Global handle for the main window


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // ============================================================
    // [CUSTOMIZATION POINT]: UI element creation (main window)
    // ============================================================
    // Register the window class.
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        WINDOW_TITLE,                   // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    g_hwndMain = hwnd; // Store main window handle
    AddTrayIcon(hwnd); // Add tray icon

    CreateUI(hwnd, hInstance);
    LoadHistory(); // Load history on startup

    InstallKeyboardHook(); // Install keyboard hook

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UninstallKeyboardHook(); // Uninstall keyboard hook
    RemoveTrayIcon(hwnd); // Remove tray icon on exit
    return 0;
}

// ============================================================
// [CUSTOMIZATION POINT]: UI element creation
// ============================================================
void CreateUI(HWND hwnd, HINSTANCE hInstance) {
    // Create Input Field
    g_hwndInput = CreateWindowEx(
        WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_AUTOHSCROLL,
        10, 10, 360, 40,
        hwnd, (HMENU)IDC_INPUT, hInstance, NULL);

    // Create History List View
    g_hwndHistory = CreateWindowEx(
        WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        10, 60, 360, 200,
        hwnd, (HMENU)IDC_HISTORY, hInstance, NULL);

    // ============================================================
    // [CUSTOMIZATION POINT]: UI element creation (history list)
    // ============================================================
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem = 0;
    lvc.pszText = const_cast<LPSTR>("Expression");
    lvc.cx = 150;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(g_hwndHistory, 0, &lvc);

    lvc.iSubItem = 1;
    lvc.pszText = const_cast<LPSTR>("Result");
    lvc.cx = 100;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(g_hwndHistory, 1, &lvc);

    lvc.iSubItem = 2;
    lvc.pszText = const_cast<LPSTR>("Timestamp");
    lvc.cx = 110;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(g_hwndHistory, 2, &lvc);

    // ============================================================
    // [CUSTOMIZATION POINT]: Calculator button layout and positioning
    // ============================================================
    const int buttonWidth = 60;
    const int buttonHeight = 40;
    const int startX = 10;
    const int startY = 270;
    const int spacingX = 5;
    const int spacingY = 5;

    struct ButtonData {
        const char* text;
        int id;
        int x;
        int y;
    };

    ButtonData buttons[] = {
        {"7", IDC_BUTTON_7, 0, 0}, {"8", IDC_BUTTON_8, 1, 0}, {"9", IDC_BUTTON_9, 2, 0}, {"/", IDC_BUTTON_DIV, 3, 0}, {"C", IDC_BUTTON_C, 4, 0}, {"CE", IDC_BUTTON_CE, 5, 0},
        {"4", IDC_BUTTON_4, 0, 1}, {"5", IDC_BUTTON_5, 1, 1}, {"6", IDC_BUTTON_6, 2, 1}, {"*", IDC_BUTTON_MUL, 3, 1}, {"(", IDC_BUTTON_LPAREN, 4, 1}, {")", IDC_BUTTON_RPAREN, 5, 1},
        {"1", IDC_BUTTON_1, 0, 2}, {"2", IDC_BUTTON_2, 1, 2}, {"3", IDC_BUTTON_3, 2, 2}, {"-", IDC_BUTTON_MINUS, 3, 2}, {"x²", IDC_BUTTON_SQ, 4, 2}, {"√", IDC_BUTTON_SQRT, 5, 2},
        {"0", IDC_BUTTON_0, 0, 3}, {".", IDC_BUTTON_DOT, 1, 3}, {"=", IDC_BUTTON_EQUAL, 2, 3}, {"+", IDC_BUTTON_PLUS, 3, 3}
    };

    for (const auto& btn : buttons) {
        int xPos = startX + btn.x * (buttonWidth + spacingX);
        int yPos = startY + btn.y * (buttonHeight + spacingY);
        CreateWindow("BUTTON", btn.text,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            xPos, yPos, buttonWidth, buttonHeight,
            hwnd, (HMENU)btn.id, hInstance, NULL);
    }
}

void AppendToInput(const char* text) {
    int len = GetWindowTextLength(g_hwndInput);
    SendMessage(g_hwndInput, EM_SETSEL, len, len);
    SendMessage(g_hwndInput, EM_REPLACESEL, FALSE, (LPARAM)text);
}



// ============================================================
// [CUSTOMIZATION POINT]: History file I/O operations (save/load)
// ============================================================
const char* HISTORY_FILE = "history.csv";

void SaveHistory() {
    std::ofstream ofs(HISTORY_FILE);
    if (!ofs.is_open()) {
        // Handle error, e.g., log or show a message box
        return;
    }

    int count = ListView_GetItemCount(g_hwndHistory);
    for (int i = 0; i < count; ++i) {
        char expr_buf[256];
        char result_buf[256];
        char time_buf[256];

        ListView_GetItemText(g_hwndHistory, i, 0, expr_buf, sizeof(expr_buf));
        ListView_GetItemText(g_hwndHistory, i, 1, result_buf, sizeof(result_buf));
        ListView_GetItemText(g_hwndHistory, i, 2, time_buf, sizeof(time_buf));

        ofs << expr_buf << "," << result_buf << "," << time_buf << std::endl;
    }
    ofs.close();
}

void LoadHistory() {
    std::ifstream ifs(HISTORY_FILE);
    if (!ifs.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        std::string expression, result, timestamp;

        std::getline(ss, expression, ',');
        std::getline(ss, result, ',');
        std::getline(ss, timestamp);

        // Add to history list view
        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        lvi.iItem = ListView_GetItemCount(g_hwndHistory);
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPSTR>(expression.c_str());
        ListView_InsertItem(g_hwndHistory, &lvi);

        ListView_SetItemText(g_hwndHistory, lvi.iItem, 1, const_cast<LPSTR>(result.c_str()));
        ListView_SetItemText(g_hwndHistory, lvi.iItem, 2, const_cast<LPSTR>(timestamp.c_str()));
    }
    ifs.close();
}

// ============================================================
// [CUSTOMIZATION POINT]: System tray operations (minimize, restore, icon handling)
// ============================================================
void AddTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpy(nid.szTip, WINDOW_TITLE);
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hwnd) {
    NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowContextMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_RESTORE, "Restore");
    InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_EXIT, "Exit");

    SetForegroundWindow(hwnd); // Needed for the menu to disappear when clicking elsewhere
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

// ============================================================
// [CUSTOMIZATION POINT]: Keyboard hook installation and handling
// ============================================================
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKBDLLHookStruct = (KBDLLHOOKSTRUCT*)lParam;
        if (pKBDLLHookStruct->vkCode == VK_NUMLOCK) {
            if (wParam == WM_KEYDOWN) {
                // NumLock pressed
                if (GetForegroundWindow() == g_hwndMain) {
                    // App is focused, minimize to tray
                    ShowWindow(g_hwndMain, SW_HIDE);
                } else if (IsWindowVisible(g_hwndMain)) {
                    // App is open but not focused, bring to foreground
                    SetForegroundWindow(g_hwndMain);
                    ShowWindow(g_hwndMain, SW_RESTORE);
                } else {
                    // App is closed (hidden), open it
                    ShowWindow(g_hwndMain, SW_RESTORE);
                    SetForegroundWindow(g_hwndMain);
                }
            }
            // Always consume NumLock key press to prevent default behavior
            return 1;
        }
        // Force NumLock ON if it's off
        if (!(GetKeyState(VK_NUMLOCK) & 0x0001)) {
            SetNumLock(true);
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void InstallKeyboardHook() {
    // ============================================================
    // [CUSTOMIZATION POINT]: Keyboard hook installation and handling
    // ============================================================
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (g_keyboardHook == NULL) {
        MessageBox(NULL, "Failed to install keyboard hook!", "Error", MB_OK | MB_ICONERROR);
    }
}

void UninstallKeyboardHook() {
    // ============================================================
    // [CUSTOMIZATION POINT]: Keyboard hook installation and handling
    // ============================================================
    if (g_keyboardHook != NULL) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = NULL;
    }
}


// ============================================================
// [CUSTOMIZATION POINT]: NumLock monitoring and control logic
// ============================================================
void SetNumLock(bool state) {
    if (state != (GetKeyState(VK_NUMLOCK) & 0x0001)) {
        // Simulate a NumLock key press
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}

// ============================================================
// [CUSTOMIZATION POINT]: Window message handling
// ============================================================
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ACTIVATE:
            // Force NumLock ON when the window is activated
            if (LOWORD(wParam) != WA_INACTIVE) {
                SetNumLock(true);
            }
            break;
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            // ============================================================
            // [CUSTOMIZATION POINT]: WM_COMMAND message handling
            // ============================================================
            switch (wmId) {
                case IDC_BUTTON_0: AppendToInput("0"); break;
                case IDC_BUTTON_1: AppendToInput("1"); break;
                case IDC_BUTTON_2: AppendToInput("2"); break;
                case IDC_BUTTON_3: AppendToInput("3"); break;
                case IDC_BUTTON_4: AppendToInput("4"); break;
                case IDC_BUTTON_5: AppendToInput("5"); break;
                case IDC_BUTTON_6: AppendToInput("6"); break;
                case IDC_BUTTON_7: AppendToInput("7"); break;
                case IDC_BUTTON_8: AppendToInput("8"); break;
                case IDC_BUTTON_9: AppendToInput("9"); break;
                case IDC_BUTTON_PLUS: AppendToInput("+"); break;
                case IDC_BUTTON_MINUS: AppendToInput("-"); break;
                case IDC_BUTTON_MUL: AppendToInput("*"); break;
                case IDC_BUTTON_DIV: AppendToInput("/"); break;
                case IDC_BUTTON_LPAREN: AppendToInput("("); break;
                case IDC_BUTTON_RPAREN: AppendToInput(")"); break;
                case IDC_BUTTON_DOT: AppendToInput("."); break;
                case IDC_BUTTON_SQ: AppendToInput("^2"); break;
                case IDC_BUTTON_SQRT: AppendToInput("sqrt("); break;
                case IDC_BUTTON_C:
                    SetWindowText(g_hwndInput, "");
                    break;
                case IDC_BUTTON_CE:
                    SetWindowText(g_hwndInput, "");
                    break;
                case IDC_BUTTON_EQUAL:
                    EvaluateExpression();
                    break;
                case IDM_RESTORE:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    break;
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }
        break;
        case WM_KEYDOWN: {
            // ============================================================
            // [CUSTOMIZATION POINT]: Keyboard input handling
            // ============================================================
            switch (wParam) {
                case VK_NUMPAD0: AppendToInput("0"); break;
                case VK_NUMPAD1: AppendToInput("1"); break;
                case VK_NUMPAD2: AppendToInput("2"); break;
                case VK_NUMPAD3: AppendToInput("3"); break;
                case VK_NUMPAD4: AppendToInput("4"); break;
                case VK_NUMPAD5: AppendToInput("5"); break;
                case VK_NUMPAD6: AppendToInput("6"); break;
                case VK_NUMPAD7: AppendToInput("7"); break;
                case VK_NUMPAD8: AppendToInput("8"); break;
                case VK_NUMPAD9: AppendToInput("9"); break;
                case VK_ADD: AppendToInput("+"); break;
                case VK_SUBTRACT: AppendToInput("-"); break;
                case VK_MULTIPLY: AppendToInput("*"); break;
                case VK_DIVIDE: AppendToInput("/"); break;
                case VK_DECIMAL: AppendToInput("."); break;
                case VK_RETURN: EvaluateExpression(); break; // Enter key
                case VK_BACK: { // Backspace
                    int len = GetWindowTextLength(g_hwndInput);
                    if (len > 0) {
                        SendMessage(g_hwndInput, EM_SETSEL, len - 1, len);
                        SendMessage(g_hwndInput, EM_REPLACESEL, FALSE, (LPARAM)"");
                    }
                    break;
                }
            }
        }
        break;
        case WM_NOTIFY: {
            LPNMHDR lpnmhdr = (LPNMHDR)lParam;
            // ============================================================
            // [CUSTOMIZATION POINT]: History item selection/double-click handler
            // ============================================================
            if (lpnmhdr->hwndFrom == g_hwndHistory && lpnmhdr->code == NM_DBLCLK) {
                LPNMITEMACTIVATE lpnmi = (LPNMITEMACTIVATE)lParam;
                if (lpnmi->iItem != -1) {
                    char buffer[256];
                    ListView_GetItemText(g_hwndHistory, lpnmi->iItem, 0, buffer, sizeof(buffer));
                    SetWindowText(g_hwndInput, buffer);
                }
            }
        }
        break;
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED) {
                ShowWindow(hwnd, SW_HIDE);
            }
            break;
        case WM_TRAYICON:
            switch (LOWORD(lParam)) {
                case WM_RBUTTONUP:
                    ShowContextMenu(hwnd);
                    break;
                case WM_LBUTTONDBLCLK:
                    ShowWindow(hwnd, SW_RESTORE);
                    SetForegroundWindow(hwnd);
                    break;
            }
            break;
        case WM_DESTROY:
            SaveHistory(); // Save history on exit
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0; // All handled messages should return 0
}
