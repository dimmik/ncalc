#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <map>
#include "expression.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_OPEN 1000
#define ID_TRAY_EXIT 1001

class Calculator {
private:
    HWND hWnd;
    HWND hInput;
    HWND hHistory;
    HFONT hFont;
    NOTIFYICONDATAW nid;
    UINT hotkey;
    bool minimizedToTray;
    std::vector<std::string> history;
    ExpressionEvaluator evaluator;
    
    static const int BUTTON_ROWS = 5;
    static const int BUTTON_COLS = 4;
    static const std::string BUTTON_LAYOUT[BUTTON_ROWS][BUTTON_COLS];
    
public:
    Calculator() : hWnd(nullptr), hInput(nullptr), hHistory(nullptr), hFont(nullptr),
                  hotkey(VK_NUMLOCK), minimizedToTray(false) {}
    
    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    void Cleanup();
    
private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT HandleMessage(UINT, WPARAM, LPARAM);
    void CreateControls();
    void ResizeControls();
    void OnButtonClick(int buttonId);
    void OnEvaluate();
    void OnClear();
    void OnHistoryDoubleClick();
    void LoadHistory();
    void SaveHistory(const std::string& expression, const std::string& result);
    void SetupTray();
    void RemoveTray();
    void RegisterGlobalHotkey();
    void UnregisterGlobalHotkey();
    void LoadHotkey();
    void EnableNumLock();
    std::string FormatInput(const std::string& input);
    std::string UnformatInput(const std::string& input);
    void UpdateInputFormatting();
    bool DrawHistoryItem(DRAWITEMSTRUCT* dis);
};

const std::string Calculator::BUTTON_LAYOUT[BUTTON_ROWS][BUTTON_COLS] = {
    {"C", "(", ")", "/"},
    {"7", "8", "9", "*"},
    {"4", "5", "6", "-"},
    {"1", "2", "3", "+"},
    {"0", ".", "=", "^"}
};

bool Calculator::Initialize(HINSTANCE hInstance, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CalculatorWindow";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = (HICON)LoadImageW(nullptr, L"calculator.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    
    if (!RegisterClassW(&wc)) return false;
    
    hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Calculator", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 600,
        nullptr, nullptr, hInstance, this
    );
    
    if (!hWnd) return false;
    
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
    
    hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    
    CreateControls();
    LoadHistory();
    LoadHotkey();
    RegisterGlobalHotkey();
    SetupTray();
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    EnableNumLock();
    
    return true;
}

void Calculator::CreateControls() {
    hInput = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        10, 10, 364, 30, hWnd, (HMENU)100, nullptr, nullptr
    );
    SendMessage(hInput, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    hHistory = CreateWindowExW(
        0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT,
        10, 50, 364, 200, hWnd, (HMENU)101, nullptr, nullptr
    );
    SendMessage(hHistory, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    for (int row = 0; row < BUTTON_ROWS; row++) {
        for (int col = 0; col < BUTTON_COLS; col++) {
            int buttonId = 200 + row * BUTTON_COLS + col;
            std::wstring buttonText(BUTTON_LAYOUT[row][col].begin(), BUTTON_LAYOUT[row][col].end());
            HWND hButton = CreateWindowW(
                L"BUTTON", buttonText.c_str(),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                10 + col * 90, 270 + row * 60, 85, 50,
                hWnd, (HMENU)buttonId, nullptr, nullptr
            );
            SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
    }
}

void Calculator::ResizeControls() {
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    
    SetWindowPos(hInput, nullptr, 10, 10, width - 20, 30, SWP_NOZORDER);
    SetWindowPos(hHistory, nullptr, 10, 50, width - 20, height - 320, SWP_NOZORDER);
    
    int buttonWidth = (width - 20) / BUTTON_COLS - 5;
    int buttonHeight = 50;
    int buttonStartY = height - (BUTTON_ROWS * (buttonHeight + 10)) - 10;
    
    for (int row = 0; row < BUTTON_ROWS; row++) {
        for (int col = 0; col < BUTTON_COLS; col++) {
            int buttonId = 200 + row * BUTTON_COLS + col;
            HWND hButton = GetDlgItem(hWnd, buttonId);
            if (hButton) {
                SetWindowPos(hButton, nullptr, 10 + col * (buttonWidth + 5), 
                           buttonStartY + row * (buttonHeight + 10), 
                           buttonWidth, buttonHeight, SWP_NOZORDER);
            }
        }
    }
}

LRESULT CALLBACK Calculator::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Calculator* pCalc = (Calculator*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (pCalc || uMsg == WM_CREATE) {
        return pCalc ? pCalc->HandleMessage(uMsg, wParam, lParam) : DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Calculator::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            ResizeControls();
            return 0;
            
        case WM_COMMAND:
            if (LOWORD(wParam) >= 200 && LOWORD(wParam) < 300) {
                OnButtonClick(LOWORD(wParam) - 200);
            } else if (LOWORD(wParam) == 100 && HIWORD(wParam) == EN_CHANGE) {
                UpdateInputFormatting();
            }
            return 0;
            
        case WM_DRAWITEM:
            if (wParam == 101) {
                return DrawHistoryItem((DRAWITEMSTRUCT*)lParam);
            }
            break;
            
        case WM_HOTKEY:
            if (IsWindowVisible(hWnd)) {
                ShowWindow(hWnd, SW_MINIMIZE);
                minimizedToTray = true;
                ShowWindow(hWnd, SW_HIDE);
            } else {
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
                EnableNumLock();
            }
            return 0;
            
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDBLCLK) {
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
                minimizedToTray = false;
                EnableNumLock();
            } else if (lParam == WM_RBUTTONDOWN) {
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_OPEN, L"Open");
                AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
                SetForegroundWindow(hWnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
                PostMessage(hWnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);
            } else if (LOWORD(wParam) == ID_TRAY_OPEN) {
                ShowWindow(hWnd, SW_RESTORE);
                SetForegroundWindow(hWnd);
                minimizedToTray = false;
                EnableNumLock();
            } else if (LOWORD(wParam) == ID_TRAY_EXIT) {
                DestroyWindow(hWnd);
            }
            return 0;
            
        case WM_SYSCOMMAND:
            if (wParam == SC_MINIMIZE) {
                minimizedToTray = true;
                ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
            break;
            
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage(0);
            return 0;
            
        case WM_CLOSE:
            if (minimizedToTray) {
                ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Calculator::OnButtonClick(int buttonIndex) {
    int row = buttonIndex / BUTTON_COLS;
    int col = buttonIndex % BUTTON_COLS;
    std::string button = BUTTON_LAYOUT[row][col];
    
    if (button == "=") {
        OnEvaluate();
    } else if (button == "C") {
        OnClear();
    } else {
        DWORD start, end;
        SendMessage(hInput, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
        int length = GetWindowTextLengthW(hInput);
        std::wstring current;
        current.resize(length + 1);
        GetWindowTextW(hInput, &current[0], length + 1);
        
        std::string str(current.begin(), current.end());
        str.insert(start, button);
        SetWindowTextW(hInput, std::wstring(str.begin(), str.end()).c_str());
        SendMessage(hInput, EM_SETSEL, start + button.length(), start + button.length());
    }
}

void Calculator::OnEvaluate() {
    int length = GetWindowTextLengthW(hInput);
    std::wstring expr;
    expr.resize(length + 1);
    GetWindowTextW(hInput, &expr[0], length + 1);
    
    std::string expression(expr.begin(), expr.end());
    std::string unformatted = UnformatInput(expression);
    std::string error;
    
    double result = evaluator.evaluate(unformatted, error);
    
    if (!error.empty()) {
        MessageBoxW(hWnd, std::wstring(error.begin(), error.end()).c_str(), L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    
    std::ostringstream resultStream;
    resultStream << std::fixed << std::setprecision(10) << result;
    std::string resultStr = resultStream.str();
    
    if (resultStr.find('.') != std::string::npos) {
        resultStr.erase(resultStr.find_last_not_of('0') + 1);
        if (resultStr.back() == '.') resultStr.pop_back();
    }
    
    SetWindowTextW(hInput, std::wstring(resultStr.begin(), resultStr.end()).c_str());
    SaveHistory(expression, resultStr);
}

void Calculator::OnClear() {
    SetWindowTextW(hInput, L"");
}

void Calculator::LoadHistory() {
    std::ifstream file("history.txt");
    std::string line;
    while (std::getline(file, line)) {
        history.push_back(line);
        std::wstring wline(line.begin(), line.end());
        SendMessageW(hHistory, LB_ADDSTRING, 0, (LPARAM)wline.c_str());
    }
    file.close();
}

void Calculator::SaveHistory(const std::string& expression, const std::string& result) {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string timestamp = oss.str();
    
    std::string entry = timestamp + ": [" + expression + "] = " + result;
    history.insert(history.begin(), entry);
    
    std::wstring wentry(entry.begin(), entry.end());
    SendMessageW(hHistory, LB_INSERTSTRING, 0, (LPARAM)wentry.c_str());
    
    std::ofstream file("history.txt", std::ios::app);
    file << entry << std::endl;
    file.close();
}

void Calculator::SetupTray() {
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadImageW(nullptr, L"calculator.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wcscpy_s(nid.szTip, L"Calculator");
    
    Shell_NotifyIconW(NIM_ADD, &nid);
}

void Calculator::RemoveTray() {
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void Calculator::RegisterGlobalHotkey() {
    RegisterHotKey(hWnd, 1, MOD_ALT, hotkey);
}

void Calculator::UnregisterGlobalHotkey() {
    UnregisterHotKey(hWnd, 1);
}

void Calculator::LoadHotkey() {
    std::ifstream file("hotkey.txt");
    if (file.is_open()) {
        std::string hotkeyStr;
        std::getline(file, hotkeyStr);
        file.close();
        
        if (hotkeyStr == "VK_NUMLOCK") hotkey = VK_NUMLOCK;
        else if (hotkeyStr == "VK_F8") hotkey = VK_F8;
        else if (hotkeyStr.find("0x") == 0) {
            hotkey = std::stoi(hotkeyStr, nullptr, 16);
        }
    }
}

void Calculator::EnableNumLock() {
    BYTE keyState[256];
    GetKeyboardState(keyState);
    if (!(keyState[VK_NUMLOCK] & 1)) {
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
        keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
}

std::string Calculator::FormatInput(const std::string& input) {
    std::string result;
    int digitCount = 0;
    bool inNumber = false;
    
    for (char c : input) {
        if (isdigit(c)) {
            if (!inNumber) {
                inNumber = true;
                digitCount = 0;
            }
            digitCount++;
            result += c;
            
            if (digitCount > 0 && digitCount % 3 == 0) {
                size_t pos = result.length() - 1;
                while (pos > 0 && !isdigit(result[pos - 1])) pos--;
                if (pos > 0) {
                    result.insert(pos, "'");
                    digitCount = 0;
                }
            }
        } else {
            inNumber = false;
            result += c;
        }
    }
    
    return result;
}

std::string Calculator::UnformatInput(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c != '\'') result += c;
    }
    
    if (result.length() > 0 && result[0] == '.') {
        result = "0" + result;
    }
    
    return result;
}

void Calculator::UpdateInputFormatting() {
    DWORD start, end;
    SendMessage(hInput, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    
    int length = GetWindowTextLengthW(hInput);
    std::wstring current;
    current.resize(length + 1);
    GetWindowTextW(hInput, &current[0], length + 1);
    
    std::string str(current.begin(), current.end());
    std::string formatted = FormatInput(str);
    
    if (str != formatted) {
        SetWindowTextW(hInput, std::wstring(formatted.begin(), formatted.end()).c_str());
        SendMessage(hInput, EM_SETSEL, start, end);
    }
}

bool Calculator::DrawHistoryItem(DRAWITEMSTRUCT* dis) {
    if (dis->itemID == -1) return true;
    
    std::wstring text;
    text.resize(256);
    int len = SendMessageW(hHistory, LB_GETTEXT, dis->itemID, (LPARAM)text.c_str());
    text.resize(len);
    
    std::string str(text.begin(), text.end());
    size_t timestampEnd = str.find(": ");
    if (timestampEnd == std::string::npos) return true;
    
    std::string timestamp = str.substr(0, timestampEnd);
    std::string calculation = str.substr(timestampEnd + 2);
    
    COLORREF oldTextColor = SetTextColor(dis->hDC, RGB(0, 0, 0));
    COLORREF oldBkColor = SetBkColor(dis->hDC, GetSysColor(COLOR_WINDOW));
    
    RECT rect = dis->rcItem;
    HFONT hBoldFont = CreateFontW(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    
    HFONT hOldFont = (HFONT)SelectObject(dis->hDC, hBoldFont);
    DrawTextW(dis->hDC, std::wstring(timestamp.begin(), timestamp.end()).c_str(), -1, &rect, DT_TOP | DT_LEFT);
    
    SIZE timestampSize;
    GetTextExtentPoint32W(dis->hDC, std::wstring(timestamp.begin(), timestamp.end()).c_str(), timestamp.length(), &timestampSize);
    
    SelectObject(dis->hDC, hFont);
    rect.left += timestampSize.cx + 5;
    DrawTextW(dis->hDC, std::wstring(calculation.begin(), calculation.end()).c_str(), -1, &rect, DT_TOP | DT_LEFT);
    
    SelectObject(dis->hDC, hOldFont);
    DeleteObject(hBoldFont);
    SetTextColor(dis->hDC, oldTextColor);
    SetBkColor(dis->hDC, oldBkColor);
    
    return true;
}

void Calculator::Run() {
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Calculator::Cleanup() {
    RemoveTray();
    UnregisterGlobalHotkey();
    if (hFont) DeleteObject(hFont);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    InitCommonControls();
    
    Calculator calc;
    if (!calc.Initialize(hInstance, nCmdShow)) {
        MessageBoxW(nullptr, L"Failed to initialize calculator", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    calc.Run();
    return 0;
}