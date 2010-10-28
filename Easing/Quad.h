struct Quad 
{
	static double easeIn( double t, double d ) 
	{
		return (t/=d)*t;
	}
	
	static double easeOut( double t, double d ) 
	{
		return -(t/=d)*(t-2);
	}
	
	static double easeInOut( double t, double d ) 
	{
		if ((t/=d/2) < 1) return 0.5*t*t;
		return -0.5 * ((--t)*(t-2) - 1);
	}
};