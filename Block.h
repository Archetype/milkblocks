#ifndef _BLOCK_H_
#define _BLOCK_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef _ANIMATABLE_H_
#include "Animatable.h"
#endif

class Block : public Animatable
{
	public:
		static bool RenderingShadows;

		static Texture* smTexture[5];
		Texture* mTexture;

		unsigned char mFlags;
		enum Flags
		{
			LEFT_BORDER = (1<<0),
			TOP_BORDER = (1<<1)
		};

		Block() : mFlags( 0 )
		{
			name = "Block";
			mTexture = smTexture[0];
		}

		void setLeft( Block* block )
		{
			if( block == NULL )
				mFlags &= ~LEFT_BORDER;
			else
				mFlags |= LEFT_BORDER;

			if( mFlags == (TOP_BORDER|LEFT_BORDER) )
				mTexture = smTexture[3];
			else if( mFlags & TOP_BORDER )
				mTexture = smTexture[2];
			else if( mFlags & LEFT_BORDER )
				mTexture = smTexture[1];
			else
				mTexture = smTexture[0];
		}

		void setTop( Block* block )
		{
			
			if( block == NULL )
				mFlags &= ~TOP_BORDER;
			else
				mFlags |= TOP_BORDER;

			if( mFlags == (TOP_BORDER|LEFT_BORDER) )
				mTexture = smTexture[3];
			else if( mFlags & TOP_BORDER )
				mTexture = smTexture[2];
			else if( mFlags & LEFT_BORDER )
				mTexture = smTexture[1];
			else
				mTexture = smTexture[0];
		}

		void onRender()
		{
			if( RenderingShadows )
			{
				glTranslatef( -5, -2, 0 );
				glBindTexture( GL_TEXTURE_2D, smTexture[4]->glID() );
				glDrawRectangle( 38, 39 );
				glTranslatef( 5, 2, 0 );
			}
			else
			{
				glTranslatef( -1, -1, 0 );
				glBindTexture( GL_TEXTURE_2D, mTexture->glID() );
				glDrawRectangle( 29, 29 );
				glTranslatef( 1, 1, 0 );
			}
		}
};

#endif