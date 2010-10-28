#ifndef _INC_MATH
#include "math.h"
#endif

struct Circ 
{
	static double easeIn (double t, double d) 
	{
		return -(sqrt(1 - (t/=d)*t) - 1);
	}
	
	static double easeOut (double t, double d) 
	{
		return sqrt(1 - (t=t/d-1)*t);
	}
	
	static double easeInOut (double t, double d) 
	{
		if ((t/=d/2) < 1) return -0.5 * (sqrt(1 - t*t) - 1);
		return 0.5 * (sqrt(1 - (t-=2)*t) + 1);
	}
};
