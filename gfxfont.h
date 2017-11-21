// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

#include <stdint.h>

#ifndef PROGMEM
#define PROGMEM
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;



const GFXfont FreeMono12pt7b ;
const GFXfont FreeMono18pt7b ;
const GFXfont FreeMono24pt7b ;
const GFXfont FreeMono9pt7b ;
const GFXfont FreeMonoBold12pt7b ;
const GFXfont FreeMonoBold18pt7b ;
const GFXfont FreeMonoBold24pt7b ;
const GFXfont FreeMonoBold9pt7b ;
const GFXfont FreeMonoBoldOblique12pt7b ;
const GFXfont FreeMonoBoldOblique18pt7b ;
const GFXfont FreeMonoBoldOblique24pt7b ;
const GFXfont FreeMonoBoldOblique9pt7b ;
const GFXfont FreeMonoOblique12pt7b ;
const GFXfont FreeMonoOblique18pt7b ;
const GFXfont FreeMonoOblique24pt7b ;
const GFXfont FreeMonoOblique9pt7b ;
const GFXfont FreeSans12pt7b ;
const GFXfont FreeSans18pt7b ;
const GFXfont FreeSans24pt7b ;
const GFXfont FreeSans9pt7b ;
const GFXfont FreeSansBold12pt7b ;
const GFXfont FreeSansBold18pt7b ;
const GFXfont FreeSansBold24pt7b ;
const GFXfont FreeSansBold9pt7b ;
const GFXfont FreeSansBoldOblique12pt7b ;
const GFXfont FreeSansBoldOblique18pt7b ;
const GFXfont FreeSansBoldOblique24pt7b ;
const GFXfont FreeSansBoldOblique9pt7b ;
const GFXfont FreeSansOblique12pt7b ;
const GFXfont FreeSansOblique18pt7b ;
const GFXfont FreeSansOblique24pt7b ;
const GFXfont FreeSansOblique9pt7b ;
const GFXfont FreeSerif12pt7b ;
const GFXfont FreeSerif18pt7b ;
const GFXfont FreeSerif24pt7b ;
const GFXfont FreeSerif9pt7b ;
const GFXfont FreeSerifBold12pt7b ;
const GFXfont FreeSerifBold18pt7b ;
const GFXfont FreeSerifBold24pt7b ;
const GFXfont FreeSerifBold9pt7b ;
const GFXfont FreeSerifBoldItalic12pt7b ;
const GFXfont FreeSerifBoldItalic18pt7b ;
const GFXfont FreeSerifBoldItalic24pt7b ;
const GFXfont FreeSerifBoldItalic9pt7b ;
const GFXfont FreeSerifItalic12pt7b ;
const GFXfont FreeSerifItalic18pt7b ;
const GFXfont FreeSerifItalic24pt7b ;
const GFXfont FreeSerifItalic9pt7b ;
const GFXfont Org_01 ;
const GFXfont Picopixel ;
const GFXfont Tiny3x3a2pt7b ;
const GFXfont TomThumb ;


#ifdef __cplusplus
}
#endif

#endif // _GFXFONT_H_
