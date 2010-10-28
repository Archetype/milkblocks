#ifndef _SPIECE_H_
#define _SPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class SPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		SPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif