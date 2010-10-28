#ifndef _INC_MATH
#include "math.h"
#endif

#ifndef PI
#define PI 3.14159265359
#endif

struct Elastic 
{
	static double easeIn( double t, double d ) 
	{
		if (t==0) return 0;
		
		if ((t/=d)==1) return 1; 
		
		double a = 1;
		double p = d*0.3;
		double s = p/4.0;
		
		return -(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*PI)/p ));
	}
	
	static double easeOut( double t, double d ) 
	{
		if (t==0) return 0;  
		
		if ((t/=d)==1) return 1; 
		
		double a = 1;
		double p = d*0.3;
		double s = p/4.0;
		
		return (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + 1);
	}
	
	static double easeInOut( double t, double d ) 
	{
		if (t==0) return 0;  
		
		if ((t/=d/2)==2) return 1;
		
		double a = 1;
		double p = d*0.3*1.5;
		double s = p/4.0;
		
		if (t < 1) return -0.5*(a*pow(2,10*(t-=1)) * sin( (t*d-s)*(2*PI)/p ));
		return a*pow(2,-10*(t-=1)) * sin( (t*d-s)*(2*PI)/p )*0.5 + 1;
	}
};