#ifndef _LINEAR_H_
#define _LINEAR_H_

struct Linear 
{
	static double easeNone( double t, double d )
	{
		return t/d;
	}
	
	static double easeIn( double t, double d)
	{
		return t/d;
	}
	
	static double easeOut( double t, double d )
	{
		return t/d;
	}
	
	static double easeInOut (double t, double d)
	{
		return t/d;
	}
};
#endif