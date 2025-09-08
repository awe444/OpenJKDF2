# Joystick Menu Navigation

This document describes the joystick-driven menu navigation feature added to OpenJKDF2.

## Overview

The joystick menu navigation and control feature allows players to navigate GUI menus and control in-game actions using a game controller on SDL2/Linux platforms.

## Controls

### Menu Navigation (when in menus)
- **Left Joystick (X/Y axes)**: Move the mouse cursor in menus
- **A Button (Button 0)**: Left mouse click (press and release)

### In-Game Actions (when not in menus)
- **Start Button (Button 7)**: Open the pause menu

## Technical Details

### Files Modified

- `src/Gui/jkGUIRend.h` - Added function declarations and debug toggle
- `src/Gui/jkGUIRend.c` - Added controller mouse helper functions
- `src/Win95/Window.c` - Added SDL joystick event handling

### Key Functions

- `jkGuiRend_ControllerMouseMove(int dx, int dy)` - Moves the GUI cursor by delta amounts (menu navigation)
- `jkGuiRend_ControllerMouseButton(int down)` - Triggers mouse button press/release events (menu navigation)
- `Window_msg_main_handler()` with `VK_ESCAPE` - Triggers pause menu when Start button is pressed in-game

### Configuration

- **Dead Zone**: 8000 (out of ±32767)
- **Sensitivity**: 5.0x multiplier for axis movement
- **Menu Navigation**: A Button (Button 0) for clicking
- **Pause Menu**: Start Button (Button 7) for opening pause menu
- **Debug Logging**: Can be enabled by defining `JOY_MENU_DEBUG` in `jkGUIRend.h`

### Platform Support

- **Target Platform**: SDL2/Linux
- **Joystick Support**: Uses SDL2 joystick events
- **Menu Detection**: Menu navigation only active when `jkGuiRend_activeMenu` is not NULL
- **In-Game Actions**: Pause menu trigger active when `jkGuiRend_activeMenu` is NULL

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