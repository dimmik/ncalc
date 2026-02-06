# Calculator UI Redesign Task

## Context
You have an existing Win32-based calculator application that needs a visual redesign. The current implementation looks outdated. This task is to modernize the UI to match the provided design mockup while preserving existing functionality.

## Objective
Refactor the existing Win32 calculator application to implement a modern, dark-themed interface as shown in the reference design.

## Current State Assessment
Before making changes, analyze the existing codebase:
- Identify current window creation and sizing
- Locate existing button controls and their handlers
- Find display/output controls (edit controls, static text, etc.)
- Review current color scheme and font settings
- Document existing calculation logic (preserve this)

## Design Changes Required

### 1. Window Styling
**Update these properties:**
- Set dark background color: RGB(43, 43, 43) or similar dark charcoal
- Ensure window is non-resizable (fixed size) or set minimum dimensions
- Modern window title: "Calculator"
- Adjust window size to accommodate new layout (~400x600 pixels suggested)

### 2. Display Area Restructure

**Top Input Field:**
- Single-line edit control (ES_AUTOHSCROLL, ES_RIGHT)
- Position: Full width, ~40px height, 10px margin
- Background: Dark transparent with light border
- Text color: White
- Border: 1px solid RGB(85, 85, 85)
- Purpose: Shows current expression being entered

**Middle History Panel:**
- Multi-line edit control (ES_MULTILINE, ES_AUTOVSCROLL, ES_RIGHT, ES_READONLY)
- Position: Below input field, ~300px height
- Background: Slightly darker than window
- Text color: White
- Right-aligned text
- Shows calculation history with results
- Scrollable if history exceeds visible area

### 3. Button Grid Redesign

**Layout: 5 rows × 4 columns**

Update button arrangement to:
```
Row 1: [  7  ] [  8  ] [  9  ] [  /  ]
Row 2: [  4  ] [  5  ] [  6  ] [  *  ]
Row 3: [  1  ] [  2  ] [  3  ] [  -  ]
Row 4: [  0  ] [  .  ] [  =  ] [  +  ]
Row 5: [  C  ] [  (  ] [  )  ] [Theme]
```

**Button Styling Updates:**
- Size: ~90px width × 60px height (adjust to fit)
- Spacing: 8-10px gaps between buttons
- Background: RGB(51, 51, 51)
- Border: 1px rounded, RGB(85, 85, 85)
- Text color: White (RGB(255, 255, 255))
- Font: Segoe UI or similar, 14-16pt
- Border radius: 4-6px rounded corners (use owner-draw or custom control)

**Owner-Draw Buttons (Recommended):**
Since standard Win32 buttons don't support rounded corners easily:
- Implement BS_OWNERDRAW style
- Handle WM_DRAWITEM message
- Use GDI+ or GDI for rounded rectangles
- Implement hover states (slightly lighter background on hover)

### 4. Color Scheme Summary

| Element | Color | RGB Value |
|---------|-------|-----------|
| Window Background | Dark Charcoal | (43, 43, 43) |
| Button Background | Dark Gray | (51, 51, 51) |
| Border Color | Medium Gray | (85, 85, 85) |
| Text Color | White | (255, 255, 255) |
| Input/History BG | Dark Transparent | (35, 35, 35) |

### 5. Font Updates
- Change from default system font to modern font
- Recommended: Segoe UI, size 14-16 for buttons, 12-14 for display
- Create fonts using CreateFont() with appropriate parameters
- Apply to all controls via WM_SETFONT

## Implementation Steps

### Phase 1: Window and Layout
1. Update window background brush to dark color
2. Resize window to new dimensions
3. Reposition existing controls or create new ones
4. Set WS_EX_COMPOSITED for smoother rendering

### Phase 2: Display Controls
1. Replace or reconfigure top display as single-line input
2. Add/configure middle display as multi-line history panel
3. Set appropriate styles (ES_RIGHT, ES_READONLY where needed)
4. Apply dark theme colors (background, text, border)
5. Subclass edit controls if needed for custom border rendering

### Phase 3: Button Redesign
1. Change buttons to BS_OWNERDRAW style
2. Implement WM_DRAWITEM handler for custom rendering
3. Draw rounded rectangles with GDI+
4. Apply color scheme
5. Add hover effects (track mouse with WM_MOUSEMOVE)
6. Remap button positions to new 5×4 grid

### Phase 4: Theme Button
1. Add "Theme" button in bottom-right position
2. Implement click handler to toggle light/dark theme
3. Create alternate color scheme for light theme (optional for now)
4. Store theme preference

### Phase 5: Polish
1. Add proper DPI awareness
2. Smooth font rendering (ClearType)
3. Test on different Windows versions
4. Add window icon if missing
5. Remove WS_THICKFRAME if present (non-resizable)

## Technical Implementation Notes

### Custom Button Drawing (GDI+)
```cpp
// Pseudocode example
void DrawRoundedButton(HDC hdc, RECT rect, COLORREF bgColor, LPCWSTR text) {
    // Use GDI+ for rounded rectangles
    // Fill with background color
    // Draw border
    // Center text
}
```

### Subclassing Edit Controls
For custom border rendering on input/history displays:
```cpp
// Subclass edit controls to handle WM_NCPAINT
// Draw custom border around client area
```

### Message Handling Updates
- Preserve existing WM_COMMAND handlers for calculator logic
- Add WM_DRAWITEM for owner-draw buttons
- Add WM_CTLCOLOREDIT for edit control colors
- Add WM_CTLCOLORSTATIC if using static controls

## Preservation Requirements

**DO NOT CHANGE:**
- Existing calculation logic
- Expression evaluation algorithm
- Operator precedence handling
- Number parsing functions
- Error handling for division by zero, etc.

**ONLY CHANGE:**
- Visual appearance (colors, fonts, layout)
- Control positioning and sizing
- Drawing/rendering code
- UI structure to support history display

## Testing Checklist
- [ ] All buttons render with rounded corners
- [ ] Dark theme applied consistently
- [ ] Input field accepts expressions
- [ ] History panel shows previous calculations
- [ ] All calculator operations work as before
- [ ] Theme button toggles appearance (if implemented)
- [ ] No visual glitches or flickering
- [ ] Proper DPI scaling
- [ ] Keyboard shortcuts still work

## Deliverables
- Modified source code with visual redesign
- Preserved calculation functionality
- Comments explaining custom drawing code
- Before/after screenshots (optional)
- Updated build instructions if dependencies added (GDI+)