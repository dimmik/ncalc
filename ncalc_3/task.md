Write in C/C++ wireframe for windows calculator application


1. Build/compile
Use g++ for build/compile

2. UI
2.1 UI should be win32-based
2.2 UI should contain
- Input field, where expression is formed
- Layout:
Set of buttons (0-9) and the following operations:
arithmetic: +, -, *, / 
other calc: sqr (square), sqrt (square root), log, ln, a^b
clean: C (clears input)
brackets: "(", ")"
equals: "=" - evaluates the expression (for evaluation see below)
others as required
- history of operations, sorted from newest to oldest, with datetime before each operation, including operation itself and result, on double-click result is loaded into input field

3. System integration
The applicatuion should be minimized to system tray. use calculator.ico in this folder as icon.
The application should open from system tray on press of NumLock button
The application, when not in focus, should come to foreground on press of NumLock button
The application, when in focus, should be minimized to tray on press of NumLock button
When application is running it should ensure NumLock is always on (numbers are enetered on numpad keys)

The application should accept input from keyboard (including numpad), enter should always act as press on "=" (evaluation)

3. Evaluation.
The application should not evaluate anything (it is next step), designate a function or method that accepts input string, and returns tuple (status, result).
By default (in this implementation) it should return ("", "-42").
Calculator itself, when it receives the result, should put it into input field if status is "", and show alert wihdow with the status if it is not empty (like if it is ("", "2322") - put 2322 into input field, if it is ("unmatched bracket", "") - alert with "unmatched bracket")

4. Other
Aim to minimize application binary size, do not use 3rd-party libraries, comment code to designate key areas.
Specifically mark with comments places:
- Numlock always on control
- Open/focus/close on numlock
- Minimization to tray
- evaluation - where it is called, and where the wireframe of the evaluation function is.
