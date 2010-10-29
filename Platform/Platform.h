#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifndef _INC_STDIO
#include <stdio.h>
#endif


#ifndef _APPLICATION_H_
#include "Platform/Application.h"
#endif


typedef void (*ManipulationCallback)(int, float, float);

class Platform
{
	static Platform* sm_Platform;
	Application* m_Application;

	ManipulationCallback m_ManipulationStartedCallback;
	ManipulationCallback m_ManipulationDeltaCallback;
	ManipulationCallback m_ManipulationEndedCallback;

	public:
		Platform();
		static Platform* Get() { return sm_Platform; }
		static Application* GetApplication() { return sm_Platform->m_Application; }
		virtual int GetElapsedMs() { return 0; }
		virtual bool InitGL(char* windowName, int width, int height) { return false; }
		virtual void StartRenderLoop(Application* app) { m_Application = app; }

		void RegisterManipulationCallbacks(void (*started)(int, float, float), void (*delta)(int, float, float), void (*ended)(int, float, float))
		{
			m_ManipulationStartedCallback = started;
			m_ManipulationDeltaCallback = delta;
			m_ManipulationEndedCallback = ended;
		}

	protected:
		void OnManipulationStarted(int index, float x, float y)
		{
			if(m_ManipulationStartedCallback != NULL)
			{
				m_ManipulationStartedCallback(index, x, y);
			}
		}

		void OnManipulationDelta(int index, float x, float y)
		{
			if(m_ManipulationDeltaCallback != NULL)
			{
				m_ManipulationDeltaCallback(index, x, y);
			}
		}

		void OnManipulationEnded(int index, float x, float y)
		{
			if(m_ManipulationEndedCallback != NULL)
			{
				m_ManipulationEndedCallback(index, x, y);
			}
		}
};

#endif