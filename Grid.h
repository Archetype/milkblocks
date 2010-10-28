#ifndef _GRID_H_
#define _GRID_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef _VECTOR_
#include "vector"
#endif

#ifndef _BLOCK_H_
#include "Block.h"
#endif

#ifndef _PIECE_H_
#include "Piece.h"
#endif

#ifndef _BITMAP_H_
#include "Bitmap.h"
#endif

#ifndef _RANDOM_H_
#include "Random.h"
#endif

class HUD;

class Grid : public Animatable
{
	private:
		HUD* mHUD;
		Random mRand;
		//unsigned char* ReflectionBuffer;
		Bitmap* mReflectionBitmap;
		Texture* mReflection;
		Texture* mReflectionFade;
		bool mReflectionUpdateRequired;
		int mForceDrawReflection;

		int mSlamRightTime;
		int mSlamLeftTime;

		int _Score;
		int _Lines;
		int _Level;
		Piece* CurrentPiece;
		bool Dropping;
		bool Faster;
		int CommitWait;
		int _CellSize;
		std::vector<Block*> Cells;

		int LastTickTime;
		int _TickTime;
		int _TickCount;

		void SlamPieceLeft();
		void SlamPieceRight();

	public:
		Piece* NextPiece( bool peek = true );
		void onKeyRelease( char action );
		void onKeyPress( char action );
	
	private:
		static void s_onHitGround( Animation* animation, Animatable* animatable, void* object );
		void onHitGround( Animation* animation, Animatable* animatable );

		static void s_onRebound( Animation* animation, Animatable* animatable, void* object );
		void onRebound( Animation* animation, Animatable* animatable );

		static void s_onDropped( Animation* animation, Animatable* animatable, void* object );
		void onDropped( Animation* animation, Animatable* animatable );

		static void s_onRowsDestroyed( Animation* animation, Animatable* animatable, void* object );
		void onRowsDestroyed( Animation* animation, Animatable* animatable );

		static void s_onRowsDropped( Animation* animation, Animatable* animatable, void* object );
		void onRowsDropped( Animation* animation, Animatable* animatable );

		static void s_onReboundRows( Animation* animation, Animatable* animatable, void* object );
		void onReboundRows( Animation* animation, Animatable* animatable );

		static void s_onRowsFullyDropped( Animation* animation, Animatable* animatable, void* object );
		void onRowsFullyDropped( Animation* animation, Animatable* animatable );

		void Commit( Piece* piece );

	public:
		static const int GridWidth = 10;
		static const int GridHeight = 20;
		static const char RIGHT = 0;
		static const char LEFT = 1;
		static const char FASTER = 2;
		static const char DROP = 3;
		static const char TURN = 4;

		bool GameOver;

		Grid( HUD* hud );
		~Grid();

		int getScore() { return _Score; }
		int getLines() { return _Lines; }
		int getLevel() { return _Level; }
		int getCellSize() { return _CellSize; }

		bool CheckCollision( Piece& piece, int offsetX=0, int offsetY=0 );
		void onAdvance();
		void GameAdvance();

		void RenderShadows( double alpha );
		void RenderReflection();
		void UpdateReflection();
};

#endif