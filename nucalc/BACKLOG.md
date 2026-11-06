# NuCalc backlog

Ideas for possible future development. Nothing here is scheduled or committed to.

## Linux version (native, GTK3)

**Status:** idea, analyzed (2026-11), not started.

**Goal:** build and run NuCalc natively on Linux desktops (X11 and Wayland), toggled by the keyboard **Calculator key** (`XF86Calculator`) instead of NumLock.

### Why not NumLock on Linux
- Wayland does not let applications listen to the keyboard globally.
- The GlobalShortcuts portal may not accept a bare `Num_Lock` as a trigger. Even if it does, the lock toggle cannot be undone, because Wayland clients cannot inject keys. The numpad would flip between digits and arrows on every toggle (the bug fixed in `5fd29ca` on Windows).
- "Keep NumLock always on" is not possible from an app on Wayland, only via DE startup settings.

With the Calculator key, all NumLock-specific machinery (LL hook, undo press, `EnsureNumLockOn`, numpad scancode decoding) is simply not needed on Linux.

### Proposed design
1. **Refactor first (separate PR):** split `main.cpp` into a portable core (tinyexpr evaluation, number formatting, history load/save, input logic: Enter-keeps-result, `ans`, history navigation) and a Win32 platform layer. Windows behavior must not change.
2. **Linux layer on GTK3** (not GTK4: it dropped window positioning and keep-above). Roughly 500-700 lines:
   - custom drawing ported from GDI to Cairo (layout from `Layout()` reused as is);
   - GtkEntry for input;
   - config in `~/.config/nucalc/`, history in `~/.local/share/nucalc/history.txt`;
   - single instance via a unix socket;
   - autostart via `~/.config/autostart/nucalc.desktop`;
   - theme from the system color-scheme setting.
3. **Hotkey:**
   - primary: `nucalc --toggle` (forwards to the running instance and exits), bound to the Calculator key in the DE settings. Works everywhere: GNOME, KDE, sway (`bindsym XF86Calculator exec nucalc --toggle`), X11 and Wayland;
   - X11 extra: grab `XF86Calculator` directly (fails if the DE already binds it);
   - Wayland extra (later): register the shortcut via the `org.freedesktop.portal.GlobalShortcuts` portal (KDE 5.27+, GNOME 48+, Hyprland; not wlroots/sway).
   - GNOME and KDE bind the Calculator key to their own calculator by default; the README must explain how to unbind it.
4. **Tray:** optional (StatusNotifierItem where available: KDE, XFCE, GNOME with an extension). Not required, since the key toggles the window and the close button's right-click menu has Quit.
5. **CI:** add an `ubuntu-latest` job; publish a Linux binary (dynamic, needs GTK3) or an AppImage next to `nucalc.exe` in the release.

### Known Wayland limitations / risks
- **Focus (main risk):** the window needs an xdg-activation token to get keyboard focus. The DE usually passes one to the launched `nucalc --toggle` via the environment; it must be forwarded to the running instance. Needs testing on GNOME and KDE.
- **Position:** apps cannot place their own windows; the saved position cannot be restored (the compositor decides, usually centered). Dragging still works. X11 behaves like Windows.
- **Always on top:** not guaranteed on Wayland once the window loses focus.

### Testing
- In WSL (WSLg is available): window, drawing, input, history, `--toggle` from a terminal. Needs `pkg-config` and `libgtk-3-dev` installed (sudo).
- On a real Linux desktop only: the Calculator key, Wayland focus, tray.

## Windows: Calculator key as an alternative hotkey

**Status:** idea.

Also accept the Calculator key (`VK_LAUNCH_APP2`) in the existing LL keyboard hook, as an alternative to NumLock. Small change; would make the hotkey consistent with a future Linux version.
