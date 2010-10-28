#ifndef _JPIECE_H_
#define _JPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class JPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		JPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif