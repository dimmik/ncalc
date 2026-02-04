# Functional Requirements for ncalc

## 1.0 Core Functionality: Expression Evaluation

### 1.1 Expression Parsing
The application shall evaluate mathematical expressions provided as strings.

### 1.2 Supported Operators
The evaluation engine shall support the following operators with standard mathematical precedence:
- `+` (Addition)
- `-` (Subtraction)
- `*` (Multiplication)
- `/` (Division)
- `^` (Exponentiation)
- `%` (Modulo)

### 1.3 Supported Functions
The evaluation engine shall support the following built-in functions:
- `abs(x)`: Absolute value
- `acos(x)`: Arc cosine
- `asin(x)`: Arc sine
- `atan(x)`: Arc tangent
- `atan2(y, x)`: Arc tangent of y/x
- `ceil(x)`: Ceiling (round up)
- `cos(x)`: Cosine
- `cosh(x)`: Hyperbolic cosine
- `exp(x)`: e raised to the power of x
- `fac(x)`: Factorial
- `floor(x)`: Floor (round down)
- `ln(x)`: Natural logarithm
- `log(x)` or `log10(x)`: Base-10 logarithm
- `ncr(n, r)`: Number of combinations
- `npr(n, r)`: Number of permutations
- `pow(x, y)`: x raised to the power of y (equivalent to `x^y`)
- `sin(x)`: Sine
- `sinh(x)`: Hyperbolic sine
- `sqrt(x)`: Square root
- `tan(x)`: Tangent
- `tanh(x)`: Hyperbolic tangent

### 1.4 Supported Constants
The evaluation engine shall support the following built-in constants:
- `pi`: The mathematical constant Pi (~3.14159)
- `e`: The mathematical constant e (~2.71828)

### 1.5 Parentheses
The engine shall support parentheses `()` for grouping expressions and controlling the order of operations.

## 2.0 User Interface (UI)

### 2.1 Main Window
- The application shall have a main window titled "Calculator".
- The window shall have a custom icon loaded from `calculator.ico`.
- The window shall be resizable. All UI elements within the window shall scale and reposition dynamically when the window is resized.

### 2.2 Input/Result Display
- A text input field shall be prominently displayed at the top of the window.
- This field shall be used for both entering expressions and displaying results.
- The text in this field shall use a dynamically scaled font.

### 2.3 On-Screen Buttons
- A grid of buttons shall be provided for user input.
- The grid shall contain buttons for:
    - Numbers: `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`
    - Operators: `/`, `*`, `-`, `+`, `^`
    - Other: `.`, `=`, `C`, `(`, `)`
- Clicking a number or operator button shall append its character to the current expression in the input field.
- Clicking the `=` button shall trigger the evaluation of the expression in the input field.
- Clicking the `C` button shall clear the input field.

### 2.4 History View
- A listbox control below the input field shall display the history of calculations.
- The history view shall be populated from `history.txt` on startup.
- Each entry shall be custom-drawn to display a timestamp in a small, bold font and the corresponding calculation in a normal font.
- The listbox items shall have a height that dynamically adjusts to the font size.

## 3.0 Input and Interaction

### 3.1 Real-time Input Formatting
- As the user types a number into the input field, thousand separators (`'`) shall be automatically inserted to improve readability (e.g., `1000000` becomes `1'000'000`).
- The cursor position shall be maintained correctly during automatic formatting.
- These separators shall be removed from the expression before it is passed to the evaluation engine.

### 3.2 Input Pre-processing
- Before evaluation, if an expression contains a number starting with a decimal point (e.g., `.5`), a leading zero shall be automatically added (`0.5`).

### 3.3 Keyboard Input
- The user shall be able to type expressions directly into the input field.
- Pressing the `Enter` key while the input field is focused shall trigger the evaluation of the expression.

### 3.4 History Interaction
- Double-clicking an entry in the history listbox shall copy the *result* of that calculation into the input field, ready for use in a new calculation.

## 4.0 Calculation History

### 4.1 History Logging
- Upon successful evaluation, a new entry shall be added to the top of the history view.
- The entry shall be formatted as: `YYYY-MM-DD HH:MM:SS: [Expression] = [Result]`
- This new entry shall also be appended to the `history.txt` file located in the application's directory.

### 4.2 History Persistence
- The `history.txt` file shall serve as a persistent log of all calculations.
- The application shall load all entries from this file upon startup.

## 5.0 System Integration

### 5.1 System Tray
- When the main window is minimized, it shall be hidden from the taskbar and a corresponding icon shall appear in the system tray.
- The tray icon shall be loaded from `calculator.ico`.
- The tooltip for the tray icon shall be "Calculator".
- Double-clicking the tray icon shall restore the application window to its previous state.
- Right-clicking the tray icon shall display a context menu with two options:
    - "Open": Restores the application window.
    - "Exit": Terminates the application.

### 5.2 Global Hotkey
- The application shall register a global hotkey to toggle the visibility of its main window.
- If the window is active/visible, the hotkey shall minimize it to the tray.
- If the window is hidden or inactive, the hotkey shall show and focus the window.
- The specific key for the hotkey shall be read from the `hotkey.txt` file in the application's directory.
- The file can specify the key by its virtual key code name (e.g., `VK_NUMLOCK`, `VK_F8`) or as a hexadecimal value.
- If the file is not present or the content is invalid, `VK_NUMLOCK` shall be used as the default hotkey.

### 5.3 NumLock Enforcement
- The application shall attempt to programmatically enable the `NumLock` key state when its window is shown or restored.

## 6.0 Error Handling

### 6.1 Invalid Expression
- If the user attempts to evaluate an invalid or malformed expression, an error shall be reported.
- The error shall be displayed in a modal message box.
- The error message shall indicate that the expression is invalid and show the expression along with a `^` character pointing to the position of the error.
- The calculation shall not be added to the history.

### 6.2 Division by Zero
- Operations that result in invalid mathematical outcomes (e.g., division by zero, `sqrt(-1)`) shall result in `inf` or `nan` being displayed as the result, as handled by the evaluation engine.
