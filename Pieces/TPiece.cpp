#include "TPiece.h"

static char __Grid[] =
{
		0, 0, 0,
		1, 1, 1,
		0, 1, 0,

		0, 1, 0,
		0, 1, 1,
		0, 1, 0,

		0, 1, 0,
		1, 1, 1,
		0, 0, 0,

		0, 1, 0,
		1, 1, 0,
		0, 1, 0
};

char* TPiece::RotationGrid()
{
	return __Grid;
}

int TPiece::States()
{
	return 4;
}

int TPiece::Size()
{
	return 3;
}