# ncalc Usage Scenarios

This document provides a set of usage scenarios based on the functional requirements to verify the application's behavior.

## Scenario 1: Basic Calculation

1.  **Action**: User launches the application.
2.  **Expected Result**: The calculator window appears. The input field is empty and focused. The history view shows previous calculations loaded from `history.txt`.
3.  **Action**: User clicks the buttons `2`, `+`, `2`, `=`.
4.  **Expected Result`:
    - The input field shows `4`.
    - A new entry appears at the top of the history view, e.g., `2026-02-03 10:00:00: 2+2 = 4`.
    - The same entry is appended to the `history.txt` file.

## Scenario 2: Complex Expression with Functions and Keyboard

1.  **Action**: User types `sqrt(9) + (2^3)` into the input field.
2.  **Expected Result**: The text appears as typed.
3.  **Action**: User presses the `Enter` key.
4.  **Expected Result`:
    - The input field is updated to show `11`.
    - A new entry `...: sqrt(9) + (2^3) = 11` is added to the history.

## Scenario 3: Input Formatting

1.  **Action**: User types `1234567.89`.
2.  **Expected Result**: As the user types, the input field automatically formats the text to `1'234'567.89`. The cursor remains in the correct position.
3.  **Action**: User clicks `*`, `2`, `=`.
4.  **Expected Result`:
    - The input field shows `2'469'135.78`.
    - The history entry shows `...: 1'234'567.89*2 = 2'469'135.78`.

## Scenario 4: Error Handling

1.  **Action**: User enters the expression `5 + * 3` and presses `=`.
2.  **Expected Result`:
    - A modal error dialog appears.
    - The message indicates an invalid expression and points to the `*` character, e.g., `Invalid expression.\n5 + * 3\n      ^`.
    - The input field remains unchanged.
    - No entry is added to the history.

## Scenario 5: Using History

1.  **Action**: The history view contains the entry `...: 2+2 = 4`. User double-clicks this entry.
2.  **Expected Result**: The input field is populated with the result, `4`. The text is selected.
3.  **Action**: User clicks `/`, `2`, `=`.
4.  **Expected Result`:
    - The input field shows `2`.
    - A new entry `...: 4/2 = 2` is added to the history.

## Scenario 6: System Tray and Hotkey

1.  **Action**: User clicks the minimize button on the window.
2.  **Expected Result**: The window disappears from the screen and the taskbar. An icon for the calculator appears in the system tray.
3.  **Action**: User double-clicks the tray icon.
4.  **Expected Result**: The calculator window reappears in its previous position.
5.  **Action**: User presses the global hotkey (e.g., `NumLock`).
6.  **Expected Result**: The window minimizes to the tray again.
7.  **Action**: User presses the global hotkey again.
8.  **Expected Result**: The window is restored and brought to the foreground. The text in the input field is fully selected.

## Scenario 7: Right-Click Menu on Tray Icon

1.  **Action**: With the application minimized to the tray, the user right-clicks the tray icon.
2.  **Expected Result**: A context menu appears with "Open" and "Exit" options.
3.  **Action**: User clicks "Open".
4.  **Expected Result**: The calculator window is restored.
5.  **Action**: User right-clicks the tray icon again and clicks "Exit".
6.  **Expected Result**: The application terminates.

## Scenario 8: Input Pre-processing

1.  **Action**: User types `.5` and presses `=`.
2.  **Expected Result`:
    - The expression is treated as `0.5`.
    - The input field shows `0.5`.
    - The history entry shows `...: .5 = 0.5`.

## Scenario 9: Window Resizing

1.  **Action**: User drags the corner of the window to make it larger.
2.  **Expected Result`:
    - The input field, history view, and all buttons resize and reposition themselves to fit the new window dimensions.
    - The font size of the text in the controls increases proportionally.
    - The application remains fully functional.
