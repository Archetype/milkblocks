#include "texture.h"
#include "Bitmap.h"

#ifndef GL_CLAMP_TO_EDGE_EXT
#define GL_CLAMP_TO_EDGE_EXT 0x812F
#endif

TextureData::TextureData( const char *f, unsigned int fl ) : filename( f ), flags( fl&Texture::_EXTERNAL_BITS ), bitmap( NULL )
{
}


TextureData::TextureData( Bitmap* bmp, unsigned int fl ) : filename( NULL ), flags( fl&Texture::_EXTERNAL_BITS ), bitmap( bmp )
{
}


Texture::Texture( const TextureData& td ) : mHeight( 0 ), mWidth( 0 ), mGL_TexID( 0 ), mFileName( NULL ), mBitmap( NULL )
{
	if( td.bitmap != NULL )
		load( td.bitmap, td.flags );
	else
		load( td.filename, td.flags );
}


void Texture::sendToGPU() 
{ 
	mFlags |= SendToGPU; 
	processFlags(); 
}

Texture::~Texture()
{
	unload( false );
	
	delete [] mFileName;
}

void Texture::processFlags()
{
	if( mFlags & SendToGPU )
	{
		glEnable( GL_TEXTURE_2D );

		glGenTextures( 1, &mGL_TexID );
		
		glBindTexture( GL_TEXTURE_2D, mGL_TexID );

		GLenum format;
		if( mBitmap->flags() & Channels1 )
			format = GL_LUMINANCE;
		else if( mBitmap->flags() & Channels2 )
			format = GL_LUMINANCE_ALPHA;
		else if( mBitmap->flags() & Channels3 )
			format = GL_RGB;
		else if( mBitmap->flags() & Channels4 )
			format = GL_RGBA;

		int bytesPerPixel = getPixelBytes();
		for ( unsigned int i = 0; i < mBitmap->mipmaps(); i++ )
		{
			glTexImage2D(GL_TEXTURE_2D,
						i,
						bytesPerPixel,
						mBitmap->width(i), mBitmap->height(i),
						0,
						format,
						GL_UNSIGNED_BYTE,
						mBitmap->getBytes(i));
		}

		// set wrap mode
		if( mFlags & Wrap )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT );
		}

		resetFiltering();

		glDisable( GL_TEXTURE_2D );

		mFlags |= SentGPU;
	}
}

bool Texture::load()
{
	if( mBitmap != NULL )
	{
		processFlags();
		return true;
	}
	else
		return load( mFileName, mFlags );
}

bool Texture::load( Bitmap* bmp, unsigned int flags )
{
	mFlags = (flags & (~_INTERNAL_BITS));

	// set anisotropic filtering only if we've got it ... 
	if(/*doesn't support anisotropic filtering*/ false )
		mFlags = (flags & (~Anisotropic));

	//if( flags & BuildMipmaps ) 
	//	bmp->
	mBitmap = bmp;
	mFlags = mBitmap->flags()|flags;
	mWidth = mBitmap->width();
	mHeight = mBitmap->height();
	processFlags();

	if( !(mFlags & KeepLocal) )
	{
		delete mBitmap;
		mBitmap = NULL;
	}

	mFlags |= Loaded;
	return true;
}

bool Texture::load( const char *filename, unsigned int flags )
{
	const char *extension = strrchr( filename, '.' );
	if(extension == NULL)
	{
		printf( "Texture::load - \"%s\" is not a valid filename." );
		return false;
	}

	delete mBitmap;
	//delete [] mBytes;
	//mBytes = NULL;

	// check if we need to save the name (not needed on texture resurrect)
	if( mFileName != filename )
	{
		mFileName = new char[strlen( filename )+1];
		strcpy( mFileName, filename );
	}

	// set flags, clean out internally managed bits ... 
	mFlags = (flags & (~_INTERNAL_BITS));

	// set anisotropic filtering only if we've got it ... 
	if(/*doesn't support anisotropic filtering*/ false )
		mFlags = (flags & (~Anisotropic));

	mBitmap = new Bitmap( filename, flags );
	mFlags = mBitmap->flags();
	mWidth = mBitmap->width();
	mHeight = mBitmap->height();
	/*
	extension++;

	bool loaded = false;

	FILE* fp;
	if( (fp = fopen( mFileName, "rb" )) == NULL )
	{
		printf( "Texture::load - could not open \"%s\".", mFileName );
		return false;
	}

	if( strcmp( extension, "png" ) == 0 )
	{
		// load png
		loaded = load_png( fp );
		mFlags |= PNG;
	}
	else if( strcmp( extension, "jpg" ) == 0 )
	{
		// load jpg
		loaded = load_jpg( fp );
		mFlags |= JPG;
	}
	else if( strcmp( extension, "bmp" ) == 0 )
	{
		// load bmp
		loaded = load_bmp( fp );
		mFlags |= BMP;
	}
	else
	{
		printf( "Texture::load - \"%s\" is not a supported texture file.", mFileName );
		fclose( fp );
		return false;
	}

	// allocate alpha channel if requested ...
	if( mFlags & RequestAlpha )
	{
		int dbytes = getPixelBytes();

		// allocate alpha if we don't have one
		if( mFlags & Channels1 || mFlags & Channels3 )
		{
			int new_stride = (dbytes+1);
			unsigned char* tbytes = new unsigned char[mHeight * mWidth * new_stride];
			for(int i=0; i<mHeight*mWidth; i++)
			{
				int d;
				for( d=0; d < dbytes; d++ )
					tbytes[i*new_stride+d] = mBytes[i*dbytes+d];
				tbytes[i*new_stride+d] = 255;
			}
			delete [] mBytes;
			mBytes = tbytes;

			// reset depth
			mFlags &= (~ChannelBits);

			if( dbytes == 1 )
				mFlags |= Channels2;
			else
				mFlags |= Channels4;
		}
		// if we didn't already have an alpha emit an error for an unknown/unsupported bitdepth
		else if(!(mFlags & Channels2 || mFlags & Channels4))
		{
			printf( "Texture::load - cannot allocate alpha channel on a \"%i\" bit texture.", dbytes<<3 );
		}
	}

	fclose( fp );
	*/
	//if( loaded )
	{
		processFlags();

		if( !(mFlags & KeepLocal) )
		{
			delete mBitmap;
			mBitmap = NULL;
		}
	}

//	if( loaded )
		mFlags |= Loaded;

	return true;
}

void Texture::update( )
{
	if( mBitmap == NULL ) return;
	glEnable( GL_TEXTURE_2D );
			
	glBindTexture( GL_TEXTURE_2D, mGL_TexID );

	mFlags = mFlags|mBitmap->flags();
	mWidth = mBitmap->width();
	mHeight = mBitmap->height();

	GLenum format;
	if( mBitmap->flags() & Channels1 )
		format = GL_LUMINANCE;
	else if( mBitmap->flags() & Channels2 )
		format = GL_LUMINANCE_ALPHA;
	else if( mBitmap->flags() & Channels3 )
		format = GL_RGB;
	else if( mBitmap->flags() & Channels4 )
		format = GL_RGBA;


	// int bytesPerPixel = getPixelBytes();
	for ( unsigned int i = 0; i < mBitmap->mipmaps(); i++ )
	{
		glTexSubImage2D(GL_TEXTURE_2D,
					i,
					0,
					0,
					mBitmap->width(i), mBitmap->height(i),
					format,
					GL_UNSIGNED_BYTE,
					mBitmap->getBytes(i));
	}
/*
	if( mFlags & Channels1 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, mBitmap->getBytes() );

	else if( mFlags & Channels2 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, mBitmap->getBytes() );

	else if( mFlags & Channels3 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, mBitmap->getBytes() );

	else if( mFlags & Channels4 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, mBitmap->getBytes() );
*/
	glDisable( GL_TEXTURE_2D );
}

void Texture::update( unsigned char *data, int step, unsigned short width, unsigned short height )
{
	glEnable( GL_TEXTURE_2D );
			
	glBindTexture( GL_TEXTURE_2D, mGL_TexID );

	if( mFlags & Channels1 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, data );

	else if( mFlags & Channels2 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data );

	else if( mFlags & Channels3 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, data );

	else if( mFlags & Channels4 )
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data );

	glDisable( GL_TEXTURE_2D );
}

void Texture::unload( bool lazarus )
{
	if( mGL_TexID != 0 )
	{
		glDeleteTextures( 1, &mGL_TexID );
		mGL_TexID = 0;
	}
	
	if( !lazarus )
	{
		delete mBitmap;
		mBitmap = NULL;
	}
}

void Texture::resetAllFiltering()
{
//	for( Texture *t = Resources::Textures.getHead(); t != NULL; t = static_cast<Texture*>(t->getNext()) )
//		t->resetFiltering();
}

void Texture::resetFiltering()
{
	glEnable( GL_TEXTURE_2D );
			
	glBindTexture( GL_TEXTURE_2D, mGL_TexID );

	unsigned int prefFlags = 0;
	unsigned int flags = (mFlags & FilteringBits) ? mFlags : mFlags | prefFlags;

	if( flags & Linear )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

	// bilinear and trilinear filtering require mipmaps (mipmapping only set engine side)

	else if( flags & Bilinear && (flags & BuildMipMaps) )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	}
	else if( flags & Trilinear && (flags & BuildMipMaps) )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}

	// no setting or no mipmaps

	else
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	glDisable( GL_TEXTURE_2D );
}

void Texture::setAlpha( int x, int y, unsigned char c )
{
	if( mBitmap == NULL || !mBitmap->hasAlpha() )
		return;

	if( mBitmap->flags() & Channels2 )
		mBitmap->getWriteBytes()[(((y*mBitmap->width()) + x)<<1)+1] = c;
	else // 4 bytes per pixel
		mBitmap->getWriteBytes()[(((y*mBitmap->width()) + x)<<2)+3] = c;
}

void Texture::setGrayscale( int x, int y, unsigned char c )
{
	if( mBitmap == NULL )
		return;

	int b		= mBitmap->getPixelBytes();
	int i		= ((y*mBitmap->width()) + x)*b;
	if( b > 3 )
		b = 3;
	for( int j=0; j < b; j++ )
		mBitmap->getWriteBytes()[i+j] = c;
}

const unsigned char* Texture::getBytes()
{
	return mBitmap == NULL ? NULL : mBitmap->getBytes();
}

unsigned char* Texture::getWriteBytes()
{
	return mBitmap == NULL ? NULL : mBitmap->getWriteBytes();
}

unsigned char Texture::getAlpha( int x, int y )
{
	if( mBitmap == NULL || !mBitmap->hasAlpha() )
		return 255;

	if( mBitmap->flags() & Channels2 )
		return mBitmap->getWriteBytes()[(((y*mBitmap->width()) + x)<<1)+1];
	else // 4 bytes per pixel
		return mBitmap->getWriteBytes()[(((y*mBitmap->width()) + x)<<2)+3];
}

unsigned char Texture::getGrayscale( int x, int y )
{
	if( mBitmap == NULL )
		return 255;

	int b		= mBitmap->getPixelBytes();
	int i		= ((y*mBitmap->width()) + x)*b;
	int gs		= 0;
	if( b > 3 )
		b = 3;
	for( int j = 0; j < b; j++ )
		gs += mBitmap->getWriteBytes()[i+j];
	
	return (unsigned char)(gs/b);
}