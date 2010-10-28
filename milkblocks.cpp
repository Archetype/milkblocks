
#ifdef MILK_MAC_OSX
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "texture.h"
#include "Bitmap.h"
#include "Block.h"
#include "Grid.h"
#include "math.h"
#include "HUD.h"

Grid* GameGrid = NULL;
HUD* GameHUD = NULL;
Texture* background = NULL;
int ScreenWidth = 0;
int ScreenHeight = 0;
#ifdef WIN32
#include <crtdbg.h>
#endif



static void
resize(int width, int height)
{
	glViewport(0, 0, (ScreenWidth=width), (ScreenHeight=height));
}

GLuint rectangleID = 0;
void glDrawRectangle( float width, float height )
{	
	glPushMatrix();
	glScalef( width, height, 1 );
	glCallList( rectangleID );
	glPopMatrix();
}

static void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho( 0, ScreenWidth, ScreenHeight, 0, 0, 100 );
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, background->glID() );

	glPushMatrix();
	glTranslated( floor(ScreenWidth/2.0f - background->width()/2.0f), floor(ScreenHeight/2.0f - background->height()/2.0f), 0 );
	glDrawRectangle( background->width(), background->height() );
	glPopMatrix();

	GameGrid->x = floor( ScreenWidth/2.0f - background->width()/2.0f + 263 );
	GameGrid->y = floor( ScreenHeight/2.0f - background->height()/2.0f + 70 );
	
	GameGrid->Advance();
	GameGrid->UpdateAnimations();
	GameGrid->RenderShadows( 1 );
	GameGrid->Render( 1 );
	GameGrid->UpdateReflection();
	GameGrid->RenderReflection();
	
    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y)
{
	switch( key )
	{
		case ' ':
			GameGrid->onKeyPress( Grid::DROP );
			break;
	}
	glutPostRedisplay();
}

static void special_up (int key, int x, int y)
{
	switch (key)
    {
		case GLUT_KEY_UP:
			GameGrid->onKeyRelease( Grid::TURN );
			break;

		case GLUT_KEY_DOWN:
			GameGrid->onKeyRelease( Grid::FASTER );
			break;

		case GLUT_KEY_RIGHT:
			GameGrid->onKeyRelease( Grid::RIGHT );
			break;

		case GLUT_KEY_LEFT:
			GameGrid->onKeyRelease( Grid::LEFT );
			break;
	}
	glutPostRedisplay();
}
static void special (int key, int x, int y)
{
	switch (key)
    {
		case GLUT_KEY_UP:
			GameGrid->onKeyPress( Grid::TURN );
			break;

		case GLUT_KEY_DOWN:
			GameGrid->onKeyPress( Grid::FASTER );
			break;

		case GLUT_KEY_RIGHT:
			GameGrid->onKeyPress( Grid::RIGHT );
			break;

		case GLUT_KEY_LEFT:
			GameGrid->onKeyPress( Grid::LEFT );
			break;
	}
	glutPostRedisplay();
}


static void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInitWindowSize((ScreenWidth=903),(ScreenHeight=700));
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("MilkBlocks");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(special);
	glutSpecialUpFunc(special_up);
    glutIdleFunc(idle);

#if not defined(MILK_MAC_OSX)
	/* is this is a free glut extentsion? */
	glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;
#endif

    glClearColor(37/255.0,31/255.0,21/255.0,0);

	Font::initialize();
	
	rectangleID = glGenLists( 1 );
	glNewList( rectangleID, GL_COMPILE );
		glBegin( GL_QUADS );
			glTexCoord2f( 0, 0 );
			glVertex2f( 0, 0 );

			
			glTexCoord2f( 1, 0 );
			glVertex2f( 1, 0 );
			
			glTexCoord2f( 1, 1 );
			glVertex2f( 1, 1 );

			
			glTexCoord2f( 0, 1 );
			glVertex2f( 0, 1 );
		glEnd();
	glEndList();

	GameHUD = new HUD();
	GameGrid = new Grid( GameHUD );
	GameGrid->addChild( GameHUD );
	
	background = new Texture( TextureData( "bg.png", Texture::SendToGPU | Texture::Linear ) );

	// load block textures...
	Block::smTexture[0] = new Texture( TextureData( "block.png", Texture::SendToGPU | Texture::Linear ) );
	Block::smTexture[1] = new Texture( TextureData( "blockleft.png", Texture::SendToGPU | Texture::Linear ) );
	Block::smTexture[2] = new Texture( TextureData( "blocktop.png", Texture::SendToGPU | Texture::Linear ) );
	Block::smTexture[3] = new Texture( TextureData( "blocktopleft.png", Texture::SendToGPU | Texture::Linear ) );
	Block::smTexture[4] = new Texture( TextureData( "shadow.png", Texture::SendToGPU | Texture::Linear ) );

	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glutMainLoop();

	Font::shutdown();

#ifdef WIN32
    /* DUMP MEMORY LEAK INFORMATION */
    _CrtDumpMemoryLeaks () ;
#endif

    return EXIT_SUCCESS;
}
