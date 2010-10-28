#ifndef _PIECE_H_
#define _PIECE_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef _VECTOR_
#include "vector"
#endif

#ifndef _BLOCK_H_
#include "Block.h"
#endif

#ifndef _ANIMATABLE_H_
#include "Animatable.h"
#endif

class Grid;

class Piece : public Animatable
{
	private:
		int _GridWidth;
		int _GridHeight;
		int RotationIdx;
		std::vector<Block*> Cells;
		Grid* GameGrid;

		void Set( int idx );

	protected:
		virtual char* RotationGrid();
		virtual int States();
		virtual int Size();

	public:
		int IX;
		int IY;
		int FreeFall;


		Piece( Grid* grid );
		~Piece();

		void Initialize();

		Block* GetBlock( int x, int y );
		Block* RemoveBlock( int x, int y );
		int Center();
		int BaseLine();
		bool isSolid( int x, int y );

		int getGridHeight() { return _GridHeight; }
		int getGridWidth() { return _GridWidth; }

		double getWidth();// { return _GridWidth*GameGrid->getCellSize(); }
		double getHeight();// { return _GridHeight*GameGrid->getCellSize(); }

		void Rotate( int idx = -1 )
		{
			if( idx == -1 )
				RotationIdx=(RotationIdx+1)%States();
			else
				RotationIdx = idx%States();
			Set( RotationIdx );
		}
		
		int getRotation()
		{
			return RotationIdx;
		}
};

#endif