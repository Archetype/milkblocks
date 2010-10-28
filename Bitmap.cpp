#include "Bitmap.h"
#include "math.h"

#include "lpng/png.h"

Bitmap::Bitmap() : mByteSize( 0 ), mBytes( NULL ), mMipmaps( 0 ), mWidth( 0 ), mHeight( 0 ), mFlags( 0x00 )
{
	memset( mOffsets, 0, sizeof( mOffsets ) );
}

Bitmap::Bitmap( const Bitmap& source ) : mByteSize( 0 ), mBytes( NULL ), mMipmaps( 0 ), mWidth( 0 ), mHeight( 0 ), mFlags( 0x00 )
{
	memset( mOffsets, 0, sizeof( mOffsets ) );

	mByteSize = source.mByteSize;
	mBytes = new unsigned char[mByteSize];
	memcpy( mBytes, source.mBytes, mByteSize );

	mWidth = source.mWidth;
	mHeight = source.mHeight;
	mFlags = source.mFlags;

	mMipmaps = source.mMipmaps;
	memcpy( mOffsets, source.mOffsets, sizeof(mOffsets) );
}

Bitmap::Bitmap( unsigned int width, unsigned int height, int flags ) : mByteSize( 0 ), mBytes( NULL ), mMipmaps( 0 ), mWidth( width ), mHeight( height ), mFlags( flags )
{
	memset( mOffsets, 0, sizeof( mOffsets ) );
	allocate();
}

Bitmap::Bitmap( const char* filename, int flags ) : mByteSize( 0 ), mBytes( NULL ), mMipmaps( 0 ), mWidth( 0 ), mHeight( 0 ), mFlags( flags )
{
	memset( mOffsets, 0, sizeof( mOffsets ) );

	const char *extension = strrchr( filename, '.' );
	if(extension == NULL)
	{
		printf( "Bitmap::Bitmap - \"%s\" is not a valid filename.", filename );
		return;
	}
	extension++;

	FILE* fp;
	if( (fp = fopen( filename, "rb" )) == NULL )
	{
		printf( "Bitmap::Bitmap - could not open \"%s\".", filename );
		return;
	}

	if( strcmp( extension, "png" ) == 0 )
	{
		// load png
		readPNG( fp, filename );
		mFlags |= Texture::PNG;
	}
/*	else if( strcmp( extension, "jpg" ) == 0 )
	{
		// load jpg
		readJPG( fp, filename );
		mFlags |= Texture::JPG;
	}
	else if( strcmp( extension, "bmp" ) == 0 )
	{
		// load bmp
		readBMP( fp );
		mFlags |= Texture::BMP;
	}*/
	else
	{
		printf( "Bitmap::Bitmap - \"%s\" is not a supported texture file.", filename );
		fclose( fp );
		return;
	}
	fclose( fp );


	if( mFlags & Texture::BuildMipmaps )
		buildMipmaps();
}

Bitmap::~Bitmap()
{
	delete [] mBytes;
}

bool Bitmap::allocate()
{
	//assert( mWidth != 0 && mHeight != 0 );

	//if( mFlags & Texture::BuildMipmaps )
	//	assert( isPow2( mWidth ) && isPow2( mHeight ) );
	
	int bytesPerPixel = getPixelBytes();

	mMipmaps = 1;
	unsigned int allocPixels = mWidth * mHeight * bytesPerPixel;
	mOffsets[0] = 0;

	if( mFlags & Texture::BuildMipmaps )
	{
		unsigned int currWidth = mWidth;
		unsigned int currHeight = mHeight;
		
		do
		{
			mOffsets[mMipmaps] = mOffsets[mMipmaps-1] + (currWidth * currHeight * bytesPerPixel);
			currWidth >>= 1;
			currHeight >>= 1;
			if( currWidth == 0 ) currWidth = 1;
			if( currHeight == 0 ) currHeight = 1;

			mMipmaps++;
			allocPixels += currWidth * currHeight * bytesPerPixel;
		}while( currWidth != 1 || currHeight != 1 );
	}
	//assert( mMipmaps <= MAX_MIP_LEVELS );

	unsigned char* bytes = new unsigned char[ allocPixels ];
	memset( bytes, 0xFF, allocPixels );

	if( mBytes != NULL )
	{
		memcpy( bytes, mBytes, mByteSize );
		delete [] mBytes;
	}
	
	mBytes = bytes;
	mByteSize = allocPixels;

	return true;
}

void Bitmap::buildMipmaps()
{
	if( !(mFlags & Texture::BuildMipmaps) )
	{
		mFlags |= Texture::BuildMipmaps;
		allocate();
	}

	for( unsigned int i = 1; i < mMipmaps; i++ )
		buildMipmap( getBytes(i-1), getWriteBytes(i), height(i-1), width(i-1) );

	int bytesPerPixel = getPixelBytes();
	if( mFlags & Texture::ClearMipBorders )
	{
		for( unsigned int i = 1; i < mMipmaps; i++ )
		{
			unsigned int w = width(i);
			unsigned int h = height(i);

			if( h < 3 || w < 3 )
				memset( getWriteBytes(i),0,w*h*bytesPerPixel );
			else
			{
				w *= bytesPerPixel;
				unsigned char* bytes = getWriteBytes( i );
				unsigned char* end = bytes + (h-1)*w - bytesPerPixel;
				memset( bytes, 0, w-bytesPerPixel );
				bytes -= bytesPerPixel;
				while( bytes < end )
				{
					bytes += w;
					memset( bytes, 0, bytesPerPixel*2 );
				}
				memset(bytes+2*bytesPerPixel,0,w-bytesPerPixel);
			}
		}
	}

	// normalize pixels to save shader op
	// 2.0f * (tex2D(bump, vin.tex0).rgb - 0.5f)
	if( mFlags & Texture::BumpMap )
	{
		if( bytesPerPixel < 3 )
			printf( "Bitmap::buildMipmaps - Bitmap has less than 3 channels, cannot normalize for bump mapping." );
		else
		{
			for( unsigned int i = 1; i < mMipmaps; i++ )
			{
				for( unsigned int x = 0; x < width(i); x++ )
					for( unsigned int y = 0; y < height(i); y++ )
					{
						unsigned char* bytes = getAddress( x, y, i );
						double vx = (bytes[0]/255.0 - 0.5)*2.0;
						double vy = (bytes[1]/255.0 - 0.5)*2.0;
						double vz = (bytes[2]/255.0 - 0.5)*2.0;
						
						bytes[0] = (unsigned char)floor((((vx*0.5f)+0.5f)*255.0f)+0.5f);
						bytes[1] = (unsigned char)floor((((vy*0.5f)+0.5f)*255.0f)+0.5f);
						bytes[2] = (unsigned char)floor((((vz*0.5f)+0.5f)*255.0f)+0.5f);
					}
			}
		}
	}
}

void Bitmap::buildMipmap( const unsigned char* srcBytes, unsigned char* bytes, unsigned int srcWidth, unsigned int srcHeight )
{
	int bytesPerPixel = getPixelBytes();
	unsigned int stride = srcHeight != 1 ? srcWidth * bytesPerPixel : 0;


	unsigned int width = srcWidth >> 1;
	unsigned int height = srcHeight >> 1;
	if( width == 0 ) width = 1;
	if( height == 0 ) height = 1;

	if( srcWidth != 1 )
	{
		for( unsigned int y = 0; y < height; y++ )
		{
			for( unsigned int x = 0; x < width; x++ )
			{
				for( unsigned int p = 0; p < bytesPerPixel; p++ )
				{
					*bytes++ = (((unsigned int)*srcBytes) + ((unsigned int) srcBytes[bytesPerPixel]) + ((unsigned int)srcBytes[stride]) + ((unsigned int) srcBytes[stride+bytesPerPixel]) + 2) >> 2;
					srcBytes++;
				}
				srcBytes += bytesPerPixel;
			}
			srcBytes += stride;
		}
	}
	else
	{
		for( unsigned int y = 0; y < height; y++ )
		{
			for( unsigned int p = 0; p < bytesPerPixel; p++ )
			{
				*bytes++ = (((unsigned int) *srcBytes) + ((unsigned int)srcBytes[stride]) + 1) >> 1;
				srcBytes++;
			}
			srcBytes += bytesPerPixel+stride;
		}
	}

}

const unsigned char* Bitmap::getBytes( unsigned int miplevel )
{
	return &mBytes[mOffsets[miplevel]];
}

unsigned char* Bitmap::getWriteBytes( unsigned int miplevel )
{
	return &mBytes[mOffsets[miplevel]];
}

unsigned int Bitmap::width( unsigned int miplevel )
{
	unsigned int retVal = mWidth >> miplevel;
	return (retVal != 0) ? retVal : 1;
}

unsigned int Bitmap::height( unsigned int miplevel )
{
	unsigned int retVal = mHeight >> miplevel;
	return (retVal != 0) ? retVal : 1;
}

bool Bitmap::readPNG( FILE* fp, const char* filename )
{
	bool gotAlpha = false;
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width,height;
	int bit_depth, color_type, interlace_type;

	png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
		
	if( png_ptr == NULL )
	{
		printf( "Bitmap::readPNG - libpng failed (png_create_read_struct)." );
		return false;
	}

	info_ptr = png_create_info_struct( png_ptr );
	if( info_ptr == NULL )
	{
		png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
		printf( "Bitmap::readPNG - libpng failed (png_create_info_struct)." );
		return false;
	}

	if( setjmp( png_ptr->jmpbuf ) )
	{
		png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
		printf( "Bitmap::readPNG - \"%s\" is not a valid png file.", filename == NULL ? "???" : filename );
		return false;
	}

	png_init_io( png_ptr, fp );
	png_read_info( png_ptr, info_ptr );

	png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL );
		
	png_set_strip_16( png_ptr );

	if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
	{
		png_set_expand( png_ptr );
		gotAlpha = true;
	}

	if( color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB )
	{
		png_set_expand( png_ptr );
		if( gotAlpha )
			mFlags |= Texture::Channels4;
		else
			mFlags |= Texture::Channels3;
	}
	else if( color_type == PNG_COLOR_TYPE_GRAY ) 
	{
		png_set_expand( png_ptr );
		mFlags |= Texture::Channels1;
	} 
	else if( color_type == PNG_COLOR_TYPE_GRAY_ALPHA ) 
	{
		png_set_expand( png_ptr );
		png_set_gray_to_rgb( png_ptr ); 
		mFlags |= Texture::Channels4;
	} 
	else if( color_type == PNG_COLOR_TYPE_RGB_ALPHA ) 
	{
		png_set_expand( png_ptr );
		mFlags |= Texture::Channels4;
	}

	mWidth = width;
	mHeight = height;
	int pixelbytes = getPixelBytes();

	//mBytes = new unsigned char[height*width*pixelbytes];
	allocate();

	png_uint_32 row_stride = png_get_rowbytes(png_ptr, info_ptr);
	png_bytep* row_pointers = new png_bytep[height];

	if( row_stride != width * getPixelBytes() )
	{
		unsigned char *t = new unsigned char[height*row_stride];
		for (unsigned row = 0; row < height; row++)
			row_pointers[row] = t + (row * row_stride);

		png_read_image( png_ptr, row_pointers );
		png_read_end( png_ptr, info_ptr );

		for( unsigned int i=0; i<height; i++ )
		{
			for( unsigned int c=0; c<width; c++ )
			{
				mBytes[(i*width+c)*pixelbytes] = row_pointers[i][c*pixelbytes];
				if( pixelbytes == 1 )
					continue;
				mBytes[(i*width+c)*pixelbytes+1] = row_pointers[i][c*pixelbytes+1];
				mBytes[(i*width+c)*pixelbytes+2] = row_pointers[i][c*pixelbytes+2];
				if( pixelbytes == 3 )
					continue;
				mBytes[(i*width+c)*pixelbytes+3] = row_pointers[i][c*pixelbytes+3];
			}
		}

		delete [] t;
	}
	else
	{
		for( unsigned row = 0; row < height; row++ )
			row_pointers[row] = mBytes + (row * row_stride);

		png_read_image( png_ptr, row_pointers );
		png_read_end( png_ptr, info_ptr );
	}

	png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );

	delete [] row_pointers;

	return true;
}