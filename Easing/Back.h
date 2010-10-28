struct Back
{
	static double easeIn( double t, double d )
	{
		double s = 1.70158;
		return (t/=d)*t*((s+1)*t - s);
	}
	
	static double easeOut( double t, double d )
	{
		double s = 1.70158;
		return ((t=t/d-1)*t*((s+1)*t + s) + 1);
	}
	
	static double easeInOut( double t, double d )
	{
		double s = 1.70158; 
		if ((t/=d/2) < 1) return 0.5*(t*t*(((s*=(1.525))+1)*t - s));
		return 0.5*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2);
	}
};
