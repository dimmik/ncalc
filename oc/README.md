# ncalc - Win32 Calculator Application

A C++ Win32 calculator application with advanced mathematical expression evaluation capabilities.

## Features

### Core Functionality
- Mathematical expression evaluation with standard operator precedence
- Support for: +, -, *, /, ^, % operators
- Parentheses for grouping expressions
- Real-time input formatting with thousand separators

### Mathematical Functions
- **Basic**: abs(), ceil(), floor(), fac() (factorial)
- **Trigonometric**: sin(), cos(), tan(), asin(), acos(), atan(), atan2()
- **Hyperbolic**: sinh(), cosh(), tanh()
- **Logarithmic**: ln(), log(), log10(), exp()
- **Power/Root**: pow(), sqrt()
- **Combinatorics**: ncr() (combinations), npr() (permutations)

### Constants
- pi (π ≈ 3.14159)
- e (Euler's number ≈ 2.71828)

### User Interface
- Resizable window with dynamic UI scaling
- On-screen numeric and operator buttons
- Input field with real-time thousand separator formatting
- History view with timestamped calculations
- Custom icon support (calculator.ico)

### System Integration
- System tray minimization with context menu
- Global hotkey support (configurable via hotkey.txt)
- Automatic NumLock activation
- Persistent calculation history (history.txt)

## Building

### Prerequisites
- MinGW/g++ compiler on Windows
- Windows SDK headers and libraries

### Compilation
```batch
# Using the build script (recommended)
build.bat

# Or using make
make

# Or manually
g++ -std=c++17 -Wall -Wextra -O2 -c main.cpp -o main.o
g++ main.o -o ncalc.exe -mwindows -luser32 -lgdi32 -lcomctl32 -lshell32 -lcomdlg32
del main.o
```

## Usage

1. **Running**: Execute `ncalc.exe`
2. **Input**: Type expressions directly or use on-screen buttons
3. **Evaluation**: Press Enter or click = to calculate
4. **Clear**: Click C or clear the input field
5. **History**: View previous calculations below the input field
6. **Tray**: Minimize to system tray, restore by double-clicking tray icon

## Configuration Files

### hotkey.txt
Specifies the global hotkey for toggling window visibility. Examples:
- `VK_NUMLOCK` (default)
- `VK_F8`
- `0x90` (hexadecimal virtual key code)

### history.txt
Stores all calculation history with timestamps. Auto-created and maintained.

### calculator.ico
Application icon (optional). If missing, default system icon is used.

## Example Expressions

```
2 + 3 * 4           // 14
(2 + 3) * 4         // 20
sqrt(16) + 2        // 6
sin(pi/2)           // 1
fac(5)              // 120
ncr(10, 3)          // 120
1'000'000 + 2'000   // 1'002'000 (auto-formatted)
```

## Error Handling

- Invalid expressions show error messages with cursor position
- Division by zero returns infinity
- Invalid operations (sqrt(-1)) return NaN
- Syntax errors are highlighted with caret position

## Technical Notes

- Unicode support for Windows API
- Custom expression parser with recursive descent
- Owner-drawn listbox for history display
- Windows message-driven architecture
- No external dependencies beyond Windows libraries