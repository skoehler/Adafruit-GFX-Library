/*
 This is the core graphics library for all our displays, providing a common
 set of graphics primitives (points, lines, circles, etc.).  It needs to be
 paired with a hardware-specific library for each display device we carry
 (to handle the lower-level functions).

 Adafruit invests time and resources providing this open source code, please
 support Adafruit & open-source hardware by purchasing products from Adafruit!

 Copyright (c) 2013 Adafruit Industries.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <cstring>

#include "Adafruit_GFX.h"
#include "glcdfont.h"

static inline int16_t min(int16_t a, int16_t b) {
	return a < b ? a : b;
}
static inline void _swap_int16_t(Adafruit_GFX::coord_t &a, Adafruit_GFX::coord_t &b) {
	Adafruit_GFX::coord_t t = a;
	a = b;
	b = t;
}
static inline void _sort_int16_t(Adafruit_GFX::coord_t &a, Adafruit_GFX::coord_t &b) {
	if (a > b) {
		Adafruit_GFX::coord_t t = a;
		a = b;
		b = t;
	}
}

Adafruit_GFX::Adafruit_GFX(coord_t w, coord_t h) :
		WIDTH(w), HEIGHT(h) {
	cursor_y = cursor_x = 0;
	textheight = 1;
	wrap = true;
	gfxFont = NULL;
	setRotation(0);
	setBgColor(COLOR_BLACK);
	setDrawColor(COLOR_WHITE);
	setTextColor(COLOR_WHITE);
}

Adafruit_GFX::~Adafruit_GFX() {
	// nothing
}

// Bresenham's algorithm - thx wikpedia
void Adafruit_GFX::writeLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
	coord_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	coord_t dx, dy;
	dx = x1 - x0;
	dy = std::abs(y1 - y0);

	coord_t err = dx / 2;
	coord_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			writePixel(y0, x0, color);
		} else {
			writePixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void Adafruit_GFX::writeVLine(coord_t x0, coord_t y0, coord_t y1, color_t color) {
	for (int16_t y = y0; y <= y1; y++) {
		writePixel(x0, y, color);
	}
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void Adafruit_GFX::writeHLine(coord_t x0, coord_t y0, coord_t x1, color_t color) {
	for (int16_t x = x0; x <= x1; x++) {
		writePixel(x, y0, color);
	}
}

void Adafruit_GFX::fillRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	_sort_int16_t(rx0, rx1);
	_sort_int16_t(ry0, ry1);

	for (int16_t y = ry0; y <= ry1; y++) {
		writeHLine(rx0, y, rx1, colors.draw);
	}
}

void Adafruit_GFX::clearScreen() {
	for (int y = 0; y < HEIGHT; y++) {
		writeHLine(0, y, WIDTH - 1, colors.drawbg);
	}
}

void Adafruit_GFX::drawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);
	if (rx0 == rx1) {
		_sort_int16_t(y0, y1);
		writeVLine(rx0, ry0, ry1, colors.draw);
	} else if (ry0 == ry1) {
		_sort_int16_t(x0, x1);
		writeHLine(rx0, ry0, rx1, colors.draw);
	} else {
		writeLine(rx0, ry0, rx1, ry1, colors.draw);
	}
}

// Draw a circle outline
void Adafruit_GFX::drawCircle(coord_t x0, coord_t y0, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);

	drawCircleHelper(rx0, ry0, r, 0, 0);

//    int16_t f = 1 - r;
//    int16_t ddF_x = 1;
//    int16_t ddF_y = -2 * r;
//    int16_t x = 0;
//    int16_t y = r;
//
//    writePixel(rx0  , ry0+r, color);
//    writePixel(rx0  , ry0-r, color);
//    writePixel(rx0+r, ry0  , color);
//    writePixel(rx0-r, ry0  , color);
//
//    while (x<y) {
//        if (f >= 0) {
//            y--;
//            ddF_y += 2;
//            f += ddF_y;
//        }
//        x++;
//        ddF_x += 2;
//        f += ddF_x;
//
//        writePixel(rx0 + x, ry0 + y, color);
//        writePixel(rx0 - x, ry0 + y, color);
//        writePixel(rx0 + x, ry0 - y, color);
//        writePixel(rx0 - x, ry0 - y, color);
//        writePixel(rx0 + y, ry0 + x, color);
//        writePixel(rx0 - y, ry0 + x, color);
//        writePixel(rx0 + y, ry0 - x, color);
//        writePixel(rx0 - y, ry0 - x, color);
//    }
}

void Adafruit_GFX::drawCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		writePixel(x0 + x + deltaX, y0 + y + deltaY, colors.draw);
		writePixel(x0 + y + deltaX, y0 + x + deltaY, colors.draw);
		writePixel(x0 + x + deltaX, y0 - y, colors.draw);
		writePixel(x0 + y + deltaX, y0 - x, colors.draw);
		writePixel(x0 - x, y0 + y + deltaY, colors.draw);
		writePixel(x0 - y, y0 + x + deltaY, colors.draw);
		writePixel(x0 - x, y0 - y, colors.draw);
		writePixel(x0 - y, y0 - x, colors.draw);
	}
}

void Adafruit_GFX::fillCircle(coord_t x0, coord_t y0, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);

	fillCircleHelper(x0, y0, r, 0, 0);
}

// Used to do circles and roundrects
void Adafruit_GFX::fillCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		writeHLine(x0 - x, y0 + y, x0 + x + deltaX, colors.draw);
		writeHLine(x0 - y, y0 + x, x0 + y + deltaX, colors.draw);
		writeHLine(x0 - x, y0 - y + deltaY, x0 + x + deltaX, colors.draw);
		writeHLine(x0 - y, y0 - x + deltaY, x0 + y + deltaX, colors.draw);
	}
}

// Draw a rectangle
void Adafruit_GFX::drawRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	_sort_int16_t(rx0, rx1);
	_sort_int16_t(ry0, ry1);

	writeHLine(rx0, ry0, rx1, colors.draw);
	writeHLine(rx0, ry1, rx1, colors.draw);
	writeVLine(rx0, ry0 + 1, ry1 - 1, colors.draw);
	writeVLine(rx1, ry0 + 1, ry1 - 1, colors.draw);
}

// Draw a rounded rectangle
void Adafruit_GFX::drawRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	_sort_int16_t(rx0, rx1);
	_sort_int16_t(ry0, ry1);

	// smarter version
	writeHLine(rx0 + r + 1, ry1, rx1 - r - 1, colors.draw); // Top
	writeHLine(rx0 + r + 1, ry0, rx1 - r - 1, colors.draw); // Bottom
	writeVLine(rx0, ry0 + r + 1, ry1 - r - 1, colors.draw); // Left
	writeVLine(rx1, ry0 + r + 1, ry1 - r - 1, colors.draw); // Right
	// draw four corners
	drawCircleHelper(rx0 + r, ry0 + r, r, rx1 - rx0 - 2 * r, ry1 - ry0 - 2 * r);
}

// Fill a rounded rectangle
void Adafruit_GFX::fillRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	_sort_int16_t(rx0, rx1);
	_sort_int16_t(ry0, ry1);

	for (int16_t y = ry0 + r + 1; y < ry1 - r; y++) {
		writeHLine(rx0, y, rx1, colors.draw);
	}

	// draw four corners
	fillCircleHelper(rx0 + r, ry1 - r, r, rx1 - rx0 - 2 * r, ry1 - ry0 - 2 * r);
	fillCircleHelper(rx0 + r, ry0 + r, r, rx1 - rx0 - 2 * r, ry1 - ry0 - 2 * r);
}

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void Adafruit_GFX::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h) {

	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];
			if (byte & 0x80)
				writePixel(x + i, y, colors.draw);
		}
	}
}

// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground (for set bits) and background (unset
// bits) colors.
void Adafruit_GFX::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {

	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];
			writePixel(x + i, y, (byte & 0x80) ? colors.draw : colors.drawbg);
		}
	}
}

// Draw a RAM-resident 8-bit image (grayscale) at the specified (x,y)
// pos.  Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void Adafruit_GFX::drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h) {
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			writePixel(x + i, y, bitmap[j * w + i]);
		}
	}
}

// Draw a RAM-resident 8-bit image (grayscale) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) pos.
// BOTH buffers (grayscale and mask) must be RAM-resident, no mix-and-
// match.  Specifically for 8-bit display devices such as IS31FL3731;
// no color reduction/expansion is performed.
void Adafruit_GFX::drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {
	int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
	uint8_t byte = 0;
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = mask[j * bw + i / 8];
			if (byte & 0x80) {
				writePixel(x + i, y, bitmap[j * w + i]);
			}
		}
	}
}

// Draw a RAM-resident 16-bit image (RGB 5/6/5) at the specified (x,y)
// position.  For 16-bit display devices; no color reduction performed.
void Adafruit_GFX::drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, coord_t w, coord_t h) {
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			writePixel(x + i, y, bitmap[j * w + i]);
		}
	}
}

// Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask
// (set bits = opaque, unset bits = clear) at the specified (x,y) pos.
// BOTH buffers (color and mask) must be RAM-resident, no mix-and-match.
// For 16-bit display devices; no color reduction performed.
void Adafruit_GFX::drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {
	int16_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
	uint8_t byte = 0;
	for (int16_t j = 0; j < h; j++, y++) {
		for (int16_t i = 0; i < w; i++) {
			if (i & 7)
				byte <<= 1;
			else
				byte = mask[j * bw + i / 8];
			if (byte & 0x80) {
				writePixel(x + i, y, bitmap[j * w + i]);
			}
		}
	}
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void Adafruit_GFX::drawChar(coord_t x, coord_t y, unsigned char c, coord_t size) {

	bool opaque = (colors.text != colors.textbg);

	if (!gfxFont) { // 'Classic' built-in font

		if ((x >= _width) || // Clip right
				(y >= _height) || // Clip bottom
				((x + 6 * size - 1) < 0) || // Clip left
				((y + 8 * size - 1) < 0))   // Clip top
			return;

		for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
			uint8_t line = font[c * 5 + i];
			for (int8_t j = 0; j < 8; j++, line >>= 1) {
				bool draw = line & 1;
				uint16_t c = draw ? textcolor : textbgcolor;
				if (opaque || draw) {
					if (size == 1) {
						drawPixel(x + i, y + j, c);
					} else {
						int16_t xp = x + i * size;
						int16_t yp = y + i * size;
						fillRect(xp, yp, xp + size - 1, yp + size - 1, c);
					}
				}
			}
		}
		if (opaque) { // If opaque, draw vertical line for last column
			if (size == 1) {
				drawLine(x + 5, y, x + 5, y + 8, bg);
			} else {
				fillRect(x + 5 * size, y, x + 6 * size, y + 8 * size, bg);
			}
		}

	} else { // Custom font

		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling
		// drawChar() directly with 'bad' characters of font may cause mayhem!

		c -= (uint8_t) gfxFont->first;
		GFXglyph *glyph = gfxFont->glyph + c;
		uint8_t *bitmap = gfxFont->bitmap;

		uint16_t bo = glyph->bitmapOffset;
		int16_t xs = glyph->xOffset, ys = glyph->yOffset;
		int16_t xe = xs + glyph->width, ye = ys + glyph->height;
		uint16_t xx, yy, bits = 0, bit = 0;

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		for (yy = ys; yy < ye; yy++) {
			for (xx = xs; xx < xe; xx++) {
				if (!(bit++ & 7)) {
					bits = bitmap[bo++];
				}
				if (bits & 0x80) {
					if (size == 1) {
						drawPixel(x + xx, y + yy, color);
					} else {
						fillRect(x + xx * size, y + yy * size, x + (xx + 1) * size - 1, y + (yy + 1) * size - 1, color);
					}
				}
				bits <<= 1;
			}
		}

	} // End classic vs custom font
}

void Adafruit_GFX::write(const char *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		this->Adafruit_GFX::write(data[i]);
	}
}

void Adafruit_GFX::write(char c2) {
	uint8_t c = (uint8_t) c2;
	if (!gfxFont) { // 'Classic' built-in font

		if (c == '\n') {                        // Newline?
			cursor_x = 0;                     // Reset x to zero,
			cursor_y += textheight * 8;          // advance y one line
		} else if (c == '\r') {
			cursor_x = 0;                     // Reset x to zero,
		} else {
			if (wrap && ((cursor_x + textheight * 6) > _width)) { // Off right?
				cursor_x = 0;                 // Reset x to zero,
				cursor_y += textheight * 8;      // advance y one line
			}
			drawChar(cursor_x, cursor_y, c, textheight);
			cursor_x += textheight * 6;          // Advance x one char
		}

	} else { // Custom font

		if (c == '\n') {
			cursor_x = 0;
			cursor_y += (int16_t) textheight * gfxFont->yAdvance;
		} else if (c == '\r') {
			cursor_x = 0;
		} else {
			uint8_t first = gfxFont->first;
			if ((c >= first) && (c <= gfxFont->last)) {
				GFXglyph *glyph = gfxFont->glyph + (c - first);
				uint8_t w = glyph->width, h = glyph->height;
				if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
					int16_t xo = glyph->xOffset; // sic
					if (wrap && ((cursor_x + textheight * (xo + w)) > _width)) {
						cursor_x = 0;
						cursor_y += (int16_t) textheight * gfxFont->yAdvance;
					}
					drawChar(cursor_x, cursor_y, c, textheight);
				}
				cursor_x += glyph->xAdvance * (int16_t) textheight;
			}
		}

	}
}

void Adafruit_GFX::setCursor(coord_t x, coord_t y) {
	cursor_x = x;
	cursor_y = y;
}

Adafruit_GFX::coord_t Adafruit_GFX::getCursorX(void) const {
	return cursor_x;
}

Adafruit_GFX::coord_t Adafruit_GFX::getCursorY(void) const {
	return cursor_y;
}

void Adafruit_GFX::setTextSize(coord_t s) {
	textheight = (s > 0) ? s : 1;
}

void Adafruit_GFX::setBgColor(color_t b) {
	colors.drawbg = translateColor(b);
}

void Adafruit_GFX::setDrawColor(color_t c) {
	colors.draw = translateColor(c);
}

void Adafruit_GFX::setTextColor(color_t c, color_t b) {
	colors.text = translateColor(c);
	colors.textbg = translateColor(b);
}

void Adafruit_GFX::setTextWrap(bool w) {
	wrap = w;
}

uint8_t Adafruit_GFX::getRotation(void) const {
	return rotation;
}

void Adafruit_GFX::setRotation(uint8_t x) {
	rotation = (x & 3);
	switch (rotation) {
	case 0:
	case 2:
		_width = WIDTH;
		_height = HEIGHT;
		break;
	case 1:
	case 3:
		_width = HEIGHT;
		_height = WIDTH;
		break;
	}
	switch (rotation) {
	case 0:
		mrot[0] = 1;
		mrot[1] = 0;
		mrot[2] = 0;
		mrot[3] = 1;
		vtrans[0] = 0;
		vtrans[1] = 0;
		break;
	case 2:
		mrot[0] = -1;
		mrot[1] = 0;
		mrot[2] = 0;
		mrot[3] = -1;
		vtrans[0] = WIDTH - 1;
		vtrans[1] = HEIGHT - 1;
		break;
	case 1:
		mrot[0] = 0;
		mrot[1] = -1;
		mrot[2] = 1;
		mrot[3] = 0;
		vtrans[0] = WIDTH - 1;
		vtrans[1] = 0;
		break;
	case 3:
		mrot[0] = 0;
		mrot[1] = 1;
		mrot[2] = -1;
		mrot[3] = 0;
		vtrans[0] = 0;
		vtrans[1] = HEIGHT - 1;
		break;
	}
}

void Adafruit_GFX::setFont(const GFXfont *f) {
	if (f) {            // Font struct pointer passed in?
		if (!gfxFont) { // And no current font struct?
			// Switching from classic to new font behavior.
			// Move cursor pos down 6 pixels so it's on baseline.
			cursor_y += 6;
		}
	} else if (gfxFont) { // NULL passed.  Current font struct defined?
		// Switching from new to classic font behavior.
		// Move cursor pos up 6 pixels so it's at top-left of char.
		cursor_y -= 6;
	}
	gfxFont = (GFXfont *) f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void Adafruit_GFX::charBounds(char c, coord_t *x, coord_t *y, coord_t *minx, coord_t *miny, coord_t *maxx, coord_t *maxy) {

	if (gfxFont) {

		if (c == '\n') { // Newline?
			*x = 0;    // Reset x to zero, advance y by one line
			*y += textheight * gfxFont->yAdvance;
		} else if (c != '\r') { // Not a carriage return; is normal char
			uint8_t first = gfxFont->first, last = gfxFont->last;
			if ((c >= first) && (c <= last)) { // Char present in this font?
				GFXglyph *glyph = gfxFont->glyph + (c - first);
				uint8_t gw = glyph->width, gh = glyph->height, xa = glyph->xAdvance;
				int8_t xo = glyph->xOffset, yo = glyph->yOffset;
				if (wrap && ((*x + (((int16_t) xo + gw) * textheight)) > _width)) {
					*x = 0; // Reset x to zero, advance y by one line
					*y += textheight * gfxFont->yAdvance;
				}
				int16_t ts = (int16_t) textheight, x1 = *x + xo * ts, y1 = *y + yo * ts, x2 = x1 + gw * ts - 1, y2 = y1 + gh * ts - 1;
				if (x1 < *minx)
					*minx = x1;
				if (y1 < *miny)
					*miny = y1;
				if (x2 > *maxx)
					*maxx = x2;
				if (y2 > *maxy)
					*maxy = y2;
				*x += xa * ts;
			}
		}

	} else { // Default font

		if (c == '\n') {                     // Newline?
			*x = 0;                        // Reset x to zero,
			*y += textheight * 8;             // advance y one line
			// min/max x/y unchaged -- that waits for next 'normal' character
		} else if (c != '\r') {  // Normal char; ignore carriage returns
			if (wrap && ((*x + textheight * 6) > _width)) { // Off right?
				*x = 0;                    // Reset x to zero,
				*y += textheight * 8;         // advance y one line
			}
			int x2 = *x + textheight * 6 - 1, // Lower-right pixel of char
			y2 = *y + textheight * 8 - 1;
			if (x2 > *maxx)
				*maxx = x2;      // Track max x, y
			if (y2 > *maxy)
				*maxy = y2;
			if (*x < *minx)
				*minx = *x;      // Track min x, y
			if (*y < *miny)
				*miny = *y;
			*x += textheight * 6;             // Advance x one char
		}
	}
}

// Pass string and a cursor position, returns UL corner and W,H.
void Adafruit_GFX::getTextBounds(char *str, coord_t x, coord_t y, coord_t *x1, coord_t *y1, coord_t *w, coord_t *h) {
	uint8_t c; // Current character

	*x1 = x;
	*y1 = y;
	*w = *h = 0;

	coord_t minx = _width, miny = _height, maxx = -1, maxy = -1;

	while ((c = *str++))
		charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

	if (maxx >= minx) {
		*x1 = minx;
		*w = maxx - minx + 1;
	}
	if (maxy >= miny) {
		*y1 = miny;
		*h = maxy - miny + 1;
	}
}

// Return the size of the display (per current rotation)
Adafruit_GFX::coord_t Adafruit_GFX::getWidth(void) const {
	return _width;
}

Adafruit_GFX::coord_t Adafruit_GFX::getHeight(void) const {
	return _height;
}

/***************************************************************************/
// code for the GFX button UI element
Adafruit_GFX_Button::Adafruit_GFX_Button(void) {
	_gfx = 0;
}

// Classic initButton() function: pass center & size
void Adafruit_GFX_Button::initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor,
		char *label, uint8_t textsize) {
	// Tweak arguments and pass to the newer initButtonUL() function...
	initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void Adafruit_GFX_Button::initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor,
		char *label, uint8_t textsize) {
	_x1 = x1;
	_y1 = y1;
	_w = w;
	_h = h;
	_outlinecolor = outline;
	_fillcolor = fill;
	_textcolor = textcolor;
	_textsize = textsize;
	_gfx = gfx;
	strncpy(_label, label, 9);
}

void Adafruit_GFX_Button::drawButton(bool inverted) {
	uint16_t fill, outline, text;

	if (!inverted) {
		fill = _fillcolor;
		outline = _outlinecolor;
		text = _textcolor;
	} else {
		fill = _textcolor;
		outline = _outlinecolor;
		text = _fillcolor;
	}

	uint8_t r = min(_w, _h) / 4; // Corner radius
	_gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
	_gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

	_gfx->setCursor(_x1 + (_w / 2) - (strlen(_label) * 3 * _textsize), _y1 + (_h / 2) - (4 * _textsize));
	_gfx->setTextColor(text, text);
	_gfx->setTextSize(_textsize);
	_gfx->print(_label);
}

bool Adafruit_GFX_Button::contains(int16_t x, int16_t y) {
	return ((x >= _x1) && (x < (_x1 + _w)) && (y >= _y1) && (y < (_y1 + _h)));
}

void Adafruit_GFX_Button::press(bool p) {
	laststate = currstate;
	currstate = p;
}

bool Adafruit_GFX_Button::isPressed() {
	return currstate;
}
bool Adafruit_GFX_Button::justPressed() {
	return (currstate && !laststate);
}
bool Adafruit_GFX_Button::justReleased() {
	return (!currstate && laststate);
}

// -------------------------------------------------------------------------

// GFXcanvas1, GFXcanvas8 and GFXcanvas16 (currently a WIP, don't get too
// comfy with the implementation) provide 1-, 8- and 16-bit offscreen
// canvases, the address of which can be passed to drawBitmap() or
// pushColors() (the latter appears only in a couple of GFX-subclassed TFT
// libraries at this time).  This is here mostly to help with the recently-
// added proportionally-spaced fonts; adds a way to refresh a section of the
// screen without a massive flickering clear-and-redraw...but maybe you'll
// find other uses too.  VERY RAM-intensive, since the buffer is in MCU
// memory and not the display driver...GXFcanvas1 might be minimally useful
// on an Uno-class board, but this and the others are much more likely to
// require at least a Mega or various recent ARM-type boards (recommended,
// as the text+bitmap draw can be pokey).  GFXcanvas1 requires 1 bit per
// pixel (rounded up to nearest byte per scanline), GFXcanvas8 is 1 byte
// per pixel (no scanline pad), and GFXcanvas16 uses 2 bytes per pixel (no
// scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.

GFXcanvas1::GFXcanvas1(uint16_t w, uint16_t h) :
		Adafruit_GFX(w, h) {
	uint16_t bytes = ((w + 7) / 8) * h;
	if ((buffer = (uint8_t *) malloc(bytes))) {
		memset(buffer, 0, bytes);
	}
}

GFXcanvas1::~GFXcanvas1(void) {
	if (buffer)
		free(buffer);
}

uint8_t* GFXcanvas1::getBuffer(void) {
	return buffer;
}

void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
			return;

		int16_t t;
		switch (rotation) {
		case 1:
			t = x;
			x = WIDTH - 1 - y;
			y = t;
			break;
		case 2:
			x = WIDTH - 1 - x;
			y = HEIGHT - 1 - y;
			break;
		case 3:
			t = x;
			x = y;
			y = HEIGHT - 1 - t;
			break;
		}

		uint8_t *ptr = buffer + (x / 8) + y * ((WIDTH + 7) / 8);
		if (color)
			*ptr |= 0x80 >> (x & 7);
		else
			*ptr &= ~(0x80 >> (x & 7));
	}
}

void GFXcanvas1::fillScreen(uint16_t color) {
	if (buffer) {
		uint16_t bytes = ((WIDTH + 7) / 8) * HEIGHT;
		memset(buffer, color ? 0xFF : 0x00, bytes);
	}
}

GFXcanvas8::GFXcanvas8(uint16_t w, uint16_t h) :
		Adafruit_GFX(w, h) {
	uint32_t bytes = w * h;
	if ((buffer = (uint8_t *) malloc(bytes))) {
		memset(buffer, 0, bytes);
	}
}

GFXcanvas8::~GFXcanvas8(void) {
	if (buffer)
		free(buffer);
}

uint8_t* GFXcanvas8::getBuffer(void) {
	return buffer;
}

void GFXcanvas8::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
			return;

		int16_t t;
		switch (rotation) {
		case 1:
			t = x;
			x = WIDTH - 1 - y;
			y = t;
			break;
		case 2:
			x = WIDTH - 1 - x;
			y = HEIGHT - 1 - y;
			break;
		case 3:
			t = x;
			x = y;
			y = HEIGHT - 1 - t;
			break;
		}

		buffer[x + y * WIDTH] = color;
	}
}

void GFXcanvas8::fillScreen(uint16_t color) {
	if (buffer) {
		memset(buffer, color, WIDTH * HEIGHT);
	}
}

void GFXcanvas8::writeHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

	if ((x >= _width) || (y < 0) || (y >= _height))
		return;
	int16_t x2 = x + w - 1;
	if (x2 < 0)
		return;

	// Clip left/right
	if (x < 0) {
		x = 0;
		w = x2 + 1;
	}
	if (x2 >= _width)
		w = _width - x;

	int16_t t;
	switch (rotation) {
	case 1:
		t = x;
		x = WIDTH - 1 - y;
		y = t;
		break;
	case 2:
		x = WIDTH - 1 - x;
		y = HEIGHT - 1 - y;
		break;
	case 3:
		t = x;
		x = y;
		y = HEIGHT - 1 - t;
		break;
	}

	memset(buffer + y * WIDTH + x, color, w);
}

GFXcanvas16::GFXcanvas16(uint16_t w, uint16_t h) :
		Adafruit_GFX(w, h) {
	uint32_t bytes = w * h * 2;
	if ((buffer = (uint16_t *) malloc(bytes))) {
		memset(buffer, 0, bytes);
	}
}

GFXcanvas16::~GFXcanvas16(void) {
	if (buffer)
		free(buffer);
}

uint16_t* GFXcanvas16::getBuffer(void) {
	return buffer;
}

void GFXcanvas16::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
			return;

		int16_t t;
		switch (rotation) {
		case 1:
			t = x;
			x = WIDTH - 1 - y;
			y = t;
			break;
		case 2:
			x = WIDTH - 1 - x;
			y = HEIGHT - 1 - y;
			break;
		case 3:
			t = x;
			x = y;
			y = HEIGHT - 1 - t;
			break;
		}

		buffer[x + y * WIDTH] = color;
	}
}

void GFXcanvas16::fillScreen(uint16_t color) {
	if (buffer) {
		uint8_t hi = color >> 8, lo = color & 0xFF;
		if (hi == lo) {
			memset(buffer, lo, WIDTH * HEIGHT * 2);
		} else {
			uint32_t i, pixels = WIDTH * HEIGHT;
			for (i = 0; i < pixels; i++)
				buffer[i] = color;
		}
	}
}

