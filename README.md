# ncalc - Numlock Calculator

`ncalc` is a lightweight, tray-resident calculator application for Windows. It's designed for quick access and calculation, primarily controlled via the NumLock key.

## Features

*   **NumLock Hotkey**: Toggle the calculator window visibility with a press of the NumLock key.
*   **Tray Icon**: Minimizes to the system tray, keeping your taskbar clean.
*   **Expression Evaluation**: Evaluates mathematical expressions using the `tinyexpr` library.
*   **Calculation History**: Keeps a persistent history of your calculations, saved to `history.txt` in the application directory.
*   **Dynamic UI**: The user interface (input field, history, and buttons) and fonts scale dynamically with the window size.
*   **NumLock Enforcement**: Attempts to ensure the NumLock key is always active when the calculator is in use.

## Dependencies

*   **Windows API (WinAPI)**: Used for all GUI elements, window management, hotkey registration, and system tray integration. This is a core part of the Windows operating system.
*   **tinyexpr**: A small, fast, and open-source C library for parsing and evaluating mathematical expressions. `ncalc` uses it to perform all calculations. (Source files: `tinyexpr.h`, `tinyexpr.c`)

## How to Build

This project is a standard C++ WinAPI application. You will need a C++ compiler, such as MinGW or Visual Studio with C++ desktop development tools.

**Using MinGW (or similar GCC-based compiler):**

1.  Navigate to the project root directory in your terminal.
2.  Run the `build.bat` script:
    ```bash
    build.bat
    ```
    This script compiles `main.cpp` and `tinyexpr.c` and links them to create the `ncalc.exe` executable.

**Manual Compilation (Example with g++):**

```bash
g++ main.cpp tinyexpr.c -o ncalc.exe -std=c++11 -Wall -luser32 -lgdi32 -lcomctl32 -lshell32
```

*   `-std=c++11`: Ensures C++11 features are available.
*   `-Wall`: Enables all warnings (good practice).
*   `-luser32 -lgdi32 -lcomctl32 -lshell32`: Links necessary Windows libraries for GUI, common controls, and shell functions.

## How to Use

1.  Run `ncalc.exe`. The calculator window will appear.
2.  You can type expressions directly into the input field. Press `Enter` to evaluate.
3.  Click the buttons to input numbers, operators, and functions.
4.  The NumLock key acts as a global hotkey:
    *   Press NumLock to show the calculator if it's hidden.
    *   Press NumLock again to hide it (minimize to tray) if it's active.
5.  Double-click the tray icon to restore the window.
6.  The calculation history is displayed in the listbox. Double-clicking an entry will put its result back into the input field.
7.  The history is saved to `history.txt` in the same directory as `ncalc.exe`.

## How to Modify

To modify `ncalc`, you will primarily be working with `main.cpp`.

*   **Adding/Changing Functions**:
    *   To add new mathematical functions or constants, you would typically extend the `tinyexpr` library or integrate another expression parser. For simple additions, you might modify the `evaluateExpression` function and the button handling logic in `WndProc`.
    *   The `tinyexpr` library itself supports custom functions and variables, which would involve modifying how `te_compile` or `te_interp` is called and providing the necessary `te_variable` or `te_fun` structures.
*   **UI Changes**:
    *   The UI is built using raw WinAPI calls (e.g., `CreateWindow`). To change layout, add new controls, or modify existing ones, you'll need to adjust the `CreateWindow` calls and the `WM_SIZE` message handling in `WndProc`.
    *   Button definitions are in the `WinMain` function.
*   **Hotkey/Tray Behavior**:
    *   The hotkey registration (`RegisterHotKey`) and handling (`WM_HOTKEY` in `WndProc`) can be modified to change the hotkey or its behavior.
    *   Tray icon behavior is managed by `Shell_NotifyIcon` and `WM_TRAYICON` messages.
*   **History Management**:
    *   The `loadHistory`, `addToHistory` functions, and `historyFilePath` variable control how history is loaded, saved, and displayed.
*   **Logging**:
    *   The `log` function is currently commented out. Uncommenting it will enable logging to `c:\tmp\clog.txt` for debugging purposes.

Remember to recompile the application after making any changes.