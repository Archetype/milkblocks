#include "PlatformIPhone.h"
#include <stdio.h>
#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>

PlatformIPhone::PlatformIPhone()
{
	m_StartTime = CFAbsoluteTimeGetCurrent();
}

int PlatformIPhone::GetElapsedMs()
{
	return (int)((CFAbsoluteTimeGetCurrent()-m_StartTime)*1000);
}
