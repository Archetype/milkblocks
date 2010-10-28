#include "ZPiece.h"

static char __Grid[] =
{
		0, 0, 0,
		1, 1, 0,
		0, 1, 1,

		0, 0, 1,
		0, 1, 1,
		0, 1, 0
};

char* ZPiece::RotationGrid()
{
	return __Grid;
}

int ZPiece::States()
{
	return 2;
}

int ZPiece::Size()
{
	return 3;
}