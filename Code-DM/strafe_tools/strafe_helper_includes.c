#include "strafe_helper_includes.h"

/* this header is not actually used here, but the QVM code seems to break if we
 * do not include it here */
#include "../cgame/cg_local.h"


#ifdef __LCC__
float sqrtf(float arg)
{
	/* sqrt is exported directly for the QVM so can just use it. */
	return sqrt(arg);
}

float acosf(float arg)
{
	/* Luckily atan2 has been exported for the QVM, so we can compute
	 * acos with atan2 using some trigonometric relationship. */
	return 2.0 * atan2(sqrt(1.0f - arg * arg), 1.0f + arg);
}

float atan2f(float arg1, float arg2)
{
	/* atan2 is exported directly for the QVM so can just use it. */
	return atan2(arg1, arg2);
}

void snprintf(char* buffer, int bufsz, const char* format, float value)
{
	/* We only need to write one float value to string for our usecase, so
	 * luckily we do not need to deal with the variable argument stuff. */
	Com_sprintf(buffer, bufsz, format, value);
}
#endif
