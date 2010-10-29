#include "PlatformWindows.h"
#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef __IPHONE_3_0
#include "mmgr.h"
#endif

static void display(void)
{
	Platform::GetApplication()->Advance(glutGet( GLUT_ELAPSED_TIME ));
}

static void idle()
{
    glutPostRedisplay();
}

bool wasCtrlPressed;

static void onMouseClick(int button, int state, int x, int y)
{
	((PlatformWindows*)Platform::Get())->OnMouseClick(button,state,x,y);
}

static void onMouseDrag(int x, int y)
{
	((PlatformWindows*)Platform::Get())->OnMouseMove(x,y);
}

static void onMouseMove(int x, int y)
{
    
}

void PlatformWindows::OnMouseClick( int button, int state, int x, int y)
{
	if(button == 0)
	{
		m_LastX = x;
		m_LastY = y;
		if( state == 1 )
		{
			m_IsClicked = false;
			OnManipulationEnded(0, x, y);
		}
		else
		{
			m_IsClicked = true;
			OnManipulationStarted(0, x, y);
		}
	}
	else if(button == 2)
	{
		if( state == 1 )
		{
			OnManipulationEnded(1, x, y);
		}
		else
		{
			OnManipulationStarted(1, x, y);
		}
	}
}

void PlatformWindows::OnMouseMove( int x, int y )
{
	if(m_IsClicked)
	{
		OnManipulationDelta(0, x-m_LastX, y-m_LastY);
		m_LastX = x;
		m_LastY = y;
	}
}

bool PlatformWindows::InitGL(char* windowName, int width, int height)
{
	m_IsClicked = false;
	m_LastX = 0;
	m_LastY = 0;

	glutInitWindowSize(width, height);
	glutInitWindowPosition(40,40);
	int argc = 0;
	char** argv = NULL;
	glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow(windowName);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  printf("Init Error: %s\n", glewGetErrorString(err));
	}
	if (!glewIsSupported("GL_VERSION_2_0"))
	{
		printf("OpenGL 2.0 not supported\n");
		return false;
	}

    glutDisplayFunc(display);
    glutIdleFunc(idle);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseDrag);
	glutPassiveMotionFunc(onMouseMove);

	return true;
}

int PlatformWindows::GetElapsedMs()
{
	return glutGet( GLUT_ELAPSED_TIME );
}

void PlatformWindows::StartRenderLoop(Application* app)
{
	Platform::StartRenderLoop(app);

	glutMainLoop();
}