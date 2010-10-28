#include "Piece.h"
#include "Grid.h"
#include "math.h"

static char __Grid[] =
{
		0, 0, 0,
		1, 1, 1,
		1, 0, 0,
	
		0, 1, 0,
		0, 1, 0,
		0, 1, 1,
	
		0, 0, 1,
		1, 1, 1,
		0, 0, 0,

		0, 1, 1,
		0, 0, 1,
		0, 0, 1,
};

char* Piece::RotationGrid()
{
	return __Grid;
}

int Piece::States()
{
	return 4;
}

int Piece::Size()
{
	return 3;
}

Piece::Piece( Grid* grid )
{
	name = "Piece";
	GameGrid = grid;
	FreeFall = 0;
	RotationIdx = 0;
}

void Piece::Initialize()
{
	_GridHeight =
	_GridWidth = Size();

	int blocks = 0;
	int i;
	int c;
	int r;
	
	char* BlockGrid = RotationGrid();

	for( i = 0; i < States(); i++ )
	{
		for( r = 0; r < _GridHeight; r++ )
		{
			for( c = 0; c < _GridWidth; c++ )
			{
				if( BlockGrid[i*_GridHeight*_GridWidth+r*_GridWidth+c] == 1 )
					blocks++;
			}
		}
		break;
	}
			
	for( i = 0; i < _GridHeight*_GridWidth; i++ )
		Cells.push_back( NULL );
	
	for( i = 0; i < blocks; i++ )
	{
		Block* block = new Block();
		addChild( block );
	}
	Set( 0 );
}

Piece::~Piece()
{
	for( iterator i = begin(); i != end(); i++ )
	{
		delete (*i);
	}	
	clear();
}

double Piece::getWidth()
{ 
	return _GridWidth*GameGrid->getCellSize(); 
}

double Piece::getHeight()
{ 
	return _GridHeight*GameGrid->getCellSize(); 
}

void Piece::Set( int idx )
{
	char* BlockGrid = RotationGrid()+idx*_GridWidth*_GridHeight;
	Cells.clear();
		
	int blockIdx = 0;
	for( int r = 0; r < _GridHeight; r++ )
	{
		for( int c = 0; c < _GridWidth; c++ )
		{
			if( BlockGrid[r*_GridWidth+c] == 1 )
			{
				Block* block = static_cast<Block*>((*this)[blockIdx]);
				block->x = GameGrid->getCellSize() * c;
				block->y = GameGrid->getCellSize() * r;
				if( c == 0 )
					block->setLeft( NULL );
				else
					block->setLeft( Cells[Cells.size()-1] );
				if( r == 0 )
					block->setTop( NULL );
				else
					block->setTop( Cells[(r-1)*_GridWidth+c] );
				Cells.push_back( block  );
				blockIdx++;
			}
			else
				Cells.push_back( NULL );
		}
	}
}

Block* Piece::GetBlock( int x, int y )
{
	return Cells[y*_GridWidth+x];
}

Block* Piece::RemoveBlock( int x, int y )
{
	if( x >= _GridWidth || y >= _GridHeight ) return NULL;
	Block* block = Cells[y*_GridWidth+x];
	if( block != NULL )
	{
		Cells[y*_GridWidth+x] = NULL;
		
		for( iterator i = begin(); i != end(); i++ )
		{
			if( (*i) == block )
			{
				erase( i );
				break;
			}
		}
	}
	return block;
}

int Piece::Center()
{
	float count = 0;
	int totalX = 0;
	
	for( int ix = 0; ix < _GridWidth; ix++ )
	{
		for( int iy = 0; iy < _GridHeight; iy++ )
		{
			Block* block = Cells[iy*_GridWidth+ix];
			if( block == NULL ) continue;
			count++;
			totalX += ix;
		}
	}
	return int( floor( totalX/count ) );
}

int Piece::BaseLine()
{
	for( int ix = 0; ix < _GridWidth; ix++ )
	{
		for( int iy = 0; iy < _GridHeight; iy++ )
		{
			Block* block = Cells[iy*_GridWidth+ix];
			if( block == NULL ) continue;
			return iy;
		}
	}
	return 0;
}

bool Piece::isSolid( int x, int y )
{
	return Cells[ y*_GridWidth+x ] != NULL;
}