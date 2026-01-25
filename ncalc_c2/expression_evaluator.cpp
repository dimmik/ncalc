#include "expression_evaluator.h"
#include "exprtk.hpp"
#include <string>
#include <windows.h> // For MessageBox
#include <chrono>
#include <ctime>
#include <commctrl.h> // For ListView_InsertItem, ListView_SetItemText

// External declarations for global HWNDs
extern HWND g_hwndInput;
extern HWND g_hwndHistory;

// ============================================================
// [CUSTOMIZATION POINT]: History item creation
// ============================================================
void AddHistoryEntry(const std::string& expression, const std::string& result) {
    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = ListView_GetItemCount(g_hwndHistory);
    lvi.iSubItem = 0;
    lvi.pszText = const_cast<LPSTR>(expression.c_str());
    ListView_InsertItem(g_hwndHistory, &lvi);

    ListView_SetItemText(g_hwndHistory, lvi.iItem, 1, const_cast<LPSTR>(result.c_str()));

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_s(&buf, &in_time_t);
    char time_buf[26];
    asctime_s(time_buf, sizeof(time_buf), &buf);
    ListView_SetItemText(g_hwndHistory, lvi.iItem, 2, time_buf);
}

// ============================================================
// [CUSTOMIZATION POINT]: Expression evaluation integration
// ============================================================
void EvaluateExpression() {
    char buffer[256];
    GetWindowText(g_hwndInput, buffer, 256);
    std::string expression_string = buffer;

    if (expression_string.empty()) {
        return;
    }

    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double>         parser_t;

    expression_t expression;
    parser_t parser;

    if (parser.compile(expression_string, expression)) {
        double result = expression.value();
        std::string result_str = std::to_string(result);
        SetWindowText(g_hwndInput, result_str.c_str());
        AddHistoryEntry(expression_string, result_str);
    } else {
        MessageBox(NULL, "Invalid Expression", "Error", MB_OK);
    }
}
