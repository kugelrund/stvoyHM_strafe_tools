#include "strafe_helper/strafe_helper_customization.h"

#include "../cgame/cg_local.h"


struct StrafeHelperColor {
	vec4_t data;
};

static const struct StrafeHelperColor color_accelerating = {{0.0f, 0.5f, 0.125f, 0.375f}};
static const struct StrafeHelperColor color_optimal = {{0.0f, 1.0f, 0.25f, 0.75f}};
static const struct StrafeHelperColor color_center = {{1.0f, 1.0f, 1.0f, 0.75f}};
static const struct StrafeHelperColor color_speed = {{1.0f, 1.0f, 1.0f, 0.9f}};
static const struct StrafeHelperColor color_fallback = {{1.0f, 1.0f, 1.0f, 1.0f}};

static struct StrafeHelperColor getColorForElement(const enum shc_ElementId element_id)
{
	switch (element_id) {
	case shc_ElementId_AcceleratingAngles:
		return color_accelerating;
	case shc_ElementId_OptimalAngle:
		return color_optimal;
	case shc_ElementId_CenterMarker:
		return color_center;
	case shc_ElementId_SpeedText:
		return color_speed;
	}
	CG_Error("StrafeHelper: shc_ElementId out of range");
	return color_fallback;
}

void shc_drawFilledRectangle(const float x, const float y,
                             const float w, const float h,
                             const enum shc_ElementId element_id)
{
	const struct StrafeHelperColor color = getColorForElement(element_id);
	CG_FillRect(x, y, w, h, color.data);
}

static int getFontSizeFromScale(int scale)
{
	switch (scale) {
	case 1:
		return UI_TINYFONT;
	case 2:
		return UI_SMALLFONT;
	case 3:
		return UI_BIGFONT;
	default:
		return 0;
	}
}

void shc_drawString(const float x, const float y, const char* string,
                    const float scale, const enum shc_ElementId element_id)
{
	struct StrafeHelperColor color = getColorForElement(element_id);
	UI_DrawProportionalString(
		x, y, string, getFontSizeFromScale((int)scale) | UI_CENTER, color.data);
}
