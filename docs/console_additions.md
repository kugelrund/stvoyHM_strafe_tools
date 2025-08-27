
# Console Additions

## Strafe Helper

### Variables

- `cg_drawStrafeHelper` (0 or 1)

  Draw a strafe helper.
  Default: `0`.

- `cg_strafeHelperCenter` (0 or 1)

  Put the strafe helper in the center of the screen.
  Default: `1`.

- `cg_strafeHelperCenterMarker` (0 or 1)

  Draw a marker in the center of the strafe helper.
  Default: `1`.

- `cg_strafeHelperHeight` (#)

  Height of strafe helper bar.
  Default: `20.0`.

- `cg_strafeHelperScale` (#)

  Horizontal scale of the strafe helper.
  Angles correspond directly to the field of view if this scale is set to `360/cg_fov`.
  Default: `4.0`.

- `cg_strafeHelperSpeedScale` (#)

  Scale of the text for speed display tied to strafe helper.
  Note that this shows the speed after friction but prior to acceleration, as this is what is relevant for optimal strafing.
  It is not the intuitive current player speed.
  Default: `0.9`.

- `cg_strafeHelperSpeedY` (#)

  Vertical offset of the text for speed display, relative to the strafe helper.
  Default: `5.0` (directly under the strafe helper).

- `cg_strafeHelperY` (#)

  Vertical position of the strafe helper.
  Default: `50.0`.

## Strafing Modifications

### Commands

- `setacceleration <holomatch|singleplayer>`

  Sets all acceleration modification cvars to the defaults of Holomatch or Singleplayer.
  Note that movement is also influenced by other variables such as framerates and velocity snapping, which are left untouched by this command.

  To get the same velocity snapping as in singleplayer, `pmove_velocitySnapping` must be `1` and `vm_game` must be `1`.
  If this mod is compiled and provided as native DLL, `vm_game` can also be `0`.
  For more details see [velocity_snapping.md](velocity_snapping.md).

### Variables

- `pmove_accelerate` (#)

  Acceleration factor while on ground.
  Singleplayer uses `12.0`.
  Default: `10.0`.

- `pmove_airAccelerate` (#)

  Acceleration factor while in air.
  Singleplayer uses `4.0`.
  Default: `1.0`.

- `pmove_airDecelRate` (#)

  Extra acceleration factor when moving accelerating away from current movement direction while in air.
  Singleplayer uses `1.35`.
  Default: `1.0` (no effect).

- `pmove_jumpVelocity` (#)

  Initial vertical velocity when jumping.
  Singleplayer uses `225.0`.
  Default: `270.0`.

- `pmove_airCmdScaling` (0 or 1)

  Allows to enable or disable scaling of wishspeed with the scale of the user input cmd while in the air.
  When this scaling is enabled, acceleration in air is decreased while jump is being held.
  Singleplayer uses `0`.
  Default: `1`.

- `pmove_velocitySnapping` (0 or 1)

  Allows to enable or disable [velocity snapping](velocity_snapping.md).
  Default: `1`.
