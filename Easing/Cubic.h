struct Cubic 
{
	static double easeIn (double t, double d) 
	{
		return (t/=d)*t*t;
	}
	
	static double easeOut (double t, double d) 
	{
		return ((t=t/d-1)*t*t + 1);
	}
	
	static double easeInOut (double t, double d) 
	{
		if ((t/=d/2) < 1) return 0.5*t*t*t;
		return 0.5*((t-=2)*t*t + 2);
	}
};
