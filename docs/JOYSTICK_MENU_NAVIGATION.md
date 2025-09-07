# Joystick Menu Navigation

This document describes the joystick-driven menu navigation feature added to OpenJKDF2.

## Overview

The joystick menu navigation feature allows players to navigate GUI menus using a game controller on SDL2/Linux platforms. This feature only affects menu navigation and does not impact gameplay controls.

## Controls

- **Left Joystick (X/Y axes)**: Move the mouse cursor in menus
- **A Button (Button 0)**: Left mouse click (press and release)

## Technical Details

### Files Modified

- `src/Gui/jkGUIRend.h` - Added function declarations and debug toggle
- `src/Gui/jkGUIRend.c` - Added controller mouse helper functions
- `src/Win95/Window.c` - Added SDL joystick event handling

### Key Functions

- `jkGuiRend_ControllerMouseMove(int dx, int dy)` - Moves the GUI cursor by delta amounts
- `jkGuiRend_ControllerMouseButton(int down)` - Triggers mouse button press/release events

### Configuration

- **Dead Zone**: 8000 (out of ±32767)
- **Sensitivity**: 5.0x multiplier for axis movement
- **Debug Logging**: Can be enabled by defining `JOY_MENU_DEBUG` in `jkGUIRend.h`

### Platform Support

- **Target Platform**: SDL2/Linux
- **Joystick Support**: Uses SDL2 joystick events
- **Menu Detection**: Only active when `jkGuiRend_activeMenu` is not NULL

## Debugging

To enable debug logging:

1. Open `src/Gui/jkGUIRend.h`
2. Uncomment the line: `// #define JOY_MENU_DEBUG 1`
3. Rebuild the project

Debug output will show:
- Mouse movement deltas and new positions
- Button press/release events
- Click dispatch notifications

## Implementation Notes

- The feature reuses existing mouse interaction paths for compatibility
- Only the first joystick (index 0) is supported
- Cursor movement is clamped to screen boundaries
- Button mapping uses SDL button 0 (typically the A button on most controllers)