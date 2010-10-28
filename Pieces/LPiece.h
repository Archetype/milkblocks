#ifndef _LPIECE_H_
#define _LPIECE_H_

#ifndef _PIECE_H_
#include "piece.h"
#endif

class LPiece : public Piece
{
	public:
		LPiece( Grid* grid ) : Piece( grid ) { Initialize(); }
};

#endif