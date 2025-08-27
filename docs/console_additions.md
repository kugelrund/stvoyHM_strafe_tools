
# Console Additions

## Variables

### Strafe Helper

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

### Strafing Modifications

- `pmove_velocitySnapping` (0 or 1)

  Allows to enable or disable [velocity snapping](velocity_snapping.md).
  Default: `1`.
