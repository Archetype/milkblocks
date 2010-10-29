#ifndef _PLATFORMWINDOWS_H_
#define _PLATFORMWINDOWS_H_

#ifndef _PLATFORM_H_
#include "Platform/Platform.h"
#endif

class PlatformWindows : public Platform
{
	bool m_IsClicked;
	int m_LastX;
	int m_LastY;

	public:
		int GetElapsedMs();
		bool InitGL(char* windowName, int width, int height);
		void StartRenderLoop(Application* app);

		void OnMouseClick( int button, int state, int x, int y);

		void OnMouseMove( int x, int y );
};

#endif