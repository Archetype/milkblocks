#ifndef _EASE_SINE_H_
#define _EASE_SINE_H_

#ifndef _INC_MATH
#include "math.h"
#endif

#ifndef PI
#define PI 3.14159265359
#endif

struct Sine 
{
	static double easeIn (double t, double d) 
	{
		return -cos(t/d * (PI/2)) + 1;
	}
	
	static double easeOut (double t, double d) 
	{
		return sin(t/d * (PI/2));
	}
	
	static double easeInOut (double t, double d) 
	{
		return -0.5 * (cos(PI*t/d) - 1);
	}
};

#endif