#ifndef _TPIECE_H_
#define _TPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class TPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		TPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif