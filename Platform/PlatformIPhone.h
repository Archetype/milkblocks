#ifndef _PLATFORMWINDOWS_H_
#define _PLATFORMWINDOWS_H_

#ifndef _PLATFORM_H_
#include "Platform/Platform.h"
#endif

class PlatformIPhone : public Platform
{
	double m_StartTime;
	
	public:
		PlatformIPhone();
		int GetElapsedMs();
};

#endif