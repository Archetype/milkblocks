#ifndef _OPIECE_H_
#define _OPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class OPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		OPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif