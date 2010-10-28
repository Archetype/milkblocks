#include "SPiece.h"

static char __Grid[] =
{
		0, 0, 0,
		0, 1, 1,
		1, 1, 0,

		0, 1, 0,
		0, 1, 1,
		0, 0, 1
};

char* SPiece::RotationGrid()
{
	return __Grid;
}

int SPiece::States()
{
	return 2;
}

int SPiece::Size()
{
	return 3;
}