#include "font.h"

FT_Library Font::smFT_library = NULL;

FontData::FontData( const char* _f, unsigned int _p, unsigned char _fl )
{
	face = _f;
	pixel = _p;
	flags = _fl & Font::FontFlags;
}

Font::Font( const FontData& l ) : mFontFace( l.face ), mPixelSize( l.pixel ), mGlyphWidths( NULL ), mGL_DisplayList( 0 ), mGL_TexID( 0 )
{
	mFlags = (l.flags & FontFlags);
	load();
}

Font::~Font()
{
	unload( false );
}

void Font::initialize()
{
	if( FT_Init_FreeType( &smFT_library ) )
	{
		//DispatchError("Could not initialize FreeType library.");
	}
}

void Font::shutdown()
{
	if( FT_Done_FreeType( smFT_library ) )
	{
		//DispatchError("Could not close FreeType library.");
	}
}


bool Font::load()
{
	if( smFT_library == NULL )
	{
		//DispatchError("Font::load - FreeType library has not been initialized.");
		return false;
	}

	if( mFontFace == NULL )
	{
		//DispatchError("Font::load - No font face specified.");
		return false;
	}

	if( mFlags & Loaded )
	{
		//DispatchWarning("Font::load - Font with pixel size \"%i\" and font face \"%s\" has already been loaded.", mPixelSize, mFontFaceName);
		return true;
	}

	FT_Error error = FT_New_Face( smFT_library,
						   mFontFace,
						   0,
						   &mFT_face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		printf("Font::load - Font face \"%s\" is an unrecognized format.", mFontFace);
		return false;
	}
	else if ( error )
	{
		printf("Font::load - Font face \"%s\" is either missing or damaged.", mFontFace);
		return false;
	}

	error = FT_Set_Pixel_Sizes(
            mFT_face,		/* handle to face object */
            0,				/* pixel_width           */
            mPixelSize );	/* pixel_height          */
	

	if ( error )
	{
		printf("Font::load - Could not set pixel size \"%i\" for font face \"%s\".", mPixelSize, mFontFace);
		return false;
	}

	mFT_face->glyph->format = ft_glyph_format_bitmap;
	FT_Pos highest = 0;
	FT_Pos width = 0;
	mGlyphWidths = new FT_Pos[mFT_face->num_glyphs];

	int i;
	for( i=0; i<mFT_face->num_glyphs; i++ )
	{
		error = FT_Load_Glyph(
            mFT_face,			/* handle to face object */
            i,					/* glyph index           */
            FT_LOAD_DEFAULT );	/* load flags, see below */

		if ( error )
		{
			printf("Font::load - could not load glyph %i for font face \"%s\".", i, mFontFace);
			return false;
		}

		mGlyphWidths[i] = mFT_face->glyph->advance.x >> 6;

		if(mFT_face->glyph->metrics.height >> 6 > highest)
			highest = mFT_face->glyph->metrics.height >> 6;
		width += mFT_face->glyph->metrics.width >> 6;
	}

	FT_Pos area = highest * width;
	int img_width;
	for( img_width=0; (1<<img_width)*(1<<img_width) < area; img_width++ ) {}

	unsigned char *texture_data=NULL;
	for( ;img_width<MAX_TEXTURE_SIZE; )
	{
		int img_row = 0;
		int img_col = 0;
		FT_Pos w;
		for( i=0; i<mFT_face->num_glyphs; i++ )
		{
			error = FT_Load_Glyph(
				mFT_face,			/* handle to face object */
				i,					/* glyph index           */
				FT_LOAD_DEFAULT );	/* load flags, see below */
			
			if ( error )
			{
				printf("Font::load - could not load glyph %i for font face \"%s\".", i, mFontFace);
				return false;
			}

			w = mFT_face->glyph->metrics.width >> 6;
			if( img_col + w < 1<<img_width )
				img_col += w;
			else
			{
				img_col = w;
				img_row += highest;
			}
		}
		if( img_row+highest < 1<<img_width )
		{
			texture_data = new unsigned char[(1<<img_width)*(1<<img_width)<<1];
			for( int i=0; i<(1<<img_width)*(1<<img_width); i++ )
				texture_data[i]=0;
			break;
		}
		else
			img_width++; 
	}

	if( texture_data == NULL )
	{
		printf("Font::load - font \"%s\" with pixel size %i could not fit in a %i square bitmap.", mFontFace, mPixelSize, (1<<(img_width-1)));
		return false;
	}

	int img_row = 0;
	int img_col = 0;
	FT_Pos w;
	FT_Pos h;
	FT_Pos x, y;
	float tex_map_ratio = 1.0f/(1<<img_width);
	float tex_map_r, tex_map_c, tex_map_h, tex_map_w;

	mGL_DisplayList = glGenLists( mFT_face->num_glyphs );
	
	if( glGetError() != GL_NO_ERROR )
	{
		delete [] texture_data;
		return false;
	}

	for( i=0; i<mFT_face->num_glyphs; i++ )
	{
		error = FT_Load_Glyph(
			mFT_face,			/* handle to face object */
			i,					/* glyph index           */
			FT_LOAD_DEFAULT );  /* load flags, see below */
		
		if ( error )
		{
			printf("Font::load - could not load glyph %i for font face \"%s\".", i, mFontFace);
			delete [] texture_data;
			return false;
		}

		error = FT_Render_Glyph( mFT_face->glyph, mFlags & AntiAlias ? FT_RENDER_MODE_LIGHT : FT_RENDER_MODE_MONO );
		
		if ( error )
		{
			printf("Font::load - could not rasterize glyph %i for font face \"%s\".", i, mFontFace);
			delete [] texture_data;
			return false;
		}

		w = mFT_face->glyph->metrics.width >> 6;
		h = mFT_face->glyph->metrics.height >> 6;

		if(img_col + w >= 1<<img_width)
		{
			img_col = 0;
			img_row += highest;
		}

		int idx;
		FT_Bitmap *bitmap = &mFT_face->glyph->bitmap;
		for( y = 0; y < h; y++ )
		{
			for( x = 0; x < w; x++ )
			{
				if( mFlags & AntiAlias )
				{
					//idx = (((img_row)+y)*(1<<img_width)+(img_col)+x)<<1;
					idx = (((img_row)+(h-y-1))*(1<<img_width)+(img_col)+x)<<1;
					texture_data[idx] = bitmap->buffer[(y)*w+x];
					texture_data[idx+1] = texture_data[idx];
				}
				else
				{
					//idx = (((img_row)+y)*(1<<img_width)+(img_col)+x)<<1;
					idx = (((img_row)+(h-y-1))*(1<<img_width)+(img_col)+x)<<1;
					texture_data[idx] = (bitmap->buffer[y * bitmap->pitch + x / 8] & (1 << (7 - x % 8))) ? 255 : 0;
					texture_data[idx+1] = texture_data[idx];
				}
			}
		}

		tex_map_r = img_row*tex_map_ratio;
		tex_map_c = img_col*tex_map_ratio;
		tex_map_h = (h*tex_map_ratio);
		tex_map_w = (w*tex_map_ratio);

		glNewList( mGL_DisplayList+i, GL_COMPILE );

			glPushMatrix( );

			
			glTranslatef((float)(mFT_face->glyph->metrics.horiBearingX >> 6), (float)-((mFT_face->glyph->metrics.horiBearingY) >> 6), 0.0f);

			glBegin( GL_TRIANGLE_STRIP );
				glTexCoord2f( tex_map_c, tex_map_r );
				glVertex2i( 0, h );

				glTexCoord2f( tex_map_c, tex_map_r+tex_map_h );
				glVertex2i( 0, 0 );

				glTexCoord2f( tex_map_c+tex_map_w, tex_map_r );
				glVertex2i( w, h );

				glTexCoord2f( tex_map_c+tex_map_w, tex_map_r+tex_map_h );
				glVertex2i( w, 0 );
			glEnd( );

			glPopMatrix( );

			glTranslatef( (float)(mFT_face->glyph->advance.x >> 6), (float)(mFT_face->glyph->advance.y >> 6), 0.0f );

		glEndList( );

		img_col += w;
	}

	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &mGL_TexID );
	glBindTexture( GL_TEXTURE_2D, mGL_TexID );

	if( glGetError() != GL_NO_ERROR )
	{
		delete [] texture_data;
		return false;
	}

	if( mFlags & Is3D )
	{
		// 3d font enable filtering and mipmaps ...

		// -- TODO: SET PREFERRED FILTERING TYPE: linear, trilinear, anisotropic ... --
		// right now we're always using trilinear
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

		gluBuild2DMipmaps( GL_TEXTURE_2D, 2, 1<<(img_width), 1<<(img_width),
						GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texture_data );

	}
	else
	{
		// 2d font, no filtering ...
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

		glTexImage2D( GL_TEXTURE_2D, 0, 2, 1<<(img_width), 1<<(img_width), 0, 
						GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texture_data );
	}

	glDisable( GL_TEXTURE_2D );
	delete [] texture_data;

	mFlags |= Loaded;
	return true;
}

void Font::unload( bool lazarus )
{
	delete [] mGlyphWidths;
	mGlyphWidths = NULL;
	if( mFlags & Loaded )
	{
		glDeleteTextures ( 1, &mGL_TexID );
		glDeleteLists( mGL_DisplayList, mFT_face->num_glyphs );
		mGL_TexID = 0;
		mFlags &= ~Loaded;
	}
}

bool Font::hasGlyph( const char c )
{
	return FT_Get_Char_Index( mFT_face, c ) != 0;
}

void Font::render( const char *text )
{
	//assert( mFlags & Loaded );

	glBindTexture( GL_TEXTURE_2D, mGL_TexID );
	glTexEnvf( GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE );

	const char *c = text;
	unsigned int idx;

	glPushMatrix();
	while(*c != '\0')
	{
		if(*c == '\t')
			glTranslatef( 10.0f, 0.0f, 0.0f );
		idx = FT_Get_Char_Index( mFT_face, *c );

		if(idx > 0)
			glCallList( mGL_DisplayList+idx );

		c++;
	}
	glPopMatrix();
}

void Font::renderCJ( const char *text, int area )
{
	glPushMatrix();

	// center :
	glTranslatef( (GLfloat)((area >> 1) - (getWidth(text) >> 1)), 0.0f, 0.0f );
	render( text );

	glPopMatrix();
}

void Font::renderRJ( const char *text, int area )
{
	glPushMatrix();

	// right justified :
	glTranslatef( (GLfloat)((area) - (getWidth(text))), 0.0f, 0.0f );
	render( text );

	glPopMatrix();
}

int Font::render( const char *text, int n )
{
	//assert( mFlags & Loaded );

	glBindTexture( GL_TEXTURE_2D, mGL_TexID );
	glTexEnvf( GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE );

	const char *c = text;
	unsigned int idx;
	int i = 0, width = 0;

	glPushMatrix();
	while( *c != '\0' && i < n )
	{
		idx = FT_Get_Char_Index( mFT_face, *c );

		if(idx > 0)
		{
			glCallList( mGL_DisplayList+idx );
			width += mGlyphWidths[idx];
		}

		c++; i++;
	}
	glPopMatrix();

	return width;
}

int Font::renderCJ( const char *text, int n, int area )
{
	glPushMatrix();

	// center :
	glTranslatef( (GLfloat)((area >> 1) - (getWidth(text) >> 1)), 0.0f, 0.0f );
	int width = render( text, n );

	glPopMatrix();

	return width;
}

int Font::renderRJ( const char *text, int n, int area )
{
	glPushMatrix();

	// right justified :
	glTranslatef( (GLfloat)((area) - (getWidth(text))), 0.0f, 0.0f );
	int width = render( text, n );

	glPopMatrix();

	return width;
}

int Font::render( char c )
{
	//assert( mFlags & Loaded );

	glBindTexture( GL_TEXTURE_2D, mGL_TexID );
	glTexEnvf( GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE );

	glPushMatrix();
	unsigned int idx = FT_Get_Char_Index( mFT_face, c );
	if(idx > 0)
		glCallList( mGL_DisplayList+idx );
	glPopMatrix();

	return mGlyphWidths[idx];
}

int Font::renderCJ(char c, int area)
{
	glPushMatrix();

	// center :
	glTranslatef( (GLfloat)((area >> 1) - (getWidth(c) >> 1)), 0.0f, 0.0f );
	int w = render(c);

	glPopMatrix();

	return w;
}

int Font::renderRJ( char c, int area )
{
	glPushMatrix();

	// right justified :
	glTranslatef( (GLfloat)((area) - (getWidth(c))), 0.0f, 0.0f);
	int w = render(c);

	glPopMatrix();

	return w;
}

int Font::getWidth( char c )
{
	//assert( mFlags & Loaded );

	if(c == '\t')
		return 10;
	return mGlyphWidths[FT_Get_Char_Index( mFT_face, c )];
}

unsigned int Font::closest( const char *text, int w )
{
	//assert( mFlags & Loaded );

	if(w <= 0)
		return 0;

	const char *c = text;
	int width=0;

	unsigned int i = 0;

	while(*c != '\0')
	{
		if(*c == '\t')
			width += 10;
		else
			width += mGlyphWidths[FT_Get_Char_Index( mFT_face, *c )];

		if(width >= w)
			break;

		c++; i++;
	}

	return i;
}

int Font::getWidth( const char *text, int chars )
{
	//assert( mFlags & Loaded );

	const char *c = text;
	int i=0;
	unsigned int width=0;

	while(*c != '\0' && i < chars)
	{
		if(*c == '\t')
			width += 10;
		else
			width += mGlyphWidths[FT_Get_Char_Index( mFT_face, *c )];
		c++; i++;
	}

	return width;
}

int Font::getWidth( const char *text )
{
	//assert( mFlags & Loaded );

	const char *c = text;
	unsigned int width=0;

	while(*c != '\0')
	{
		if(*c == '\t')
			width += 10;
		else
			width += mGlyphWidths[FT_Get_Char_Index( mFT_face, *c )];
		c++;
	}

	return width;
}