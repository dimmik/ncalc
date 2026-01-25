Create a desktop calculator application in C/C++ using Win32 API with the following requirements:

**Core Functionality:**
- Support basic arithmetic operations (+, -, *, /)
- Support advanced operations: square (x²), square root (√), parentheses
- Expression evaluation using exprtk (exprtk.hpp in this folder)

**Keyboard Integration:**
- Accept input from numpad keys
- NumLock hotkey behavior:
  * Press NumLock when app is closed → open the calculator
  * Press NumLock when app is open but not focused → bring to foreground
  * Press NumLock when app is focused → minimize to system tray
- While running, force NumLock to always stay ON (enable numeric input mode, not cursor keys)

**UI Requirements (Win32 API):**
- Main calculation input field
- Calculator buttons for all operations including "=" that should evaluate the expression
- Number buttons
- Enter always acts as "evaluate expression" (same as "=" button)
- History panel displaying previous calculations
- Each history entry shows: expression, result, and timestamp (date/time)
- Double-click on history item loads it into main input field

**Data Persistence:**
- Save calculation history to a file (suggest appropriate format - JSON, CSV, or binary)
- Load history on startup
- Store: expression, result, timestamp for each calculation

**System Tray:**
- Minimize to system tray (not taskbar)
- Tray icon with context menu (restore, exit)
- Double-click tray icon to restore window

**Technical Requirements:**
- Pure Win32 API for UI (no MFC, no Qt)
- C or C++ implementation
- Global keyboard hook for NumLock detection
- Proper resource cleanup and memory management

**CODE ORGANIZATION - CRITICAL:**
Mark all key customization points with clear comment blocks using this format:

// ============================================================
// [CUSTOMIZATION POINT]: <Description>
// ============================================================

Required comment markers for:
1. UI element creation (main window, input field, buttons, history list)
2. NumLock monitoring and control logic
3. Keyboard hook installation and handling
4. System tray operations (minimize, restore, icon handling)
5. Expression evaluation integration (library calls)
6. History file I/O operations (save/load)
7. Window message handling (WM_COMMAND, WM_SYSCOMMAND, etc.)
8. Calculator button layout and positioning
9. History item selection/double-click handler
10. Application configuration constants (window size, colors, file paths, etc.)

Each customization point should have:
- Clear description of what can be modified
- Any relevant parameters or configuration values
- Example of typical modification if applicable

Please implement this as a complete, production-ready application with proper error handling, clean code structure, and well-organized customization points for easy maintenance and modification.