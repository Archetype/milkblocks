#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#ifdef MILK_MAC_OSX

#include <GLUT/glut.h>
#include <stdlib.h>
#include <string.h>

#else

#ifndef  __FREEGLUT_H__
#include <GL/freeglut.h>
#endif

#endif
	
class Bitmap;

#ifndef BIT
#define BIT(x) (1<<x)
#endif


struct TextureData
{
	const char* filename;
	unsigned int flags;
	Bitmap* bitmap;

	TextureData( const char *f, unsigned int fl );
	TextureData( Bitmap* bmp, unsigned int fl );
};


class Texture
{
	public:
		unsigned int mReferences;
		unsigned int mFlags;

		enum Flags
		{
			Loaded = BIT( 0 ),
			AutoPrune = BIT( 1 ),
			Lazarus = BIT( 2 ),
			FreeFlag = BIT( 3 ),
			FreeBit = 3,

			KeepLocal				= FreeFlag,
			SendToGPU				= FreeFlag << 1,

			Linear					= FreeFlag << 2,
			Bilinear				= FreeFlag << 3,
			Trilinear				= FreeFlag << 4,
			Anisotropic				= FreeFlag << 5,

			FilteringBits			= Linear | Bilinear | Trilinear | Anisotropic,

			Wrap					= FreeFlag << 6,
			BuildMipmaps			= FreeFlag << 7,
			BuildMipMaps			= FreeFlag << 7,

			BumpMap					= FreeFlag << 8,

			Channels1				= FreeFlag << 9,
			Channels2				= FreeFlag << 10,
			Channels3				= FreeFlag << 11,
			Channels4				= FreeFlag << 12,

			ChannelBits				= Channels1 | Channels2 | Channels3 | Channels4,
			
			LowestChannelBit		= FreeBit + 9,

			PNG						= FreeFlag << 13,
			JPG						= FreeFlag << 14,
			BMP						= FreeFlag << 15,

			SentGPU					= FreeFlag << 16,

			RequestAlpha			= FreeFlag << 17,
			//AllocateNew				= FreeFlag << 18,
			ClampToEdge				= FreeFlag << 18,

			ClearMipBorders			= FreeFlag << 19,

			_INTERNAL_BITS			= ChannelBits | SentGPU /*| AllocateNew*/,
			_EXTERNAL_BITS			= KeepLocal|SendToGPU|Linear|Bilinear|Trilinear|Anisotropic|Wrap|BuildMipMaps|BumpMap|RequestAlpha|ClearMipBorders
		};

	private:

		static int smPreferenceFlag;		// user preferences (filtering)

		Bitmap* mBitmap;
		unsigned short	mHeight;						// pixel width
		unsigned short	mWidth;							// pixel height

		GLuint			mGL_TexID;						// OpenGL texture object id
		char*			mFileName;						// Texture's file name

	public:
		Texture( const TextureData& td );
		~Texture();

		inline bool operator==(const TextureData& _test) const
		{
			return _test.filename != NULL && mFileName != NULL && (strcmp( mFileName, _test.filename ) == 0) && ((mFlags & _EXTERNAL_BITS) == _test.flags);
		}

		Bitmap* bitmap() { return mBitmap; }

		int getPixelBytes()
		{
			float d = float((mFlags & ChannelBits) >> LowestChannelBit);
			return (((int&)d) >> 23) - 126;
		}

		inline int hasAlpha() { return (mFlags & Channels4) || (mFlags & Channels2); }
		inline const char* filename() { return mFileName; }
	
		void setAlpha( int x, int y, unsigned char c );
		void setGrayscale( int x, int y, unsigned char c );

		const unsigned char* getBytes();
		unsigned char* getWriteBytes();
		unsigned char getAlpha( int x, int y );
		unsigned char getGrayscale( int x, int y );

		void sendToGPU();

		inline GLuint glID() { return mGL_TexID; }
		inline unsigned short width() { return mWidth; }
		inline unsigned short height() { return mHeight; }

		static void resetAllFiltering();


		void clearFlag( unsigned int f ) { mFlags &= ~(f&Texture::_EXTERNAL_BITS); }
	protected:
	
		bool load( Bitmap* bmp, unsigned int flags );
		bool load( const char *filename, unsigned int flags );

		void processFlags( );

		void resetFiltering( );

	public:
		void update( unsigned char *data, int step, unsigned short width, unsigned short height );
		void update();
		bool load();
		void unload( bool lazarus );
};

#endif