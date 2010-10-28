#ifndef _IPIECE_H_
#define _IPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class IPiece : public Piece
{
	protected:
		char* RotationGrid();
		int States();
		int Size();
	public:
		IPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif