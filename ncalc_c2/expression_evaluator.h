#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include <string>
#include <windows.h> // For HWND and MessageBox

extern HWND g_hwndInput; // Declare g_hwndInput as extern
extern HWND g_hwndHistory; // Declare g_hwndHistory as extern

void EvaluateExpression();

#endif // EXPRESSION_EEVALUATOR_H
