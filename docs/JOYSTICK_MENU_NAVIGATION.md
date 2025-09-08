# Joystick Menu Navigation and Control

This document describes the joystick-driven menu navigation and control features added to OpenJKDF2.

## Overview

The joystick menu navigation and control feature allows players to navigate GUI menus and access mappable in-game actions using a game controller on SDL2/Linux platforms.

## Controls

### Menu Navigation (when in menus)
- **Left Joystick (X/Y axes)**: Move the mouse cursor in menus
- **A Button (Button 0)**: Left mouse click (press and release)

### In-Game Actions (mappable through Controls GUI)
- **Open Pause Menu**: Can be mapped to any joystick button through the game's joystick mapping interface
  - Available in Controls > Joystick settings as "PAUSEMENU" action
  - Default mapping: None (must be manually assigned)
  - Coexists with other mappable actions like "Fire 1", "Fire 2", "Next Inventory Item", etc.

## Technical Details

### Files Modified

- `src/Gui/jkGUIRend.h` - Added function declarations and debug toggle
- `src/Gui/jkGUIRend.c` - Added controller mouse helper functions
- `src/Win95/Window.c` - SDL joystick event handling for menu navigation
- `src/types_enums.h` - Added INPUT_FUNC_PAUSEMENU enum value
- `src/Devices/sithControl.c` - Added pause menu input function processing and mapping

### Key Functions

- `jkGuiRend_ControllerMouseMove(int dx, int dy)` - Moves the GUI cursor by delta amounts (menu navigation)
- `jkGuiRend_ControllerMouseButton(int down)` - Triggers mouse button press/release events (menu navigation)
- `jkMain_do_guistate6()` - Opens the pause menu when PAUSEMENU input function is triggered
- `sithControl_ReadFunctionMap()` - Checks if input functions are active during gameplay

### Configuration

- **Dead Zone**: 8000 (out of ±32767)
- **Sensitivity**: 5.0x multiplier for axis movement
- **Menu Navigation**: A Button (Button 0) for clicking
- **Pause Menu**: Mappable through Controls > Joystick GUI (search for "PAUSEMENU" action)
- **Debug Logging**: Can be enabled by defining `JOY_MENU_DEBUG` in `jkGUIRend.h`

### Platform Support

- **Target Platform**: SDL2/Linux
- **Joystick Support**: Uses SDL2 joystick events
- **Menu Detection**: Menu navigation only active when `jkGuiRend_activeMenu` is not NULL
- **Action Mapping**: Pause menu uses the standard input mapping system like other game actions

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