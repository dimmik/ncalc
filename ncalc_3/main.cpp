#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <fstream>

#include "tinyexpr.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_HOTKEY_NUMLOCK 1

// Log function that appends msg to file c:\tmp\clog.txt
void log(const std::string& msg) {
    std::ofstream logfile("c:\\tmp\\clog.txt", std::ios_base::app);
    if (logfile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char buffer[128];
        wsprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        logfile << buffer << msg << std::endl;
    }
}

// Global variables
HINSTANCE hInst;
HWND hWnd;
HWND hInput;
HWND hHistory;
NOTIFYICONDATA nid;
HHOOK hKeyboardHook;


std::string eval(const std::string& exp)
{
   double r = te_interp(exp.c_str(), 0);
   std::string val = std::to_string(r);
   return val;
}

// Evaluation function wireframe
std::pair<std::string, std::string> evaluateExpression(const std::string& expression_s) {
    // This is a wireframe for the evaluation function.
    // It currently returns a fixed value.
    std::string val = eval(expression_s);
    return {"", val};
}

// Add to history
void addToHistory(const std::string& expression, const std::string& result) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buffer[256];
    wsprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d: %s = %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, expression.c_str(), result.c_str());
    SendMessage(hHistory, LB_INSERTSTRING, 0, (LPARAM)buffer);
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
    hHistory = CreateWindow("LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY, 10, 40, 360, 100, hWnd, (HMENU)101, hInst, NULL);

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
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
