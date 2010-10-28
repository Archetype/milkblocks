#ifndef _BITMAP_H_
#define _BITMAP_H_

#ifndef _TEXTURE_H_
#include "texture.h"
#endif

#ifndef _INC_STDIO
#include "stdio.h"
#endif

class Bitmap
{
	enum
	{
		MAX_MIP_LEVELS = 12
	};


	protected:
		unsigned int mFlags;

		unsigned int mWidth;
		unsigned int mHeight;

		unsigned char* mBytes;
		unsigned int mByteSize;
		unsigned int mMipmaps;
		unsigned int mOffsets[MAX_MIP_LEVELS];

		bool allocate();

		void buildMipmap( const unsigned char* srcBytes, unsigned char* bytes, unsigned int srcWidth, unsigned int srcHeight );

		bool readPNG( FILE* fp, const char* filename = NULL );
		//bool readJPG( FILE* fp, const char* filename = NULL );
		//bool readBMP( FILE* fp, const char* filename = NULL );

	public:
		Bitmap();
		Bitmap( const Bitmap& );
		Bitmap( unsigned int width, unsigned int height, int flags );
		Bitmap( const char* filename, int flags );
		~Bitmap();

		unsigned int flags() { return mFlags; }
		int getPixelBytes()
		{
			float d = float((mFlags & Texture::ChannelBits) >> Texture::LowestChannelBit);
			return (((int&)d) >> 23) - 126;
		}

		unsigned char* getAddress( const int x, const int y, const unsigned int miplevel = 0 );

		void buildMipmaps();
		inline int hasAlpha() { return (mFlags & Texture::Channels4) || (mFlags & Texture::Channels2); }

		unsigned int mipmaps() { return mMipmaps; }
		unsigned int width( unsigned int miplevel = 0 );
		unsigned int height( unsigned int miplevel = 0 );

		const unsigned char* getBytes( unsigned int miplevel = 0 );
		unsigned char* getWriteBytes( unsigned int miplevel = 0 );
};

inline unsigned char* Bitmap::getAddress( const int x, const int y, const unsigned int miplevel )
{
   return (getWriteBytes(miplevel) + ((y * width(miplevel)) + x) * getPixelBytes());
}

#endif