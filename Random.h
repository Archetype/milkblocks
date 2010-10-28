#ifndef _RANDOM_H_
#define _RANDOM_H_

#ifndef _INC_MATH
#include "math.h"
#endif

#ifndef _INC_TIME
#include "time.h"
#endif

class Random
{
	unsigned int mSeed;

	public:
		Random( unsigned int seed = 1 )
		{
			mSeed = seed;
		}
		
		static unsigned int RandomSeed()
		{
			return ((unsigned int) time( NULL ));
		}
		
		double next()
		{
			return ((mSeed = (mSeed * 16807) % 2147483647) / 2147483647.0);
		}
		
		double peek()
		{
			return (((mSeed * 16807) % 2147483647) / 2147483647.0);
		}
};

#endif