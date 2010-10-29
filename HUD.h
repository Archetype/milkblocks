#ifndef _HUD_H_
#define _HUD_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef _GRID_H_
#include "Grid.h"
#endif

#ifndef _ANIMATABLE_H_
#include "Animatable.h"
#endif

#ifndef _LINEAR_H_
#include "Easing/Linear.h"
#endif


#ifndef _EASE_SINE_H_
#include "Easing/Sine.h"
#endif

#ifndef _FONT_H_
#include "font/font.h"
#endif

#include "PlatformSingleton.h"

class HUD : public Animatable
{
	Piece* mNextPiece;
	Animatable PieceContainer;
	Font mFont;
	Font mSmallFont;

	int mAnimateToScore;
	int mAnimateScoreTime;
	int mAnimateFromScore;
	int mScoreNumber;
	char mScoreBuffer[256];
	char mLinesBuffer[256];
	char mLevelBuffer[256];

	public:

		HUD() : mNextPiece( NULL ), mFont( FontData( "DINNeuzeitGroteskStd-Light.otf", 24, Font::AntiAlias ) ), mSmallFont( FontData( "DINNeuzeitGroteskStd-Light.otf", 18, Font::AntiAlias ) )
		{
			memset( mScoreBuffer, 0, sizeof(mScoreBuffer) );
			memset( mLevelBuffer, 0, sizeof(mLevelBuffer) );
			memset( mLinesBuffer, 0, sizeof(mLinesBuffer) );

			mAnimateToScore = 0;
			mAnimateScoreTime = 0;
			mAnimateFromScore = 0;
			mScoreNumber = -1;
			name = "HUD";
			x = 320.0;
			y = 0.0;

			PieceContainer.x = 168.0/2.0;
			PieceContainer.y = 279.0/2.0 - 20.0;
			addChild( &PieceContainer );
		}

		void NextPieceChanged( Grid* grid )
		{
			if( mNextPiece != NULL )
			{
				Animation* anim = PieceContainer.Animate( Animatable::ALPHA, 0, 100, s_onHidden, Linear::easeNone, this );
				anim->Data = grid;
			}
			else
				ShowNext( grid );
			//if( mNextPiece != NULL )
			//	mNextPiece->deleteOnNextUpdate();

			//mNextPiece = grid->NextPiece();
			//addChild( mNextPiece );
		}

		void ShowNext( Grid* grid )
		{
			if( mNextPiece != NULL )
				mNextPiece->deleteOnNextUpdate();

			mNextPiece = grid->NextPiece();
			PieceContainer.addChild( mNextPiece );
			mNextPiece->x = -mNextPiece->getWidth()/2;
			mNextPiece->y = -mNextPiece->getHeight()/2;
			PieceContainer.scaleX = PieceContainer.scaleY = 0.01;
			PieceContainer.alpha = 0;
			PieceContainer.Animate( Animatable::ALPHA, 1, 200 );
			PieceContainer.Animate( Animatable::SCALE_X, 1, 100, NULL, Sine::easeInOut );
			PieceContainer.Animate( Animatable::SCALE_Y, 1, 100, NULL, Sine::easeInOut );
		}

		static void s_onHidden( Animation* animation, Animatable* animatable, void* object )
		{
			HUD* hud = static_cast<HUD*>(object);
			hud->onHidden( animation, animatable );
		}

		void onHidden( Animation* animation, Animatable* animatable )
		{
			ShowNext( static_cast<Grid*>(animation->Data) );
		}
		

		void UpdateScore( Grid* grid )
		{
			sprintf( mLinesBuffer, "%d", grid->getLines() );
			sprintf( mLevelBuffer, "%d", grid->getLevel() );
			
			mAnimateScoreTime = PlatformSingleton::getInstance()->GetElapsedMs();
			
			mAnimateFromScore = mScoreNumber;
			mAnimateToScore = grid->getScore();
		}

		void onRender()
		{
			if( mAnimateToScore != mScoreNumber )
			{
				double f = (PlatformSingleton::getInstance()->GetElapsedMs() -mAnimateScoreTime)/200.0;
				
				if( f > 1 ) f = 1;
				mScoreNumber = (1.0 - f)*mAnimateFromScore + mAnimateToScore*f; 
				strcpy( mScoreBuffer, Font::formatDigits(mScoreNumber) );
			}

			glPushMatrix();
			glTranslatef( 0, 35, 0 );
			mFont.renderRJ(mScoreBuffer, 158); //168
			glTranslatef( 0, 208, 0 );
			mSmallFont.renderRJ(mLinesBuffer, 158); //168
			glTranslatef( 0, 28, 0 );
			mSmallFont.renderRJ(mLevelBuffer, 158); //168
			glPopMatrix();
		}

};

#endif