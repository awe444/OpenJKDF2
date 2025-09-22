# Timer Multiplier Cvar (g_timerMultiplier)

## Overview

The `g_timerMultiplier` cvar allows users to multiply all COG timer durations in the game by a configurable factor. This affects all timer-based scripted events including cutscenes, scripted sequences, door timers, and other time-sensitive gameplay elements.

## Default Value

- **Default:** `1.0` (normal timing)
- **Type:** Floating point number (flex_t)
- **Flags:** Local setting (saved in player configuration)

## Usage

### Setting the Value

You can set the timer multiplier using the console or configuration files:

```
g_timerMultiplier 0.5    # Half speed timers (2x faster events)
g_timerMultiplier 1.0    # Normal timing (default)
g_timerMultiplier 2.0    # Double timing (2x slower events)
g_timerMultiplier 0.25   # Quarter timing (4x faster events)
```

### Examples

- **Speed up cutscenes:** Set `g_timerMultiplier 0.5` to make all scripted sequences play twice as fast
- **Slow down gameplay:** Set `g_timerMultiplier 2.0` to give more time for puzzle-solving or platforming sections
- **Debug timing issues:** Use extreme values like `0.1` or `10.0` to test timer-dependent code

## Affected Systems

This multiplier affects the following COG functions:

### Timer Functions
- `SetTimer()` - Sets a timer that triggers after a specified duration
- `SetTimerEx()` - Extended timer function with additional parameters
- `SetPulse()` - Sets a repeating timer that pulses at regular intervals

### Thing Timer Functions  
- `SetThingTimer()` - Sets a timer on a specific thing/object
- `SetThingPulse()` - Sets a repeating pulse timer on a thing/object

## Technical Details

The multiplier is applied during timer creation, multiplying the requested duration by the cvar value before converting to milliseconds. This ensures consistent timing behavior across all timer types.

**Implementation locations:**
- `src/Cog/sithCogFunction.c` - Main COG timer functions
- `src/Cog/sithCogFunctionThing.c` - Thing-specific timer functions
- `src/World/jkPlayer.c` - Cvar registration and variable definition

## Compatibility

This feature is backward compatible and does not affect save files or multiplayer compatibility when using the default value of `1.0`.

## Use Cases

- **Speedrunning:** Reduce cutscene times without skipping important story content
- **Accessibility:** Slow down timing-critical sections for players who need more time
- **Development:** Test timer-dependent code with accelerated or decelerated timing
- **Content Creation:** Create dramatic effects by adjusting the pacing of scripted sequences