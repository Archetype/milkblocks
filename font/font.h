#ifndef _FONT_H_
#define _FONT_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef __FT_HEADER_H__
#include "freetype/config/ftheader.h"
#include FT_FREETYPE_H
#endif

#include "freetype/freetype.h"


struct FontData
{
	const char* face;
	unsigned int pixel;
	unsigned char flags;

	FontData( const char* _f, unsigned int _p, unsigned char _fl );
};

class Font
{
	enum
	{
		MAX_TEXTURE_SIZE = 11										// 2^11 == 1024
	};

	static FT_Library	smFT_library;								// FreeType library

	FT_Pos*				mGlyphWidths;								// FreeType glpyh widths
	FT_Face				mFT_face;									// FreeType font face
	GLuint				mGL_DisplayList;							// OpenGL display list base id
	GLuint				mGL_TexID;									// OpenGL texture object id

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

			AntiAlias = FreeFlag,
			Is3D = FreeFlag << 1,

			FontFlags = AntiAlias|Is3D
		};

		const char *mFontFace;										// The font face file
		unsigned int mPixelSize;									// The pixel height

		static void initialize();
		static void shutdown();

		Font( const FontData& l );
		~Font();

		inline bool operator==(const FontData& _test) const
		{
			return strcmp( mFontFace, _test.face ) == 0 && mPixelSize == _test.pixel && (mFlags & FontFlags) == _test.flags;
		}


		bool load();
		void unload( bool lazarus );

		bool hasGlyph( const char c );

		unsigned int closest(const char *text, int w);				// get closest index to the desired width
		void render(const char *text);								// render left justified string
		void renderCJ(const char *text, int area);			// render center justified string
		void renderRJ(const char *text, int area);			// render right justified string

		int render(const char *text, int n);						// render limited left justified string
		int renderCJ(const char *text, int n, int area);	// render limited center justified string
		int renderRJ(const char *text, int n, int area);	// render limited right justified string

		int render(char c);											// render left justified character
		int renderCJ(char c, int area);					// render center justified character
		int renderRJ(char c, int area);					// render right justified character

		int getWidth(char c);
		int getWidth(const char *text);
		int getWidth(const char *text, int chars);


		static char *formatDigits (unsigned long integer)
		{
			static char fdigits[10 + (10 / 3) + 1];
			char digits[10 + 1];
			char *dgt = digits, *fdgt = fdigits;
			int places;

			places = sprintf(digits, "%lu", integer); // convert integer to string 
			// places = digits in string 
			while (*fdgt++ = *dgt++) // while there are more digits
			if (--places) // if places-1 > 0
			if ( !(places % 3) ) // if places is multiple of 3
			*fdgt++ = ','; // insert a comma here
			return fdigits;
		}
};
#endif