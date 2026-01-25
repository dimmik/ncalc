#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <fstream>
#include <shlobj.h> // For SHGetFolderPath

#include "tinyexpr.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_HOTKEY_NUMLOCK 1

// Global variables
HINSTANCE hInst;
HWND hWnd;
HWND hInput;
HWND hHistory;
NOTIFYICONDATA nid;
HHOOK hKeyboardHook;
std::string historyFilePath;
HFONT hNormalFont = NULL;
HFONT hSmallBoldFont = NULL;

// Log function that appends msg to file c:\tmp\clog.txt
void log(const std::string& msg) {
    return;
    // keep for debug prpss
    std::ofstream logfile("c:\\tmp\\clog.txt", std::ios_base::app);
    if (logfile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buffer[128];
        wsprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        logfile << buffer << msg << std::endl;
    }
}

// Function to get the directory of the executable
std::string getExecutableDirectory() {
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::string s(path);
    return s.substr(0, s.find_last_of("\\/"));
}

// Load history from file
void loadHistory() {
    std::ifstream infile(historyFilePath);
    if (infile.is_open()) {
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(infile, line)) {
            lines.push_back(line);
        }
        infile.close();
        // Insert lines into the listbox in reverse order of reading, but at index 0
        // This will result in the newest entry (last in file) being at the top of the listbox
        for (int i = 0; i < lines.size(); ++i) {
            SendMessage(hHistory, LB_INSERTSTRING, 0, (LPARAM)lines[i].c_str());
        }
    }
}

// Add to history
void addToHistory(const std::string& expression, const std::string& result) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buffer[256];
    wsprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d: %s = %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, expression.c_str(), result.c_str());
    SendMessage(hHistory, LB_INSERTSTRING, 0, (LPARAM)buffer);

    // Also write to history file
    std::ofstream outfile(historyFilePath, std::ios_base::app);
    if (outfile.is_open()) {
        outfile << buffer << std::endl;
        outfile.close();
    }
}

std::string eval(const std::string& exp)
{
   double r = te_interp(exp.c_str(), 0);
   //std::string val = std::to_string(r);
   //std::string val = std::format("{:.2f}", r); c++ 20+
   
   char buffer[256];
   snprintf(buffer, sizeof(buffer), "%g", r);
   std::string val(buffer);
   return val;
}

// Evaluation function wireframe
std::pair<std::string, std::string> evaluateExpression(const std::string& expression_s) {
    // This is a wireframe for the evaluation function.
    // It currently returns a fixed value.
    std::string val = eval(expression_s);
    return {"", val};
}

void setInputText(const std::string& text) {
    SetWindowText(hInput, text.c_str());
    SendMessage(hInput, EM_SETSEL, text.length(), text.length());
    SetFocus(hInput);
}

// flag indicating that this is not real numlock press
int itIsEnsureNumLockKeypress = 0;
// Numlock always on control
DWORD WINAPI setNumlockThread(LPVOID lpParameter)
{
    BOOL bState = (BOOL)(uintptr_t)lpParameter;
    log("Check numlock");
    BYTE keyState[256];
    Sleep(200); // Pause for 100 milliseconds
    log("Slept 1000 ms");
    GetKeyboardState((LPBYTE)&keyState);
    if ((bState && !(keyState[VK_NUMLOCK] & 1)) ||
        (!bState && (keyState[VK_NUMLOCK] & 1)))
    {
        log("check: need to send emulating numlock");
        itIsEnsureNumLockKeypress = 1;
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    } else {
        log("check: Numlock is already on");
    }
    return 0;
}

void setNumlock(BOOL bState)
{
    HANDLE hThread = CreateThread(NULL, 0, setNumlockThread, (LPVOID)(uintptr_t)bState, 0, NULL);
    if (hThread) {
        CloseHandle(hThread);
    }
}

void ShowWindowFromTray() {
    ShowWindow(hWnd, SW_SHOW);
    ShowWindow(hWnd, SW_RESTORE);
    SetForegroundWindow(hWnd);
    log("Show from tray: Set numlock true");
    setNumlock(TRUE);
}

void MinimizeToTray() {
    ShowWindow(hWnd, SW_HIDE);
    log("Go to tray: Set numlock true");
    setNumlock(TRUE);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (!(hWnd == GetActiveWindow())) return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN && p->vkCode == VK_RETURN) {
            char currentText[256];
            GetWindowText(hInput, currentText, 256);
            // evaluation - where it is called
            auto result = evaluateExpression(currentText);
            if (result.first.empty()) {
                setInputText(result.second);
                addToHistory(currentText, result.second);
            } else {
                MessageBox(hWnd, result.first.c_str(), "Error", MB_OK | MB_ICONERROR);
            }
            return 1;
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    log("Application started");
    hInst = hInstance;

    // Initialize history file path
    historyFilePath = getExecutableDirectory() + "\\history.txt";

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, "calculator.ico");
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "CalculatorClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, "calculator.ico");
    RegisterClassEx(&wcex);

    hWnd = CreateWindow("CalculatorClass", "Calculator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 500, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        return FALSE;
    }

    // Minimization to tray
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 100;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInst, "calculator.ico");
    lstrcpy(nid.szTip, "Calculator");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // Open/focus/close on numlock
    RegisterHotKey(hWnd, ID_HOTKEY_NUMLOCK, 0, VK_NUMLOCK);

    // Keyboard hook
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);


    // Create UI elements
    hInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 10, 10, 360, 25, hWnd, (HMENU)100, hInst, NULL);
    hHistory = CreateWindow("LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS, 10, 40, 360, 100, hWnd, (HMENU)101, hInst, NULL);

    // Load history after creating the listbox
    loadHistory();

    // Buttons
    int buttonId = 200;
    const char* buttons[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+",
        "C", "(", ")", "sqr",
        "sqrt", "log", "ln", "a^b"
    };

    int x = 10, y = 150;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            CreateWindow("BUTTON", buttons[i * 4 + j], 
            WS_CHILD | WS_VISIBLE, x + j * 90, y + i * 50, 80, 40, 
            hWnd, (HMENU)(UINT_PTR)(buttonId++), hInst, NULL);
        }
    }
    
    // Numlock always on control
    //SetTimer(hWnd, 1, 500, NULL); // Check every second to enforce NumLock

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    
    setNumlock(TRUE);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SHOWWINDOW:
        if (wParam) { // Window is being shown
            SetFocus(hInput);
            int textLen = GetWindowTextLength(hInput);
            SendMessage(hInput, EM_SETSEL, textLen, textLen);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId >= 200) { // Button clicks
            char buttonText[10];
            GetDlgItemText(hWnd, wmId, buttonText, 10);
            std::string currentText(GetWindowTextLength(hInput) + 1, '\0');
            GetWindowText(hInput, &currentText[0], currentText.size());
            currentText.pop_back(); // remove null terminator

            if (strcmp(buttonText, "=") == 0) {
                // evaluation - where it is called
                auto result = evaluateExpression(currentText);
                if (result.first.empty()) {
                    setInputText(result.second);
                    addToHistory(currentText, result.second);
                } else {
                    MessageBox(hWnd, result.first.c_str(), "Error", MB_OK | MB_ICONERROR);
                }
            } else if (strcmp(buttonText, "C") == 0) {
                setInputText("");
            } else {
                setInputText(currentText + buttonText);
            }
        } else if (HIWORD(wParam) == LBN_DBLCLK) {
            int selected = SendMessage(hHistory, LB_GETCURSEL, 0, 0);
            if (selected != LB_ERR) {
                char buffer[256];
                SendMessage(hHistory, LB_GETTEXT, selected, (LPARAM)buffer);
                std::string historyLine(buffer);
                size_t pos = historyLine.find(" = ");
                if (pos != std::string::npos) {
                    setInputText(historyLine.substr(pos + 3));
                }
            }
        }
        break;
    }
    case WM_MEASUREITEM: {
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
        if (lpmis->CtlID == 101) { // hHistory listbox
            HDC hdc = GetDC(hWnd);
            
            // Get text metrics for normal font
            HFONT oldFont = (HFONT)SelectObject(hdc, hNormalFont);
            TEXTMETRIC tmNormal;
            GetTextMetrics(hdc, &tmNormal);
            int normalFontHeight = tmNormal.tmHeight + tmNormal.tmExternalLeading;
            
            // Get text metrics for small bold font
            SelectObject(hdc, hSmallBoldFont);
            TEXTMETRIC tmSmallBold;
            GetTextMetrics(hdc, &tmSmallBold);
            int smallBoldFontHeight = tmSmallBold.tmHeight + tmSmallBold.tmExternalLeading;
            
            // Use the maximum of the two heights
            lpmis->itemHeight = std::max(normalFontHeight, smallBoldFontHeight);
            
            SelectObject(hdc, oldFont); // Restore original font
            ReleaseDC(hWnd, hdc);
            return TRUE;
        }
        break;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == 101) { // hHistory listbox
            if (dis->itemID == -1) { // Empty item
                return TRUE;
            }

            char buffer[512];
            SendMessage(dis->hwndItem, LB_GETTEXT, dis->itemID, (LPARAM)buffer);
            std::string itemText(buffer);

            // Determine colors
            COLORREF textColor = (dis->itemState & ODS_SELECTED) ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
            COLORREF backgroundColor = (dis->itemState & ODS_SELECTED) ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW);

            SetBkColor(dis->hDC, backgroundColor);
            SetTextColor(dis->hDC, textColor);
            FillRect(dis->hDC, &dis->rcItem, CreateSolidBrush(backgroundColor));

            // Parse the string
            size_t eq_pos = itemText.find(" = ");
            std::string datetime_part;
            std::string expression_result_part;

            if (eq_pos != std::string::npos) {
                datetime_part = itemText.substr(0, eq_pos);
                expression_result_part = itemText.substr(eq_pos);
            } else {
                expression_result_part = itemText; // Fallback if " = " not found
            }

            // Get font metrics to calculate vertical centering
            TEXTMETRIC tmNormal;
            HFONT oldFont = (HFONT)SelectObject(dis->hDC, hNormalFont);
            GetTextMetrics(dis->hDC, &tmNormal);
            SelectObject(dis->hDC, hSmallBoldFont);
            TEXTMETRIC tmSmallBold;
            GetTextMetrics(dis->hDC, &tmSmallBold);
            SelectObject(dis->hDC, oldFont); // Restore original font

            int maxFontHeight = std::max(tmNormal.tmHeight + tmNormal.tmExternalLeading, tmSmallBold.tmHeight + tmSmallBold.tmExternalLeading);
            int y_offset = dis->rcItem.top + (dis->rcItem.bottom - dis->rcItem.top - maxFontHeight) / 2;
            if (y_offset < dis->rcItem.top) y_offset = dis->rcItem.top; // Ensure it doesn't go above the item top

            // Draw datetime part with small bold font
            oldFont = (HFONT)SelectObject(dis->hDC, hSmallBoldFont);
            RECT datetimeRect = {dis->rcItem.left, y_offset, dis->rcItem.right, dis->rcItem.bottom};
            DrawText(dis->hDC, datetime_part.c_str(), -1, &datetimeRect, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);

            // Calculate position for expression/result part
            SIZE datetimeSize;
            GetTextExtentPoint32(dis->hDC, datetime_part.c_str(), datetime_part.length(), &datetimeSize);
            int x_offset = dis->rcItem.left + datetimeSize.cx;

            // Draw expression/result part with normal font
            SelectObject(dis->hDC, hNormalFont);
            RECT exprRect = {x_offset, y_offset, dis->rcItem.right, dis->rcItem.bottom};
            DrawText(dis->hDC, expression_result_part.c_str(), -1, &exprRect, DT_LEFT | DT_SINGLELINE | DT_NOCLIP);

            SelectObject(dis->hDC, oldFont); // Restore original font

            if (dis->itemState & ODS_FOCUS) {
                DrawFocusRect(dis->hDC, &dis->rcItem);
            }
            return TRUE;
        }
        break;
    }
    case WM_TIMER:
        // Numlock always on control
        setNumlock(TRUE);
        break;
    case WM_HOTKEY:
        // Open/focus/close on numlock
        if (wParam == ID_HOTKEY_NUMLOCK) {
            if (itIsEnsureNumLockKeypress){
                itIsEnsureNumLockKeypress = 0;
            } else {
                if (IsWindowVisible(hWnd)) {
                    MinimizeToTray();
                } else {
                    ShowWindowFromTray();
                }
            }
        }
        break;
    case WM_TRAYICON:
        if (lParam == WM_LBUTTONDBLCLK) {
            ShowWindowFromTray();
        }
        break;
    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE) {
            MinimizeToTray();
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        UnhookWindowsHookEx(hKeyboardHook);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        UnregisterHotKey(hWnd, ID_HOTKEY_NUMLOCK);
        DeleteObject(hNormalFont);
        DeleteObject(hSmallBoldFont);
        PostQuitMessage(0);
        break;
    case WM_SIZE: {
        // Get the new width and height of the client area
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        // Delete old fonts if they exist
        if (hNormalFont) DeleteObject(hNormalFont);
        if (hSmallBoldFont) DeleteObject(hSmallBoldFont);

        // Create new fonts based on window height
        LOGFONT lf;
        GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        
        // Base font sizes for a window height of 500 pixels
        int baseNormalFontSize = 18;
        int baseSmallBoldFontSize = 14;
        int initialWindowHeight = 500; // The initial height of the window

        // Calculate scaled font heights
        int scaledNormalHeight = -MulDiv(baseNormalFontSize, newHeight, initialWindowHeight);
        int scaledSmallBoldHeight = -MulDiv(baseSmallBoldFontSize, newHeight, initialWindowHeight);

        // Apply reasonable min/max limits
        if (scaledNormalHeight > -10) scaledNormalHeight = -10; // Min 10pt
        if (scaledNormalHeight < -36) scaledNormalHeight = -36; // Max 36pt

        if (scaledSmallBoldHeight > -8) scaledSmallBoldHeight = -8; // Min 8pt
        if (scaledSmallBoldHeight < -28) scaledSmallBoldHeight = -28; // Max 28pt

        lf.lfHeight = scaledNormalHeight;
        hNormalFont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
                                 lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
                                 lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
                                 lf.lfPitchAndFamily, lf.lfFaceName);

        lf.lfHeight = scaledSmallBoldHeight;
        lf.lfWeight = FW_BOLD;
        hSmallBoldFont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
                                    lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
                                    lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
                                    lf.lfPitchAndFamily, lf.lfFaceName);

        // Apply normal font to hInput
        SendMessage(hInput, WM_SETFONT, (WPARAM)hNormalFont, TRUE);

        // Calculate scaled padding
        initialWindowHeight = 500; // Use the same initial height as for fonts
        int baseButtonPadding = 10;
        int scaledButtonPadding = MulDiv(baseButtonPadding, newHeight, initialWindowHeight);
        if (scaledButtonPadding < 5) scaledButtonPadding = 5; // Minimum padding
        if (scaledButtonPadding > 20) scaledButtonPadding = 20; // Maximum padding

        // Resize hInput
        MoveWindow(hInput, scaledButtonPadding, scaledButtonPadding, newWidth - 2 * scaledButtonPadding, scaledNormalHeight * -1 + scaledButtonPadding, TRUE); // Use scaled font height for input height

        // Resize hHistory
        int historyTop = scaledButtonPadding + (scaledNormalHeight * -1 + scaledButtonPadding) + scaledButtonPadding; // Below input + padding
        int minHistoryHeight = 50; // Minimum height for history
        
        // Calculate remaining height for buttons
        int numRows = 6;
        int numCols = 4;
        int estimatedButtonHeight = (scaledNormalHeight * -1) + scaledButtonPadding; // Estimate button height based on font
        int totalButtonAreaHeight = numRows * estimatedButtonHeight + (numRows + 1) * scaledButtonPadding;

        int historyHeight = newHeight - historyTop - totalButtonAreaHeight - scaledButtonPadding;
        if (historyHeight < minHistoryHeight) historyHeight = minHistoryHeight;

        MoveWindow(hHistory, scaledButtonPadding, historyTop, newWidth - 2 * scaledButtonPadding, historyHeight, TRUE);
        // Calculate new item height for the listbox
        HDC hdc = GetDC(hWnd);
        HFONT oldFont = (HFONT)SelectObject(hdc, hNormalFont);
        TEXTMETRIC tmNormal;
        GetTextMetrics(hdc, &tmNormal);
        int normalFontHeight = tmNormal.tmHeight + tmNormal.tmExternalLeading;
        
        SelectObject(hdc, hSmallBoldFont);
        TEXTMETRIC tmSmallBold;
        GetTextMetrics(hdc, &tmSmallBold);
        int smallBoldFontHeight = tmSmallBold.tmHeight + tmSmallBold.tmExternalLeading;
        
        int newCalculatedItemHeight = std::max(normalFontHeight, smallBoldFontHeight);
        SelectObject(hdc, oldFont);
        ReleaseDC(hWnd, hdc);

        // Update the listbox item height
        SendMessage(hHistory, LB_SETITEMHEIGHT, 0, newCalculatedItemHeight);
        InvalidateRect(hHistory, NULL, TRUE); // Force redraw with new fonts and item height

        // Reposition and resize buttons
        int buttonId = 200;
        int startY = historyTop + historyHeight + scaledButtonPadding;

        int totalButtonWidth = newWidth - (numCols + 1) * scaledButtonPadding;
        int buttonWidth = totalButtonWidth / numCols;
        int totalButtonHeight = newHeight - startY - scaledButtonPadding;
        int buttonHeight = totalButtonHeight / numRows;

        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                HWND hButton = GetDlgItem(hWnd, buttonId + (i * numCols + j));
                if (hButton) {
                    SendMessage(hButton, WM_SETFONT, (WPARAM)hNormalFont, TRUE); // Apply font to button
                    MoveWindow(hButton, 
                               scaledButtonPadding + j * (buttonWidth + scaledButtonPadding), 
                               startY + i * (buttonHeight + scaledButtonPadding), 
                               buttonWidth, 
                               buttonHeight, 
                               TRUE);
                }
            }
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
