Create a Windows calculator application in C# using Windows Forms targeting .NET 8 with complete Visual Studio solution structure.

## Project Structure Requirements
- Create a complete Visual Studio solution (.sln file)
- Project should target .NET 8 (net8.0-windows)
- Use Windows Forms project template
- Solution structure:
  * Solution file (.sln)
  * Project file (.csproj) with proper SDK and target framework
  * Program.cs with Main entry point
  * Main form (CalculatorForm.cs, CalculatorForm.Designer.cs, CalculatorForm.resx)
  * Calculator logic class (Calculator.cs)
  * Any additional helper classes
  * Properties folder with Settings if needed
- Enable Windows-specific features in .csproj:
```xml
  <Project Sdk="Microsoft.NET.Sdk">
    <PropertyGroup>
      <OutputType>WinExe</OutputType>
      <TargetFramework>net8.0-windows</TargetFramework>
      <UseWindowsForms>true</UseWindowsForms>
      <Nullable>enable</Nullable>
    </PropertyGroup>
  </Project>
```

## Core Functionality
- Implement a Windows Forms calculator with standard operations:
  * Basic arithmetic: addition (+), subtraction (-), multiplication (*), division (/)
  * Advanced operations: square root (√), square (x²), power (x^y), logarithm (log, log10)
- Display calculation history in a ListBox or RichTextBox showing format: "operand1 operator operand2 = result"
- Support both button clicks and keyboard input for numbers and operations
- Handle edge cases (division by zero, invalid operations, negative square roots, domain errors for log)
- Show clear error messages using MessageBox or display label
- Implement proper decimal point handling
- Support negative numbers

## System Tray Integration
- Use NotifyIcon component (drag from toolbox in Designer or create programmatically)
- Set NotifyIcon.Icon property (include .ico file in project or create from resources)
- Minimize to system tray instead of taskbar when minimize button clicked
- Implement ContextMenuStrip for tray icon with MenuItems:
  * "Show Calculator"
  * "Exit"
- Handle NotifyIcon.DoubleClick event to restore window
- Override Form.FormClosing event:
  * Prevent actual closing (e.Cancel = true) and minimize to tray instead
  * Only allow closing when Exit is explicitly clicked from context menu
- Set this.ShowInTaskbar = false when minimized
- Set this.ShowInTaskbar = true when visible

## NumLock Hotkey Behavior
- Implement global hotkey using Win32 API interop
- P/Invoke declarations needed:
```csharp
  [DllImport("user32.dll")]
  private static extern bool RegisterHotKey(IntPtr hWnd, int id, uint fsModifiers, uint vk);
  
  [DllImport("user32.dll")]
  private static extern bool UnregisterHotKey(IntPtr hWnd, int id);
  
  [DllImport("user32.dll")]
  private static extern short GetKeyState(int nVirtKey);
  
  [DllImport("user32.dll")]
  private static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
```
- Constants:
  * WM_HOTKEY = 0x0312
  * VK_NUMLOCK = 0x90
  * KEYEVENTF_EXTENDEDKEY = 0x1
  * KEYEVENTF_KEYUP = 0x2
- Register NumLock as global hotkey in Form.Load (MOD_NONE, VK_NUMLOCK)
- Override WndProc to handle WM_HOTKEY message
- Toggle calculator visibility on NumLock press:
  * If minimized/hidden → Show(), Activate(), BringToFront(), set ShowInTaskbar = true
  * If visible → Hide to tray, set ShowInTaskbar = false
- Force NumLock ON while application runs:
  * Check NumLock state using GetKeyState(VK_NUMLOCK)
  * If OFF, simulate NumLock keypress using keybd_event()
  * Implement Timer (100-200ms interval) to continuously monitor and enforce NumLock ON
- Store original NumLock state on Form.Load
- Restore original NumLock state on Form.FormClosed (after unregistering hotkey)
- Unregister hotkey in Form.FormClosing before application exits

## UI Design (Windows Forms Designer)
- Form properties:
  * Text = "Calculator"
  * Size = appropriate (e.g., 400x600)
  * StartPosition = CenterScreen
  * FormBorderStyle = FixedSingle
  * MaximizeBox = false
  * MinimizeBox = true
- Main display TextBox:
  * ReadOnly = true
  * Font = large (e.g., 16pt)
  * TextAlign = HorizontalAlignment.Right
  * Dock = Top or manual positioning
- History display (ListBox or RichTextBox):
  * ScrollBars = Vertical
  * Height = ~150-200px
  * ReadOnly = true (if TextBox)
- Button layout using TableLayoutPanel or manual positioning:
  * Number buttons: 0-9
  * Operation buttons: +, -, *, /, =
  * Advanced: √, x², x^y, log
  * Utility: C (Clear), CE (Clear Entry), ← (Backspace), +/- (negate)
  * Decimal point button (.)
- Button properties:
  * Consistent size (e.g., 60x60 or 70x50)
  * Appropriate fonts and colors
  * TabStop ordering for keyboard navigation

## Calculator Logic Class
- Separate Calculator.cs class with methods:
  * Add(double a, double b)
  * Subtract(double a, double b)
  * Multiply(double a, double b)
  * Divide(double a, double b) - throw exception for division by zero
  * SquareRoot(double a) - validate non-negative
  * Square(double a)
  * Power(double base, double exponent)
  * Logarithm(double a, double baseLog = 10) - validate positive input
- Return results or throw ArgumentException for invalid operations
- Static class or instantiable with state management

## Additional Implementation Details
- History management:
  * Store in List<string> or directly add to ListBox
  * Format: "5 + 3 = 8", "√16 = 4", etc.
  * Limit history size (optional, e.g., last 100 operations)
- Keyboard input handling:
  * Handle Form.KeyPress or Form.KeyDown events
  * Map numeric keys, operators, Enter (=), Escape (C), etc.
  * Set Form.KeyPreview = true
- Error handling:
  * Try-catch blocks around calculations
  * Display errors in result TextBox or MessageBox
  * Clear error state on next input
- State management:
  * Track current value, pending operation, operand
  * Handle chaining operations (5 + 3 + 2)
  * Clear state appropriately

## File Deliverables
Provide the following files ready to open in Visual Studio 2022:
1. **Calculator.sln** - Solution file
2. **Calculator.csproj** - Project file with .NET 8 target
3. **Program.cs** - Entry point with Application.Run()
4. **CalculatorForm.cs** - Main form code-behind
5. **CalculatorForm.Designer.cs** - Designer-generated code
6. **CalculatorForm.resx** - Form resources
7. **Calculator.cs** - Calculation logic class
8. **Icon file** (calculator.ico) - for form and tray icon
9. **README.md** - Build and run instructions

## Build and Run Instructions (include in README)
```bash
# Build the solution
dotnet build Calculator.sln

# Run the application
dotnet run --project Calculator.csproj

# Or open Calculator.sln in Visual Studio 2022 and press F5
```

## Quality Requirements
- Use C# 12 features where appropriate
- Enable nullable reference types
- XML documentation comments on public members
- Proper resource disposal (IDisposable pattern)
- Clean code organization and naming conventions
- No compiler warnings
- Handle all edge cases gracefully

## Testing Checklist
- [ ] Calculator performs all operations correctly
- [ ] History displays all calculations
- [ ] Minimizes to system tray (not taskbar)
- [ ] Tray icon context menu works
- [ ] Double-click tray icon restores window
- [ ] NumLock toggle shows/hides calculator
- [ ] NumLock stays ON while app runs
- [ ] Original NumLock state restored on exit
- [ ] Global hotkey unregisters properly on exit
- [ ] No memory leaks (NotifyIcon disposed)
- [ ] Opens correctly in Visual Studio 2022
- [ ] Builds without errors or warnings

Provide complete, production-ready code that can be immediately opened in Microsoft Visual Studio 2022 and compiled/run successfully.