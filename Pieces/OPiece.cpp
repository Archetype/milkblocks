#include "OPiece.h"

static char __Grid[] =
{
		1, 1, 
		1, 1 
};

char* OPiece::RotationGrid()
{
	return __Grid;
}

int OPiece::States()
{
	return 1;
}

int OPiece::Size()
{
	return 2;
}