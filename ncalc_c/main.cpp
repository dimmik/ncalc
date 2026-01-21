#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <stack>
#include <map>
#include <shellapi.h>

#define IDC_DISPLAY 101
#define IDC_BUTTON_0 102
#define IDC_BUTTON_1 103
#define IDC_BUTTON_2 104
#define IDC_BUTTON_3 105
#define IDC_BUTTON_4 106
#define IDC_BUTTON_5 107
#define IDC_BUTTON_6 108
#define IDC_BUTTON_7 109
#define IDC_BUTTON_8 110
#define IDC_BUTTON_9 111
#define IDC_BUTTON_ADD 112
#define IDC_BUTTON_SUB 113
#define IDC_BUTTON_MUL 114
#define IDC_BUTTON_DIV 115
#define IDC_BUTTON_EQ 116
#define IDC_BUTTON_C 117
#define IDC_BUTTON_CE 118
#define IDC_BUTTON_DOT 119
#define IDC_BUTTON_SQRT 120
#define IDC_BUTTON_SQR 121
#define IDC_BUTTON_LOG 122
#define IDC_BUTTON_NEG 123
#define IDC_BUTTON_BS 124
#define IDC_HISTORY 125

#define WM_TRAYICON (WM_USER + 1)
#define HOTKEY_ID 1

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
double EvaluateExpression(const std::wstring& expression);
void CreateTrayIcon(HWND hwnd);
void DeleteTrayIcon(HWND hwnd);
void AddToHistory(HWND hwndHistory, const std::wstring& calculation);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"NumlockCalcWindowClass";

    WNDCLASSW wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    // Create the window.
    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Numlock Calculator",          // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 320,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void AppendToDisplay(HWND hwndDisplay, const std::wstring& text, bool isOperator)
{
    int len = GetWindowTextLength(hwndDisplay);
    if (len == 1 && GetWindowTextLength(hwndDisplay) > 0)
    {
        wchar_t firstChar[2];
        GetWindowTextW(hwndDisplay, firstChar, 2);
        if (firstChar[0] == L'0' && !isOperator)
        {
            SetWindowTextW(hwndDisplay, text.c_str());
            return;
        }
    }
    SendMessage(hwndDisplay, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessage(hwndDisplay, EM_REPLACESEL, 0, (LPARAM)text.c_str());
}
// flag to distinguish numlock ensure from numlock press
int numlock_ensure = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndDisplay, hwndHistory;
    static HFONT hFont;
    static std::vector<std::wstring> history;
    static bool isNewCalculation = true;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

        hwndDisplay = CreateWindowExW(
            0, L"EDIT", L"0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_READONLY,
            10, 10, 225, 30,
            hwnd, (HMENU)IDC_DISPLAY, NULL, NULL);
        SendMessage(hwndDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

        hwndHistory = CreateWindowExW(
            0, L"LISTBOX", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            250, 10, 130, 260,
            hwnd, (HMENU)IDC_HISTORY, NULL, NULL);
        SendMessage(hwndHistory, WM_SETFONT, (WPARAM)hFont, TRUE);


        // Number buttons
        CreateWindowW(L"BUTTON", L"7", WS_CHILD | WS_VISIBLE, 10, 50, 40, 40, hwnd, (HMENU)IDC_BUTTON_7, NULL, NULL);
        CreateWindowW(L"BUTTON", L"8", WS_CHILD | WS_VISIBLE, 55, 50, 40, 40, hwnd, (HMENU)IDC_BUTTON_8, NULL, NULL);
        CreateWindowW(L"BUTTON", L"9", WS_CHILD | WS_VISIBLE, 100, 50, 40, 40, hwnd, (HMENU)IDC_BUTTON_9, NULL, NULL);
        CreateWindowW(L"BUTTON", L"4", WS_CHILD | WS_VISIBLE, 10, 95, 40, 40, hwnd, (HMENU)IDC_BUTTON_4, NULL, NULL);
        CreateWindowW(L"BUTTON", L"5", WS_CHILD | WS_VISIBLE, 55, 95, 40, 40, hwnd, (HMENU)IDC_BUTTON_5, NULL, NULL);
        CreateWindowW(L"BUTTON", L"6", WS_CHILD | WS_VISIBLE, 100, 95, 40, 40, hwnd, (HMENU)IDC_BUTTON_6, NULL, NULL);
        CreateWindowW(L"BUTTON", L"1", WS_CHILD | WS_VISIBLE, 10, 140, 40, 40, hwnd, (HMENU)IDC_BUTTON_1, NULL, NULL);
        CreateWindowW(L"BUTTON", L"2", WS_CHILD | WS_VISIBLE, 55, 140, 40, 40, hwnd, (HMENU)IDC_BUTTON_2, NULL, NULL);
        CreateWindowW(L"BUTTON", L"3", WS_CHILD | WS_VISIBLE, 100, 140, 40, 40, hwnd, (HMENU)IDC_BUTTON_3, NULL, NULL);
        CreateWindowW(L"BUTTON", L"0", WS_CHILD | WS_VISIBLE, 10, 185, 85, 40, hwnd, (HMENU)IDC_BUTTON_0, NULL, NULL);

        // Operator buttons
        CreateWindowW(L"BUTTON", L"/", WS_CHILD | WS_VISIBLE, 145, 50, 40, 40, hwnd, (HMENU)IDC_BUTTON_DIV, NULL, NULL);
        CreateWindowW(L"BUTTON", L"*", WS_CHILD | WS_VISIBLE, 145, 95, 40, 40, hwnd, (HMENU)IDC_BUTTON_MUL, NULL, NULL);
        CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE, 145, 140, 40, 40, hwnd, (HMENU)IDC_BUTTON_SUB, NULL, NULL);
        CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE, 145, 185, 40, 40, hwnd, (HMENU)IDC_BUTTON_ADD, NULL, NULL);
        CreateWindowW(L"BUTTON", L"=", WS_CHILD | WS_VISIBLE, 190, 185, 45, 40, hwnd, (HMENU)IDC_BUTTON_EQ, NULL, NULL);

        // Control buttons
        CreateWindowW(L"BUTTON", L"C", WS_CHILD | WS_VISIBLE, 190, 50, 45, 40, hwnd, (HMENU)IDC_BUTTON_C, NULL, NULL);
        CreateWindowW(L"BUTTON", L"CE", WS_CHILD | WS_VISIBLE, 190, 95, 45, 40, hwnd, (HMENU)IDC_BUTTON_CE, NULL, NULL);
        CreateWindowW(L"BUTTON", L".", WS_CHILD | WS_VISIBLE, 100, 185, 40, 40, hwnd, (HMENU)IDC_BUTTON_DOT, NULL, NULL);
        CreateWindowW(L"BUTTON", L"<-", WS_CHILD | WS_VISIBLE, 190, 140, 45, 40, hwnd, (HMENU)IDC_BUTTON_BS, NULL, NULL);

        // Scientific buttons
        CreateWindowW(L"BUTTON", L"sqrt", WS_CHILD | WS_VISIBLE, 10, 230, 50, 40, hwnd, (HMENU)IDC_BUTTON_SQRT, NULL, NULL);
        CreateWindowW(L"BUTTON", L"x^2", WS_CHILD | WS_VISIBLE, 65, 230, 50, 40, hwnd, (HMENU)IDC_BUTTON_SQR, NULL, NULL);
        CreateWindowW(L"BUTTON", L"log", WS_CHILD | WS_VISIBLE, 120, 230, 50, 40, hwnd, (HMENU)IDC_BUTTON_LOG, NULL, NULL);
        CreateWindowW(L"BUTTON", L"+/-", WS_CHILD | WS_VISIBLE, 175, 230, 60, 40, hwnd, (HMENU)IDC_BUTTON_NEG, NULL, NULL);

        // Set font for all buttons
        for (int i = IDC_BUTTON_0; i <= IDC_BUTTON_NEG; i++)
        {
            SendMessage(GetDlgItem(hwnd, i), WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        if (!RegisterHotKey(hwnd, HOTKEY_ID, 0, VK_NUMLOCK))
        {
            MessageBox(hwnd, L"Failed to register hotkey.", L"Error", MB_OK | MB_ICONERROR);
        }
        SetTimer(hwnd, 1, 100, NULL);
        CreateTrayIcon(hwnd);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (HIWORD(wParam) == LBN_DBLCLK)
        {
            int selected = SendMessage(hwndHistory, LB_GETCURSEL, 0, 0);
            if (selected != LB_ERR)
            {
                wchar_t buffer[256];
                SendMessage(hwndHistory, LB_GETTEXT, selected, (LPARAM)buffer);
                std::wstring history_entry(buffer);
                size_t pos = history_entry.find(L" =");
                if (pos != std::wstring::npos)
                {
                    SetWindowTextW(hwndDisplay, history_entry.substr(0, pos).c_str());
                }
            }
        }

        if (isNewCalculation && wmId >= IDC_BUTTON_0 && wmId <= IDC_BUTTON_DOT)
        {
            SetWindowTextW(hwndDisplay, L"");
            isNewCalculation = false;
        }

        switch (wmId)
        {
        case IDC_BUTTON_0: AppendToDisplay(hwndDisplay, L"0", false); break;
        case IDC_BUTTON_1: AppendToDisplay(hwndDisplay, L"1", false); break;
        case IDC_BUTTON_2: AppendToDisplay(hwndDisplay, L"2", false); break;
        case IDC_BUTTON_3: AppendToDisplay(hwndDisplay, L"3", false); break;
        case IDC_BUTTON_4: AppendToDisplay(hwndDisplay, L"4", false); break;
        case IDC_BUTTON_5: AppendToDisplay(hwndDisplay, L"5", false); break;
        case IDC_BUTTON_6: AppendToDisplay(hwndDisplay, L"6", false); break;
        case IDC_BUTTON_7: AppendToDisplay(hwndDisplay, L"7", false); break;
        case IDC_BUTTON_8: AppendToDisplay(hwndDisplay, L"8", false); break;
        case IDC_BUTTON_9: AppendToDisplay(hwndDisplay, L"9", false); break;
        case IDC_BUTTON_DOT: AppendToDisplay(hwndDisplay, L".", false); break;
        case IDC_BUTTON_ADD: AppendToDisplay(hwndDisplay, L" + ", true); break;
        case IDC_BUTTON_SUB: AppendToDisplay(hwndDisplay, L" - ", true); break;
        case IDC_BUTTON_MUL: AppendToDisplay(hwndDisplay, L" * ", true); break;
        case IDC_BUTTON_DIV: AppendToDisplay(hwndDisplay, L" / ", true); break;
        case IDC_BUTTON_C:
            SetWindowTextW(hwndDisplay, L"0");
            isNewCalculation = true;
            break;
        case IDC_BUTTON_CE:
            SetWindowTextW(hwndDisplay, L"0");
            isNewCalculation = true;
            break;
        case IDC_BUTTON_BS:
        {
            int len = GetWindowTextLength(hwndDisplay);
            if (len > 0)
            {
                wchar_t* buffer = new wchar_t[len + 1];
                GetWindowTextW(hwndDisplay, buffer, len + 1);
                std::wstring text(buffer);
                delete[] buffer;
                text.pop_back();
                if (text.empty())
                {
                    text = L"0";
                    isNewCalculation = true;
                }
                SetWindowTextW(hwndDisplay, text.c_str());
            }
        }
        break;
        case IDC_BUTTON_EQ:
        {
            int len = GetWindowTextLength(hwndDisplay);
            wchar_t* buffer = new wchar_t[len + 1];
            GetWindowTextW(hwndDisplay, buffer, len + 1);
            std::wstring currentCalculation(buffer);
            delete[] buffer;

            try
            {
                double result = EvaluateExpression(currentCalculation);
                std::wstring resultStr = std::to_wstring(result);
                SetWindowTextW(hwndDisplay, resultStr.c_str());
                AddToHistory(hwndHistory, currentCalculation + L" = " + resultStr);
            }
            catch (const std::exception& e)
            {
                MessageBoxA(hwnd, e.what(), "Error", MB_OK | MB_ICONERROR);
                SetWindowTextW(hwndDisplay, L"0");
            }
            isNewCalculation = true;
        }
        break;
        case IDC_BUTTON_SQRT:
        {
            int len = GetWindowTextLength(hwndDisplay);
            wchar_t* buffer = new wchar_t[len + 1];
            GetWindowTextW(hwndDisplay, buffer, len + 1);
            double num = _wtof(buffer);
            delete[] buffer;
            if (num >= 0)
            {
                double result = sqrt(num);
                SetWindowTextW(hwndDisplay, std::to_wstring(result).c_str());
            }
            else
            {
                MessageBoxW(hwnd, L"Cannot take the square root of a negative number.", L"Error", MB_OK | MB_ICONERROR);
            }
            isNewCalculation = true;
        }
        break;
        case IDC_BUTTON_SQR:
        {
            int len = GetWindowTextLength(hwndDisplay);
            wchar_t* buffer = new wchar_t[len + 1];
            GetWindowTextW(hwndDisplay, buffer, len + 1);
            double num = _wtof(buffer);
            delete[] buffer;
            double result = num * num;
            SetWindowTextW(hwndDisplay, std::to_wstring(result).c_str());
            isNewCalculation = true;
        }
        break;
        case IDC_BUTTON_LOG:
        {
            int len = GetWindowTextLength(hwndDisplay);
            wchar_t* buffer = new wchar_t[len + 1];
            GetWindowTextW(hwndDisplay, buffer, len + 1);
            double num = _wtof(buffer);
            delete[] buffer;
            if (num > 0)
            {
                double result = log10(num);
                SetWindowTextW(hwndDisplay, std::to_wstring(result).c_str());
            }
            else
            {
                MessageBoxW(hwnd, L"Cannot take the logarithm of a non-positive number.", L"Error", MB_OK | MB_ICONERROR);
            }
            isNewCalculation = true;
        }
        break;
        case IDC_BUTTON_NEG:
        {
            int len = GetWindowTextLength(hwndDisplay);
            wchar_t* buffer = new wchar_t[len + 1];
            GetWindowTextW(hwndDisplay, buffer, len + 1);
            double num = _wtof(buffer);
            delete[] buffer;
            double result = -num;
            SetWindowTextW(hwndDisplay, std::to_wstring(result).c_str());
            isNewCalculation = true;
        }
        break;
        }
    }
    break;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_NUMPAD0: AppendToDisplay(hwndDisplay, L"0", false); break;
        case VK_NUMPAD1: AppendToDisplay(hwndDisplay, L"1", false); break;
        case VK_NUMPAD2: AppendToDisplay(hwndDisplay, L"2", false); break;
        case VK_NUMPAD3: AppendToDisplay(hwndDisplay, L"3", false); break;
        case VK_NUMPAD4: AppendToDisplay(hwndDisplay, L"4", false); break;
        case VK_NUMPAD5: AppendToDisplay(hwndDisplay, L"5", false); break;
        case VK_NUMPAD6: AppendToDisplay(hwndDisplay, L"6", false); break;
        case VK_NUMPAD7: AppendToDisplay(hwndDisplay, L"7", false); break;
        case VK_NUMPAD8: AppendToDisplay(hwndDisplay, L"8", false); break;
        case VK_NUMPAD9: AppendToDisplay(hwndDisplay, L"9", false); break;
        case VK_DECIMAL: AppendToDisplay(hwndDisplay, L".", false); break;
        case VK_ADD: AppendToDisplay(hwndDisplay, L" + ", true); break;
        case VK_SUBTRACT: AppendToDisplay(hwndDisplay, L" - ", true); break;
        case VK_MULTIPLY: AppendToDisplay(hwndDisplay, L" * ", true); break;
        case VK_DIVIDE: AppendToDisplay(hwndDisplay, L" / ", true); break;
        case VK_RETURN: PostMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQ, 0); break;
        case VK_BACK: PostMessage(hwnd, WM_COMMAND, IDC_BUTTON_BS, 0); break;
        }
    }
    break;

    case WM_DESTROY:
        DeleteObject(hFont);
        UnregisterHotKey(hwnd, HOTKEY_ID);
        KillTimer(hwnd, 1);
        DeleteTrayIcon(hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_HOTKEY:
        if (wParam == HOTKEY_ID)
        {
            if (numlock_ensure == 1){
                numlock_ensure = 0;
            } else {
                if (IsWindowVisible(hwnd))
                    ShowWindow(hwnd, SW_HIDE);
                else
                    ShowWindow(hwnd, SW_SHOW);
            }
        }
        break;

    case WM_TIMER:
        if ((GetKeyState(VK_NUMLOCK) & 1) == 0)
        {
            numlock_ensure = 1;
            keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
            keybd_event(VK_NUMLOCK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        }
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE)
        {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);

    case WM_TRAYICON:
        if (lParam == WM_LBUTTONUP)
        {
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);
        }
        break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int getPrecedence(const std::wstring& op) {
    if (op == L"+" || op == L"-") return 1;
    if (op == L"*" || op == L"/") return 2;
    return 0;
}

double applyOp(double a, double b, const std::wstring& op) {
    if (op == L"+") return a + b;
    if (op == L"-") return a - b;
    if (op == L"*") return a * b;
    if (op == L"/") {
        if (b == 0) throw std::runtime_error("Division by zero");
        return a / b;
    }
    return 0;
}

double EvaluateExpression(const std::wstring& expression) {
    std::wstringstream ss(expression);
    std::wstring token;
    std::vector<std::wstring> tokens;
    while (ss >> token) {
        tokens.push_back(token);
    }

    std::stack<double> values;
    std::stack<std::wstring> ops;

    for (const auto& token : tokens) {
        if (iswdigit(token[0]) || (token.length() > 1 && (iswdigit(token[1]) || token[1] == L'.'))) {
            values.push(std::stod(token));
        }
        else {
            while (!ops.empty() && getPrecedence(ops.top()) >= getPrecedence(token)) {
                double val2 = values.top();
                values.pop();
                double val1 = values.top();
                values.pop();
                std::wstring op = ops.top();
                ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        double val2 = values.top();
        values.pop();
        double val1 = values.top();
        values.pop();
        std::wstring op = ops.top();
        ops.pop();
        values.push(applyOp(val1, val2, op));
    }

    return values.top();
}

void CreateTrayIcon(HWND hwnd)
{
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Numlock Calculator");
    Shell_NotifyIconW(NIM_ADD, &nid);
}

void DeleteTrayIcon(HWND hwnd)
{
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void AddToHistory(HWND hwndHistory, const std::wstring& calculation)
{
    SendMessage(hwndHistory, LB_ADDSTRING, 0, (LPARAM)calculation.c_str());
}

