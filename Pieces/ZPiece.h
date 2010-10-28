#ifndef _ZPIECE_H_
#define _ZPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class ZPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		ZPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif