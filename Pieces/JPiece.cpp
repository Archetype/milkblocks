#include "JPiece.h"

static char __Grid[] =
{
		0, 0, 0, 
		1, 1, 1, 
		0, 0, 1,

		0, 1, 1, 
		0, 1, 0, 
		0, 1, 0,
		
		1, 0, 0, 
		1, 1, 1, 
		0, 0, 0,
		
		0, 0, 1, 
		0, 0, 1, 
		0, 1, 1
};

char* JPiece::RotationGrid()
{
	return __Grid;
}

int JPiece::States()
{
	return 4;
}

int JPiece::Size()
{
	return 3;
}