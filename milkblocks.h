#ifndef _MILKBLOCKS_H_

#ifdef MILK_MAC_OSX
#include <GLUT/glut.h>
#else
#ifndef  __FREEGLUT_H__
#include <GL/freeglut.h>
#endif

#endif

#ifndef _TEXTURE_H_
#include "texture.h"
#endif

void glDrawRectangle( float width, float height );

#endif
