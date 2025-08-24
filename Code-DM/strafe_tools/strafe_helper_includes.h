#ifndef STRAFE_HELPER_INCLUDES_H
#define STRAFE_HELPER_INCLUDES_H

#ifdef __LCC__
/* It seems like it's not possible to include math.h in QVM code. Therefore need
 * to define these manually so that they can be used in strafe_helper.c. */
float sqrtf(float arg);
float acosf(float arg);
float atan2f(float arg1, float arg2);
void snprintf(char* buffer, int bufsz, const char* format, float value);
#else
#include <math.h>
#include <stdio.h>
#endif

#endif /* STRAFE_HELPER_INCLUDES_H */
