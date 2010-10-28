#include "IPiece.h"

static char __Grid[] =
{
		0, 0, 0, 0,
		1, 1, 1, 1,
		0, 0, 0, 0,
		0, 0, 0, 0,

		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 1, 0,
		0, 0, 1, 0
};

char* IPiece::RotationGrid()
{
	return __Grid;
}

int IPiece::States()
{
	return 2;
}

int IPiece::Size()
{
	return 4;
}