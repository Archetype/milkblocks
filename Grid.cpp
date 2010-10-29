#include "Grid.h"
#include "math.h"
#include "Easing/Circ.h"
#include "Easing/Linear.h"
#include "time.h"
#include "HUD.h"

#include "Pieces/IPiece.h"
#include "Pieces/JPiece.h"
#include "Pieces/OPiece.h"
#include "Pieces/SPiece.h"
#include "Pieces/ZPiece.h"
#include "Pieces/TPiece.h"
#include "Pieces/LPiece.h"

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

Grid::Grid( HUD* hud ) : mRand( Random::RandomSeed() )
{
	mHUD = hud;
	mSlamRightTime = mSlamLeftTime = 0;
	//srand( time( NULL ) );
	GameOver = false;
	_Lines = 0;
	_Score = 0;
	_Level = 1;

	CurrentPiece = NULL;
	Dropping = false;
	CommitWait = 0;
	Faster = false;

	mForceDrawReflection = 0;
	_CellSize = 28;

	_TickTime = ((11 - _Level) * 50);
	_TickCount =
	LastTickTime = 0;

	for( int i = 0; i < GridWidth*GridHeight; i++ )
		Cells.push_back( NULL );

	x = 263;
	y = 70;

	mReflection = NULL;
	mReflectionBitmap = NULL;
	mReflectionFade = NULL;
	mReflectionUpdateRequired = NULL;
	if( mHUD != NULL )
		mHUD->UpdateScore( this );
}

Grid::~Grid()
{
	for( iterator i = begin(); i != end(); i++ )
	{
		delete (*i);
	}
	delete mReflection;
	delete mReflectionBitmap;
	delete mReflectionFade;
}

void Grid::SlamPieceLeft()
{
	if( CurrentPiece == NULL /*&& CurrentPiece->IY >= 0*/ ) return;
	int i;
	for( i = 0; !CheckCollision( *CurrentPiece, i, 0 ); i-- ){}
	i++;
	CurrentPiece->IX += i;
}

void Grid::SlamPieceRight()
{
	if( CurrentPiece == NULL /*&& CurrentPiece->IY >= 0*/ ) return;
	int i;
	for( i = 0; !CheckCollision( *CurrentPiece, i, 0 ); i++ ){}
	i--;
	CurrentPiece->IX += i;
}

bool Grid::CheckCollision( Piece& piece, int offsetX, int offsetY )
{
	// check piece
	//int idxY = piece.IY < 0 ? 0 : piece.IY;
	for( int ix = 0; ix < piece.getGridWidth(); ix++ )
	{
		for( int iy = 0; iy < piece.getGridHeight(); iy++ )
		{
			int py = piece.IY+iy+offsetY;
			if( py < 0 ) py = 0;
			int px = piece.IX+ix+offsetX;
			if( py < 0 ) continue;
			if( piece.isSolid( ix, iy ) && ( px < 0 /*|| py < 0*/ || px >= GridWidth || py >= GridHeight || Cells[py*GridWidth+px] != NULL ) )
			{
				return true;
			}
		}
	}
	return false;
}

void Grid::onKeyRelease( char action )
{
	if( GameOver ) return;
	switch( action )
	{
			// right
		case RIGHT:
			mSlamRightTime = 0;
			break;
			
			// left
		case LEFT:
			mSlamLeftTime = 0;
			break;
			
		case FASTER:
			if( Faster )
			{
				Faster = false;
				_TickTime *= 2;
				
				LastTickTime = PlatformSingleton::getInstance()->GetElapsedMs() -_TickTime;
			}
			break;
	}
}

void Grid::onKeyPress( char action )
{
	if( GameOver ) return;
	int i;
	switch( action )
	{
		// right
		case RIGHT:
			if( CurrentPiece != NULL && !Dropping )
			{
				if( /*CurrentPiece->IY >= 0 &&*/ !CheckCollision( *CurrentPiece, 1, 0 ) )
				{
					CurrentPiece->IX += 1;
				}
				CurrentPiece->x = CurrentPiece->IX*_CellSize;

				mSlamRightTime = PlatformSingleton::getInstance()->GetElapsedMs();
			}
			break;
			
		// left
		case LEFT:
			if( CurrentPiece != NULL && !Dropping )
			{
				if( /*CurrentPiece->IY >= 0 &&*/ !CheckCollision( *CurrentPiece, -1, 0 ) )
				{
					CurrentPiece->IX -= 1;
				}
				
				CurrentPiece->x = CurrentPiece->IX*_CellSize;

				mSlamLeftTime = PlatformSingleton::getInstance()->GetElapsedMs();
			}
			break;
			
		// down
		case FASTER:
			if( !Faster )
				_TickTime /= 2;
			Faster = true;
			break;

		// space					
		case DROP:
			if( CurrentPiece != NULL && !Dropping )
			{
				for( i = 0; !CheckCollision( *CurrentPiece, 0, i ); i++ ){}
				i--;
				Dropping = true;
				
				CurrentPiece->IY += i;
				Animation* animation = CurrentPiece->Animate( Animatable::Y, CurrentPiece->IY*_CellSize, i*5, s_onHitGround, Circ::easeIn, this );
				animation->Data = (void*)i;
				
			}
			break;
			
		// up
		case TURN:
			if( CurrentPiece != NULL && !Dropping )
			{
				CurrentPiece->Rotate( CurrentPiece->getRotation()+1 );
				if( CheckCollision( *CurrentPiece ) )
					CurrentPiece->Rotate( CurrentPiece->getRotation()-1 );
			}
			break;
	}
	//trace( event.keyCode );
}

void Grid::s_onHitGround( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onHitGround( animation, animatable );
}

void Grid::onHitGround( Animation* animation, Animatable* animatable )
{
	CurrentPiece->Animate( Animatable::Y, (CurrentPiece->IY*_CellSize)-min( _CellSize, ((int)animation->Data)*5.0 ), 100, s_onRebound, Circ::easeOut, this );
}

void Grid::s_onRebound( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onRebound( animation, animatable );
}

void Grid::onRebound( Animation* animation, Animatable* animatable )
{
	CurrentPiece->Animate( Animatable::Y, CurrentPiece->IY*_CellSize, 100, s_onDropped, Circ::easeIn, this );
}

void Grid::s_onDropped( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onDropped( animation, animatable );
}

void Grid::onDropped( Animation* animation, Animatable* animatable )
{
	Dropping = false;
	Commit( CurrentPiece );
	CurrentPiece = NULL;
}

void Grid::onAdvance()
{
	if( GameOver ) return;
	mReflectionUpdateRequired = false;
	
	int time = PlatformSingleton::getInstance()->GetElapsedMs();
	
	if( CurrentPiece != NULL && !Dropping && mSlamRightTime != 0 && time - mSlamRightTime >= 140 )
	{
		SlamPieceRight();
		mSlamRightTime = 0;
		CurrentPiece->x = CurrentPiece->IX*_CellSize;
	}
	if( CurrentPiece != NULL && !Dropping && mSlamLeftTime != 0 && time - mSlamLeftTime >= 140 )
	{
		SlamPieceLeft();
		mSlamLeftTime = 0;
		CurrentPiece->x = CurrentPiece->IX*_CellSize;
	}
	while( time - LastTickTime > _TickTime )
	{
		LastTickTime += _TickTime;
		GameAdvance();
		_TickCount++;
	}
	
	// check if current piece is in bottom 2 rows
	// UPDATE_REFLECTION
	//trace( (CurrentPiece.IY + CurrentPiece.GridHeight)  + " " +( GridHeight-2));
	if( mForceDrawReflection != 0 || ( CurrentPiece != NULL && CurrentPiece->IY + CurrentPiece->getGridHeight() > GridHeight-2 ) )
		mReflectionUpdateRequired = true;
}

Piece* Grid::NextPiece( bool peek )
{
	int pieceId = (int)floor( ( peek ? mRand.peek() : mRand.next() ) * 7 );
	switch( pieceId )
	{
		case 0:
			return new LPiece( this );
			break;
		case 1:
			return new IPiece( this );
			break;
		case 2:
			return new JPiece( this );
			break;
		case 3:
			return new OPiece( this );
			break;
		case 4:
			return new SPiece( this );
			break;
		case 5:
			return new ZPiece( this );
			break;
		case 6:
			return new TPiece( this );
			break;
	};
	return NULL;
}

void Grid::GameAdvance()
{
	if( GameOver ) return;
	if( CurrentPiece == NULL )
	{	
		// get a new piece
		CurrentPiece = NextPiece( false );

		//CurrentPiece = new OPiece( this );//new PieceTypes[ Math.round( Rand.next()*(PieceTypes.length-1) ) ]( this );
		// center of grid is at piece 5
		CurrentPiece->IX = 5 - CurrentPiece->Center();
		CurrentPiece->IY = -CurrentPiece->BaseLine();
		CurrentPiece->x = CurrentPiece->IX*_CellSize;
		CurrentPiece->y = CurrentPiece->IY*_CellSize;
		CurrentPiece->y -= _CellSize;
		CommitWait = 1;
		addChild( CurrentPiece );
		CurrentPiece->Animate( Animatable::Y, CurrentPiece->IY*_CellSize, _TickTime );
		if( mHUD != NULL )
			mHUD->NextPieceChanged( this );

		if( CheckCollision( *CurrentPiece ) )
		{
			GameOver = true;
		}
	}
	else
	{
		if( !Dropping )
		{
			if( CheckCollision( *CurrentPiece, 0, 1 ) )
			{
				if( CommitWait == 0 )
				{
					Commit( CurrentPiece );
					CurrentPiece = NULL;
				}
				else
					CommitWait--;
			}
			else
			{
				CurrentPiece->FreeFall++;
				CurrentPiece->IY++;
				CurrentPiece->Animate( Animatable::Y, CurrentPiece->IY*_CellSize, _TickTime );
			}
		}
	}
}

int sortByPosition(const void * v1, const void * v2)
{
    Animatable** data1 = (Animatable **)v1;
    Animatable** data2 = (Animatable **)v2;
	if( (*data1)->y > (*data2)->y )
		return 1;
	else if( (*data1)->y < (*data2)->y )
		return -1;
	else
	{
		if( (*data1)->x > (*data2)->x )
			return 1;
		else if( (*data1)->x < (*data2)->x )
			return -1;
	}
	return 0;
	//return (*data1)->x*(*data1)->y - (*data2)->x*(*data2)->y;
}

void Grid::Commit( Piece* piece )
{		
	if( GameOver ) return;
	int ix;
	int iy;
	//var drawReflection:Boolean = false;
	
	Block* block;
	for( iy = 0; iy <= piece->getGridHeight()+1; iy++ )
	{
		for( ix = 0; ix <= piece->getGridWidth()+1; ix++ )
		{
			int py = piece->IY + iy;
			int px = piece->IX + ix;
			block = piece->RemoveBlock( ix, iy );
			if( block != NULL )
			{
				block->x = px*_CellSize;
				block->y = py*_CellSize;
				Cells[py*GridWidth+px] = block;
				addChild( block );
				if( py >= GridHeight-2 )
					mReflectionUpdateRequired = true;
				//Container.addChild( block );
			}
			if( py >= 0 && px >= 0 && py < GridHeight && px < GridWidth && (block=Cells[py*GridWidth+px]) != NULL )
			{
				block->setLeft( px > 0 ? Cells[py*GridWidth+px-1] : NULL );
				block->setTop( py > 0 ? Cells[(py-1)*GridWidth+px] : NULL );
			}  
		}
	}
	
	qsort( &front(), size(), sizeof(Animatable*), sortByPosition );

	Animatable* dropper = new Animatable();
	Animatable* destroyer = new Animatable();
	int dropCount = 0;
	// obliterate rows
//	int i = 0;
	for( iy = 0; iy < GridHeight; iy++ )
	{
		bool kill = true;
		for( ix = 0; ix < GridWidth; ix++ )
		{
			if( (block = Cells[iy*GridWidth+ix]) == NULL )
			{
				kill = false;
				break;
			}
		}
		if( kill )
		{
			// if we're deleting one of the last two rows, then draw the reflection...
			if( iy >= GridHeight-2 )
				mReflectionUpdateRequired = true;
				
			dropCount++;
			for( ix = 0; ix < GridWidth; ix++ )
			{
				block = Cells[iy*GridWidth+ix];
				if( block != NULL )
				{
					//removeChild( block );
					destroyer->addChild( block );//addChild( block );
					Cells[iy*GridWidth+ix] = NULL;
				}
			}

			for( int j = iy; j > 0; j-- )
			{
				for( ix = 0; ix < GridWidth; ix++ )
				{
					block = (Cells[j*GridWidth+ix] = Cells[(j-1)*GridWidth+ix]); 
					if( block != NULL )
					{
						//int s = size();
						//removeChild( block );
						//if( s == size() )
						//{
						//	printf("ARGH");
						//}
						dropper->addChild( block );
						block->y = j*_CellSize;
					}
				}
			}
		}
	}
	
	Animation* animation;
	
	if( destroyer->size() != 0 )
	{
		if( mReflectionUpdateRequired )
			mForceDrawReflection++;
		addChild( destroyer );
		//Container.addChild( destroyer );
		animation = destroyer->Animate( Animatable::ALPHA, 0, 100, s_onRowsDestroyed, Linear::easeNone, this );
		void** data = new void*[3];
		data[0] = dropper;
		data[1] = (void*)dropCount;
		data[2] = (void*)mReflectionUpdateRequired;
		animation->Data = data;// [ dropper, dropCount, drawReflection ];
		//trace( drawReflection );
	}
	if( dropCount != 0 )
	{
		dropper->y = -dropCount*_CellSize;
		addChild( dropper );
		//Container.addChild( dropper );
		//animation = dropper.Animate( Y, 0, dropCount*100, onRowsDropped, Circ.easeIn );
		//animation.stop();
		//animation.userdata = dropCount;
	}
	///if( drawReflection )
	//	dispatchEvent( new Event( Grid.UPDATE_REFLECTION ) );
		
	// do score
	_Lines += dropCount;
	_Score += ( (21 + (3 * _Level)) - piece->FreeFall );
	
	if (_Lines <= 0)
		_Level = 1;
	else if ((_Lines >= 1) && (_Lines <= 90))
	  _Level = 1 + ((_Lines - 1) / 10); 
	else if (_Lines >= 91)
	  _Level = 10;
	_TickTime = ((11 - _Level) * 50);
	Faster = false;
	//removeChild( piece );
	//delete piece;
	piece->deleteOnNextUpdate();
	//dispatchEvent( new Event( Grid.SCORE_CHANGE ) );
	if( mHUD != NULL )
		mHUD->UpdateScore( this );
}

void Grid::s_onRowsDestroyed( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onRowsDestroyed( animation, animatable );
}

void Grid::onRowsDestroyed( Animation* animation, Animatable* animatable )
{	
	void** Data = static_cast<void**>(animation->Data);
	Animatable* dropper = static_cast<Animatable*>(Data[0]);
	int dropCount = (int)Data[1];
	
	if( dropCount != 0 )
	{
		Animation* anim = dropper->Animate( Animatable::Y, 0, dropCount*100, s_onRowsDropped, Circ::easeIn, this );
		anim->Data = animation->Data;
	}

	//removeChild( animatable );
	for( iterator i = animatable->begin(); i != animatable->end(); i++ )
	{
		delete (*i);
	}
	animatable->clear();
	animatable->deleteOnNextUpdate();
}

void Grid::s_onRowsDropped( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onRowsDropped( animation, animatable );
}

void Grid::onRowsDropped( Animation* animation, Animatable* animatable )
{	
	void** Data = static_cast<void**>(animation->Data);
	Animation* anim = animatable->Animate( Animatable::Y, -((int)Data[1])*10.0, 100, s_onReboundRows, Circ::easeOut, this );
	anim->Data = Data;
}

void Grid::s_onReboundRows( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onReboundRows( animation, animatable );
}

void Grid::onReboundRows( Animation* animation, Animatable* animatable )
{	
	void** Data = static_cast<void**>(animation->Data);
	Animation* anim = animatable->Animate( Animatable::Y, 0, 100, s_onRowsFullyDropped, Circ::easeIn, this );
	anim->Data = Data;
}

void Grid::s_onRowsFullyDropped( Animation* animation, Animatable* animatable, void* object )
{
	Grid* grid = static_cast<Grid*>(object);
	grid->onRowsFullyDropped( animation, animatable );
}

void Grid::onRowsFullyDropped( Animation* animation, Animatable* animatable )
{	
	void** Data = static_cast<void**>(animation->Data);
	//removeChild( animatable );
	for( int i = animatable->numChildren()-1; i >= 0; i-- )
	{
		Block* block = static_cast<Block*>((*animatable)[i]);
		addChild( block );
	}
	animatable->clear();
	animatable->deleteOnNextUpdate();
	qsort( &front(), size(), sizeof(Animatable*), sortByPosition );
	//dispatchEvent( new Event( Grid.UPDATE_REFLECTION ) );
	
	if( ((bool)Data[2]) )
//	if( (animation.userdata )
		mForceDrawReflection--;
	delete [] Data;
}

void Grid::RenderShadows( double alpha )
{
	Block::RenderingShadows = true;
	Render( alpha );
	Block::RenderingShadows = false;
}

void Grid::UpdateReflection()
{
	if( !mReflectionUpdateRequired && mReflectionBitmap != NULL ) return;
	if( mReflectionBitmap == NULL )
	{
		mReflectionBitmap = new Bitmap( 280, 70, Texture::Channels3 );//  = new unsigned char[280*560*3];
		mReflection = new Texture( TextureData( mReflectionBitmap, Texture::SendToGPU | Texture::Linear | Texture::KeepLocal ) );
		mReflectionFade = new Texture( TextureData( "reflectionfade.png", Texture::SendToGPU | Texture::Linear ) );
	}

	glReadPixels( (int)x, (int)y, 280, 70, GL_RGB, GL_UNSIGNED_BYTE, mReflectionBitmap->getWriteBytes() );
	mReflection->update();
	glEnable( GL_TEXTURE_2D );
}

void Grid::RenderReflection()
{
	if( mReflection == NULL ) return;
	glPushMatrix();
	glTranslatef( x, y+560, 0 );
	//glScalef( 1, -1, 0 );
	glBindTexture( GL_TEXTURE_2D, mReflection->glID() );
	glDrawRectangle( mReflection->width(), mReflection->height() );
	glBindTexture( GL_TEXTURE_2D, mReflectionFade->glID() );
	glDrawRectangle( mReflectionFade->width()+1, mReflectionFade->height() );
	glPopMatrix();
}