#include "Platform.h"
#include <stdio.h>
#include <assert.h>

#ifdef __WIN32
#include "mmgr.h"
#endif

Platform* Platform::sm_Platform = NULL;

Platform::Platform()
{
	assert(sm_Platform == NULL);
	sm_Platform = this;
}