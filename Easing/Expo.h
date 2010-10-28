#ifndef _INC_MATH
#include "math.h"
#endif

struct Expo 
{
	static double easeIn( double t, double d ) 
	{
		return (t==0) ? 0 : 1 * pow(2, 10 * (t/d - 1));
	}
	
	static double easeOut( double t, double d ) 
	{
		return (t==d) ? 1 : 1 * (-pow(2, -10 * t/d) + 1);
	}
	
	static double easeInOut( double t, double d ) 
	{
		if (t==0) return 0;
		if (t==d) return 1;
		if ((t/=d/2) < 1) return 1/2 * pow(2, 10 * (t - 1));
		return 1/2 * (-pow(2, -10 * --t) + 2);
	}
};