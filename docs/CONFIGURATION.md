# OpenJKDF2 Configuration

This document describes configuration variables (cvars) available in OpenJKDF2.

## Physics Configuration

### phys_thrustScale
- **Type**: Float
- **Default**: 1.0
- **Range**: Any positive value
- **Description**: Global scale factor for all sector thrust (water current) physics calculations. This affects the strength of water currents and similar environmental forces that push the player and objects.
  - Value of 1.0 = Normal thrust strength (original game behavior)
  - Values > 1.0 = Stronger water currents and environmental forces
  - Values < 1.0 = Weaker water currents and environmental forces
  - Value of 0.0 = Disable all thrust effects

### Usage Examples
```
# Set normal thrust strength (default)
phys_thrustScale 1.0

# Make water currents twice as strong
phys_thrustScale 2.0

# Make water currents half as strong
phys_thrustScale 0.5

# Disable all thrust effects
phys_thrustScale 0.0
```

This setting is saved globally and will persist between game sessions.