#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <fstream>
#include <shlobj.h> // For SHGetFolderPath
#include <map>
#include <cctype>
#include <cmath>
#include <gdiplus.h>

#include "tinyexpr.h"

#define ID_THEME_BUTTON 220 // New button ID
#define WM_TRAYICON (WM_USER + 1)
#define WM_SHOW_ERROR_MSGBOX (WM_USER + 2)
#define ID_HOTKEY 1
#define IDM_OPEN 1001 // New define
#define IDM_EXIT 1002 // New define
#define THEME_BUTTON_TXT "Theme"

// Theme colors
COLORREF dark_bg = RGB(43, 43, 43);
COLORREF dark_text = RGB(255, 255, 255);
COLORREF dark_btn_bg = RGB(51, 51, 51);
COLORREF dark_btn_text = RGB(255, 255, 255);
COLORREF dark_input_history_bg = RGB(35, 35, 35);
COLORREF dark_border_color = RGB(85, 85, 85);

COLORREF light_bg = RGB(0xFF, 0xFF, 0xFF);
COLORREF light_text = RGB(0x00, 0x00, 0x00);
COLORREF light_btn_bg = RGB(0xFF, 0xFF, 0xFF);
COLORREF light_btn_text = RGB(0x00, 0x00, 0x00);

bool isDarkTheme = false;
HBRUSH hbrDarkBkgnd = NULL;
HBRUSH hbrDarkBtn = NULL;
HBRUSH hbrDarkInputHistoryBkgnd = NULL;

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
WNDPROC originalEditProc; // For subclassing edit controls
std::string iconFilePath; // New global variable for icon path
bool isUpdatingInput = false; // Flag to prevent recursion in input formatting

Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

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

void applyTheme(); // Forward declaration

// Function to create and apply fonts
void createAndApplyFonts() {
    // Delete old fonts if they exist
    if (hNormalFont) DeleteObject(hNormalFont);
    if (hSmallBoldFont) DeleteObject(hSmallBoldFont);

    // Create hNormalFont (for display/input and buttons)
    LOGFONT lfNormal;
    ZeroMemory(&lfNormal, sizeof(LOGFONT));
    lfNormal.lfHeight = -14; // 14pt
    lfNormal.lfWeight = FW_NORMAL;
    strcpy_s(lfNormal.lfFaceName, LF_FACESIZE, "Segoe UI");
    hNormalFont = CreateFontIndirect(&lfNormal);

    // Create hSmallBoldFont (for history datetime part)
    LOGFONT lfSmallBold;
    ZeroMemory(&lfSmallBold, sizeof(LOGFONT));
    lfSmallBold.lfHeight = -12; // 12pt
    lfSmallBold.lfWeight = FW_BOLD;
    strcpy_s(lfSmallBold.lfFaceName, LF_FACESIZE, "Segoe UI");
    hSmallBoldFont = CreateFontIndirect(&lfSmallBold);

    // Apply fonts to controls
    if (hInput) SendMessage(hInput, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
    if (hHistory) {
        SendMessage(hHistory, WM_SETFONT, (WPARAM)hNormalFont, TRUE); // Apply normal font to history listbox
        // For owner-drawn listbox, the font for specific parts is set in WM_DRAWITEM
    }

    // Apply font to buttons
    for (int i = 200; i < 220; ++i) { // Assuming button IDs are 200-219
        HWND hButton = GetDlgItem(hWnd, i);
        if (hButton) {
            SendMessage(hButton, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
        }
    }
    // Apply font to theme button
    HWND hThemeButton = GetDlgItem(hWnd, ID_THEME_BUTTON);
    if (hThemeButton) {
        SendMessage(hThemeButton, WM_SETFONT, (WPARAM)hNormalFont, TRUE);
    }
}

void saveTheme() {
    std::string themeFilePath = getExecutableDirectory() + "\\theme.txt";
    std::ofstream outfile(themeFilePath);
    if (outfile.is_open()) {
        outfile << (isDarkTheme ? "dark" : "light");
        outfile.close();
    }
}

void loadTheme() {
    std::string themeFilePath = getExecutableDirectory() + "\\theme.txt";
    std::ifstream infile(themeFilePath);
    if (infile.is_open()) {
        std::string theme;
        infile >> theme;
        isDarkTheme = (theme == "dark");
        infile.close();
    }
}

// Function to get hotkey from file
int getHotkeyFromFile() {
    std::string hotkeyFilePath = getExecutableDirectory() + "\\hotkey.txt";
    std::ifstream infile(hotkeyFilePath);
    if (infile.is_open()) {
        std::string line;
        if (std::getline(infile, line)) {
            // Trim whitespace from the right
            size_t last = line.find_last_not_of(" \n\r\t");
            if (std::string::npos != last) {
                line = line.substr(0, last + 1);
            }

            // Trim whitespace from the left
            size_t first = line.find_first_not_of(" \n\r\t");
            if (std::string::npos != first) {
                line = line.substr(first);
            }
            
            static const std::map<std::string, int> keyMap = {
                {"VK_NUMLOCK", VK_NUMLOCK},
                {"VK_CAPITAL", VK_CAPITAL},
                {"VK_SCROLL", VK_SCROLL},
                {"VK_PRINT", VK_PRINT},
                {"VK_LAUNCH_APP1", VK_LAUNCH_APP1},
                {"VK_F8", VK_F8},
                {"VK_LAUNCH_APP2", VK_LAUNCH_APP2}
            };

            auto it = keyMap.find(line);
            if (it != keyMap.end()) {
                return it->second;
            }

            // If not in map, try to treat as hex value
            try {
                return std::stoul(line, nullptr, 16);
            } catch (const std::invalid_argument& ia) {
                // Not a valid hex, fall through to default
            } catch (const std::out_of_range& oor) {
                // Hex value out of range, fall through to default
            }
        }
        infile.close();
    }
    return VK_NUMLOCK; // Default
}

// Load history from file
void loadHistory() {
    std::ifstream infile(historyFilePath);
    if (infile.is_open()) {
        std::string line;
        std::string fullHistoryText;
        while (std::getline(infile, line)) {
            fullHistoryText += line + "\r\n"; // Append line and a newline for EDIT control
        }
        infile.close();
        SetWindowText(hHistory, fullHistoryText.c_str());
        // Scroll to the end
        SendMessage(hHistory, EM_SETSEL, 0, -1); // Select all
        SendMessage(hHistory, EM_SETSEL, -1, -1); // Deselect and move caret to end
        SendMessage(hHistory, EM_SCROLLCARET, 0, 0); // Scroll caret into view
    }
}

// Add to history
void addToHistory(const std::string& expression, const std::string& result) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buffer[512];
    
    // Format expression - add separators to numbers in the expression
    std::string formattedExpr = expression;
    // Note: expression already has separators from user input
    
    // Result already has separators from eval() function
    
    wsprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d: %s = %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, formattedExpr.c_str(), result.c_str());
    
    // Append to EDIT control
    int len = GetWindowTextLength(hHistory);
    SendMessage(hHistory, EM_SETSEL, len, len); // Set selection to end
    SendMessage(hHistory, EM_REPLACESEL, 0, (LPARAM)(std::string(buffer) + "\r\n").c_str()); // Append new text with newline
    SendMessage(hHistory, EM_SCROLLCARET, 0, 0); // Scroll caret into view

    // Also write to history file
    std::ofstream outfile(historyFilePath, std::ios_base::app);
    if (outfile.is_open()) {
        outfile << buffer << std::endl;
        outfile.close();
    }
}

// Remove thousands separators from string for calculation
std::string removeThousandsSeparator(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != '\'') {
            result += c;
        }
    }
    return result;
}

// Add thousands separator to number string
std::string addThousandsSeparator(const std::string& numStr) {
    std::string result = numStr;
    
    // Find decimal point position
    size_t decimalPos = result.find('.');
    size_t startPos = (decimalPos != std::string::npos) ? decimalPos : result.length();
    
    // Handle negative numbers
    size_t firstDigit = (result[0] == '-') ? 1 : 0;
    
    // Add separators from right to left in the integer part
    int count = 0;
    for (int i = (int)startPos - 1; i > (int)firstDigit; --i) {
        count++;
        if (count == 3) {
            result.insert(i, "'");
            count = 0;
        }
    }
    
    return result;
}

// New function to prepare expression for tinyexpr
std::string prepareExpression(const std::string& exp) {
    std::string result;
    result.reserve(exp.length() + 1); // Reserve some space

    for (size_t i = 0; i < exp.length(); ++i) {
        char c = exp[i];
        if (c == '.') {
            // Check if the previous character is not a digit
            if (i == 0 || !isdigit(exp[i - 1])) {
                result += '0';
            }
        }
        result += c;
    }
    return result;
}

std::pair<std::string, std::string> eval(const std::string& exp)
{
   // Remove thousands separators before evaluation
   std::string cleanExp = removeThousandsSeparator(exp);

   // Prepare expression for tinyexpr (e.g., .5 -> 0.5)
   std::string preparedExp = prepareExpression(cleanExp);
   
   if (preparedExp.empty()) {
       return {"", "0"};
   }

   int error_pos = 0;
   double r = te_interp(preparedExp.c_str(), &error_pos);
   
   if (error_pos != 0) {
       std::string error_msg = "Error at position " + std::to_string(error_pos);
       if (error_pos > 0 && (size_t)error_pos <= preparedExp.length() + 1) {
            std::string pointer;
            for(int i=0; i < error_pos -1; ++i) pointer += preparedExp[i];
            pointer += "^";
           error_msg = "Invalid expression.\n" + preparedExp + "\n" + pointer;
       }
       return {error_msg, ""};
   }
   
   char buffer[256];
   snprintf(buffer, sizeof(buffer), "%.12g", r);
   std::string val(buffer);
   
   // Add thousands separators to result
   return {"", addThousandsSeparator(val)};
}

// Evaluation function wireframe
std::pair<std::string, std::string> evaluateExpression(const std::string& expression_s) {
    return eval(expression_s);
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
    log("Slept 200 ms");
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
    SetFocus(hInput);
    // Select all text in the input field
    SendMessage(hInput, EM_SETSEL, 0, -1);
    log("Show from tray: Set numlock true");
    setNumlock(TRUE);
}

void MinimizeToTray() {
    ShowWindow(hWnd, SW_HIDE);
    log("Go to tray: Set numlock true");
    setNumlock(TRUE);
}

// Helper function to draw a rounded rectangle using GDI+
void DrawRoundedRect(Gdiplus::Graphics* graphics, Gdiplus::Pen* pen, Gdiplus::Brush* brush, float x, float y, float width, float height, float radius) {
    Gdiplus::GraphicsPath path;
    
    // Top-left corner
    path.AddArc(x, y, 2 * radius, 2 * radius, 180, 90);
    // Top-right corner
    path.AddArc(x + width - 2 * radius, y, 2 * radius, 2 * radius, 270, 90);
    // Bottom-right corner
    path.AddArc(x + width - 2 * radius, y + height - 2 * radius, 2 * radius, 2 * radius, 0, 90);
    // Bottom-left corner
    path.AddArc(x, y + height - 2 * radius, 2 * radius, 2 * radius, 90, 90);
    path.CloseFigure();

    graphics->FillPath(brush, &path);
    graphics->DrawPath(pen, &path);
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCPAINT: {
            // Get the window DC
            HDC hdc = GetWindowDC(hWnd);
            if (hdc) {
                RECT rect;
                GetWindowRect(hWnd, &rect);
                rect.right -= rect.left;
                rect.bottom -= rect.top;
                rect.left = 0;
                rect.top = 0;

                // Draw the custom border
                HPEN hPen = CreatePen(PS_SOLID, 1, dark_border_color);
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

                SelectObject(hdc, hOldPen);
                SelectObject(hdc, hOldBrush);
                DeleteObject(hPen);

                ReleaseDC(hWnd, hdc);
            }
            return 0; // Indicate that we handled the non-client painting
        }
        case WM_NCCALCSIZE: {
            // This message is sent when the size of the client area needs to be calculated.
            // We need to adjust the client area to make space for our custom border.
            // If we don't do this, the client area will overlap our border.
            LPNCCALCSIZE_PARAMS pncsp = (LPNCCALCSIZE_PARAMS)lParam;
            pncsp->rgrc[0].left += 1;
            pncsp->rgrc[0].top += 1;
            pncsp->rgrc[0].right -= 1;
            pncsp->rgrc[0].bottom -= 1;
            return 0;
        }
    }
    return CallWindowProc(originalEditProc, hWnd, uMsg, wParam, lParam);
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
                // Can't show MessageBox directly from a low-level hook.
                // Post a message to our own window and let it handle it.
                std::string* error_msg = new std::string(result.first);
                PostMessage(hWnd, WM_SHOW_ERROR_MSGBOX, 0, (LPARAM)error_msg);
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

    // Initialize GDI+
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize history file path
    historyFilePath = getExecutableDirectory() + "\\history.txt";
    iconFilePath = getExecutableDirectory() + "\\calculator.ico"; // Initialize icon file path

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = (HICON)LoadImage(NULL, iconFilePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL; // Set to NULL, handled by applyTheme
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "CalculatorClass";
    wcex.hIconSm = (HICON)LoadImage(NULL, iconFilePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    RegisterClassEx(&wcex);

    hWnd = CreateWindow("CalculatorClass", "Calculator", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 400, 600, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        return FALSE;
    }

    // Minimization to tray
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 100;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadImage(NULL, iconFilePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    lstrcpy(nid.szTip, "Calculator");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // Open/focus/close on numlock
    // Open/focus/close on hotkey
    int hotkey = getHotkeyFromFile();
    RegisterHotKey(hWnd, ID_HOTKEY, 0, hotkey);

    // Keyboard hook
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);


    // Create UI elements
    hInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 10, 10, 380, 40, hWnd, (HMENU)100, hInst, NULL);
    hHistory = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_RIGHT | ES_READONLY, 10, 60, 380, 190, hWnd, (HMENU)101, hInst, NULL);

    originalEditProc = (WNDPROC)SetWindowLongPtr(hInput, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
    SetWindowLongPtr(hHistory, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

    // Load history after creating the listbox
    loadHistory();

    // Buttons
    int buttonId = 200;
    const char* buttons[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+",
        "C", "(", ")", THEME_BUTTON_TXT
    };

    int x = 10, y = 260; // Updated start position for buttons
    int buttonWidth = 87;
    int buttonHeight = 60;
    int buttonSpacing = 10;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i * 4 + j < 20) {
                CreateWindow("BUTTON", buttons[i * 4 + j], WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 
                             x + j * (buttonWidth + buttonSpacing), 
                             y + i * (buttonHeight + buttonSpacing), 
                             buttonWidth, buttonHeight, hWnd, (HMENU)(UINT_PTR)(buttonId++), hInst, NULL);
            }
        }
    }
    
    createAndApplyFonts();
    loadTheme();
    applyTheme();

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

void applyTheme() {
    if (isDarkTheme) {
        if (hbrDarkBkgnd == NULL) hbrDarkBkgnd = CreateSolidBrush(dark_bg);
        if (hbrDarkBtn == NULL) hbrDarkBtn = CreateSolidBrush(dark_btn_bg);
        if (hbrDarkInputHistoryBkgnd == NULL) hbrDarkInputHistoryBkgnd = CreateSolidBrush(dark_input_history_bg);
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrDarkBkgnd);
    } else {
        if (hbrDarkBkgnd) {
            DeleteObject(hbrDarkBkgnd);
            hbrDarkBkgnd = NULL;
        }
        if (hbrDarkBtn) {
            DeleteObject(hbrDarkBtn);
            hbrDarkBtn = NULL;
        }
        if (hbrDarkInputHistoryBkgnd) {
            DeleteObject(hbrDarkInputHistoryBkgnd);
            hbrDarkInputHistoryBkgnd = NULL;
        }
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)(COLOR_WINDOW + 1));
    }

    // Redraw all child windows
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId;
    int wmEvent;
    switch (message) {
    case WM_CTLCOLORBTN: {
        if (isDarkTheme) {
            HDC hdcButton = (HDC)wParam;
            SetTextColor(hdcButton, dark_btn_text);
            SetBkColor(hdcButton, dark_btn_bg);
            return (LRESULT)hbrDarkBtn;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    case WM_CTLCOLOREDIT: { // For the input and history fields
        if (isDarkTheme) {
            HDC hdcEdit = (HDC)wParam;
            SetTextColor(hdcEdit, dark_text);
            SetBkColor(hdcEdit, dark_input_history_bg);
            return (LRESULT)hbrDarkInputHistoryBkgnd;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    case WM_CTLCOLORSTATIC: { // For static controls, if any
        if (isDarkTheme) {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, dark_text);
            SetBkColor(hdcStatic, dark_bg);
            return (LRESULT)hbrDarkBkgnd;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    case WM_SHOW_ERROR_MSGBOX: {
        std::string* error_msg = (std::string*)lParam;
        MessageBox(hWnd, error_msg->c_str(), "Error", MB_OK | MB_ICONERROR);
        delete error_msg;
        break;
    }
    case WM_SHOWWINDOW:
        if (wParam) { // Window is being shown
            SetFocus(hInput);
            // Select all text when window is shown
            SendMessage(hInput, EM_SETSEL, 0, -1);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND: { // Start of WM_COMMAND block
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        
        // Handle input field changes to add thousands separators
        if (wmId == 100 && wmEvent == EN_CHANGE && !isUpdatingInput) {
            char currentText[512];
            GetWindowText(hInput, currentText, 512);
            std::string text(currentText);
            
            // Only format if text contains digits
            if (!text.empty()) {
                std::string formatted;
                std::string currentNumber;
                bool inNumber = false;
                bool hasDecimal = false;
                
                for (size_t i = 0; i < text.length(); ++i) {
                    char c = text[i];
                    
                    if (c == '\'' ) {
                        // Skip existing separators
                        continue;
                    } else if (isdigit(c) || c == '.') {
                        if (c == '.') {
                            hasDecimal = true;
                        }
                        currentNumber += c;
                        inNumber = true;
                    } else {
                        // End of number - format it
                        if (inNumber && !currentNumber.empty()) {
                            formatted += addThousandsSeparator(currentNumber);
                            currentNumber.clear();
                            inNumber = false;
                            hasDecimal = false;
                        }
                        formatted += c;
                    }
                }
                
                // Format last number if exists
                if (inNumber && !currentNumber.empty()) {
                    formatted += addThousandsSeparator(currentNumber);
                }
                
                // Only update if changed
                if (formatted != text) {
                    // Get cursor position
                    DWORD startPos, endPos;
                    SendMessage(hInput, EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);
                    
                    // Calculate new cursor position (account for added separators)
                    int separatorsBeforeCursor = 0;
                    for (size_t i = 0; i < startPos && i < text.length(); ++i) {
                        if (text[i] == '\'') separatorsBeforeCursor++;
                    }
                    
                    int newSeparatorsBeforeCursor = 0;
                    for (size_t i = 0; i < formatted.length() && newSeparatorsBeforeCursor + i - newSeparatorsBeforeCursor < startPos - separatorsBeforeCursor; ++i) {
                        if (formatted[i] == '\'') newSeparatorsBeforeCursor++;
                    }
                    
                    DWORD newCursorPos = startPos - separatorsBeforeCursor + newSeparatorsBeforeCursor;
                    
                    isUpdatingInput = true;
                    SetWindowText(hInput, formatted.c_str());
                    SendMessage(hInput, EM_SETSEL, newCursorPos, newCursorPos);
                    isUpdatingInput = false;
                }
            }
        }
        
        switch (wmId) {
            case ID_THEME_BUTTON:
                isDarkTheme = !isDarkTheme;
                saveTheme();
                applyTheme();
                break;
            case IDM_OPEN:
                ShowWindowFromTray();
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                // Handle existing button clicks and listbox double clicks
                if (wmId >= 200) { // Button clicks
                    char buttonText[10];
                    GetDlgItemText(hWnd, wmId, buttonText, 10);
                    //if (buttonText == THEME_BUTTON_TXT){
                    if (strcmp(buttonText, THEME_BUTTON_TXT) == 0){
                        isDarkTheme = !isDarkTheme;
                        saveTheme();
                        applyTheme();
                        break;
                    }
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
                
                break;
        }
    } // End of WM_COMMAND block
    break;
    
    case WM_DRAWITEM: { // Start of WM_DRAWITEM block
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        // It's a button
            char buttonText[10];
            GetWindowText(dis->hwndItem, buttonText, 10);

            Gdiplus::Graphics graphics(dis->hDC);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

            Gdiplus::Color gdipBgColor, gdipTextColor, gdipBorderColor;
            
            // Base colors
            COLORREF baseBgColor = isDarkTheme ? dark_btn_bg : light_btn_bg;
            COLORREF baseTextColor = isDarkTheme ? dark_btn_text : light_btn_text;
            COLORREF baseBorderColor = isDarkTheme ? dark_border_color : RGB(170, 170, 170); // Light theme border

            // Hover/Pressed states
            if (dis->itemState & ODS_SELECTED) { // Button is pressed
                baseBgColor = isDarkTheme ? RGB(0x55, 0x55, 0x55) : RGB(0xDD, 0xDD, 0xDD);
            } else if (dis->itemState & ODS_HOTLIGHT) { // Button is hovered
                // Slightly lighter background on hover
                baseBgColor = isDarkTheme ? RGB(0x40, 0x40, 0x40) : RGB(0xF0, 0xF0, 0xF0);
            }

            gdipBgColor.SetFromCOLORREF(baseBgColor);
            gdipTextColor.SetFromCOLORREF(baseTextColor);
            gdipBorderColor.SetFromCOLORREF(baseBorderColor);

            Gdiplus::SolidBrush backgroundBrush(gdipBgColor);
            Gdiplus::Pen borderPen(gdipBorderColor, 1);
            
            float radius = 5.0f; // 5px rounded corners

            // Draw the rounded rectangle
            DrawRoundedRect(&graphics, &borderPen, &backgroundBrush, 
                            (float)dis->rcItem.left, (float)dis->rcItem.top, 
                            (float)(dis->rcItem.right - dis->rcItem.left), 
                            (float)(dis->rcItem.bottom - dis->rcItem.top), radius);

            // Draw the text
            Gdiplus::FontFamily fontFamily(L"Segoe UI");
            Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint); // 14pt font
            Gdiplus::SolidBrush textBrush(gdipTextColor);
            Gdiplus::StringFormat stringFormat;
            stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
            stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

            // Convert char* to WCHAR* for GDI+ DrawString
            std::string sButtonText(buttonText);
            std::wstring wsButtonText(sButtonText.begin(), sButtonText.end());

            graphics.DrawString(wsButtonText.c_str(), -1, &font, 
                                Gdiplus::RectF((float)dis->rcItem.left, (float)dis->rcItem.top, 
                                               (float)(dis->rcItem.right - dis->rcItem.left), 
                                               (float)(dis->rcItem.bottom - dis->rcItem.top)), 
                                &stringFormat, &textBrush);

            return TRUE;
        } // End of WM_DRAWITEM block
        break;
    }
    case WM_TIMER:
        // Numlock always on control
        setNumlock(TRUE);
        break;
    case WM_HOTKEY:
        // Open/focus/close on hotkey
        if (wParam == ID_HOTKEY) {
            if (itIsEnsureNumLockKeypress){
                itIsEnsureNumLockKeypress = 0;
            } else {
                //if (IsWindowVisible(hWnd)) {
                if ((hWnd == GetActiveWindow())){
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
        } else if (lParam == WM_RBUTTONUP) { // Handle right-click
            POINT curPoint;
            GetCursorPos(&curPoint); // Get current mouse position

            HMENU hMenu = CreatePopupMenu(); // Create a popup menu
            AppendMenu(hMenu, MF_STRING, IDM_OPEN, "Open"); // Add "Open" item
            AppendMenu(hMenu, MF_STRING, IDM_EXIT, "Exit"); // Add "Exit" item

            // Set the foreground window to our window so the menu will close automatically
            SetForegroundWindow(hWnd);

            // Display the menu
            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN,
                           curPoint.x, curPoint.y, 0, hWnd, NULL);

            // Destroy the menu when done
            DestroyMenu(hMenu);
        }
        break;
    case WM_CLOSE:
        MinimizeToTray();
        return 0;
    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE) {
            MinimizeToTray();
            return 0;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        UnhookWindowsHookEx(hKeyboardHook);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        UnregisterHotKey(hWnd, ID_HOTKEY);
        DeleteObject(hNormalFont);
        DeleteObject(hSmallBoldFont);
        if (hbrDarkBkgnd) DeleteObject(hbrDarkBkgnd);
        if (hbrDarkBtn) DeleteObject(hbrDarkBtn);
        if (hbrDarkInputHistoryBkgnd) DeleteObject(hbrDarkInputHistoryBkgnd);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}