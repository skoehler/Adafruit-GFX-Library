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

#include "Canvas.h"

#include <cstdlib>
#include <cstring>

using namespace GFX;


static const unsigned char glcdfont[] = {
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x22, 0x54, 0x54, 0x78, 0x42, // a-umlaut
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0x7D, 0x12, 0x11, 0x12, 0x7D, // A-umlaut
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x3A, 0x44, 0x44, 0x44, 0x3A, // o-umlaut
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x3D, 0x42, 0x42, 0x42, 0x3D, // O-umlaut
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0x55, 0x00, 0x55, 0x00, 0x55, // #176 (25% block) missing in old code
	0xAA, 0x55, 0xAA, 0x55, 0xAA, // 50% block
	0xFF, 0x55, 0xFF, 0x55, 0xFF, // 75% block
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0xFC, 0x4A, 0x4A, 0x4A, 0x34, // sharp-s or beta
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00  // #255 NBSP
};


static inline int16_t min(int16_t a, int16_t b) {
	return a < b ? a : b;
}

static inline void swapCoords(coord_t &a, coord_t &b) {
	coord_t t = a;
	a = b;
	b = t;
}
static inline void sortCoords(coord_t &a, coord_t &b) {
	if (a > b) {
		coord_t t = a;
		a = b;
		b = t;
	}
}

Canvas::Canvas(coord_t w, coord_t h) :
		WIDTH(w), HEIGHT(h) {
	cursor_y = cursor_x = 0;
	textheight = 1;
	wrap = false;
	gfxFont = NULL;
	setRotation(0);
}

void Canvas::initColors() {
	setBgColor(COLOR_BLACK);
	setDrawColor(COLOR_WHITE);
	setTextColor(COLOR_WHITE);
}

Canvas::~Canvas() {
	// nothing
}

// Bresenham's algorithm - thx wikpedia
void Canvas::writeLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
	coord_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swapCoords(x0, y0);
		swapCoords(x1, y1);
	}

	if (x0 > x1) {
		swapCoords(x0, x1);
		swapCoords(y0, y1);
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
void Canvas::writeVLine(coord_t x0, coord_t y0, coord_t y1, color_t color) {
	for (int16_t y = y0; y <= y1; y++) {
		writePixel(x0, y, color);
	}
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void Canvas::writeHLine(coord_t x0, coord_t y0, coord_t x1, color_t color) {
	for (int16_t x = x0; x <= x1; x++) {
		writePixel(x, y0, color);
	}
}

void Canvas::fillRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	sortCoords(rx0, rx1);
	sortCoords(ry0, ry1);

	for (int16_t y = ry0; y <= ry1; y++) {
		writeHLine(rx0, y, rx1, colors.draw);
	}
}

void Canvas::clearScreen() {
	for (int y = 0; y < HEIGHT; y++) {
		writeHLine(0, y, WIDTH - 1, colors.drawbg);
	}
}

void Canvas::drawPixel(coord_t x, coord_t y) {
	coord_t rx = realX(x, y);
	coord_t ry = realY(x, y);

	writePixel(rx, ry, colors.draw);
}

void Canvas::drawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	if (rx0 == rx1) {
		sortCoords(y0, y1);
		writeVLine(rx0, ry0, ry1, colors.draw);
	} else if (ry0 == ry1) {
		sortCoords(x0, x1);
		writeHLine(rx0, ry0, rx1, colors.draw);
	} else {
		writeLine(rx0, ry0, rx1, ry1, colors.draw);
	}
}

void Canvas::drawTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2) {
    drawLine(x0, y0, x1, y1);
    drawLine(x0, y0, x2, y2);
    drawLine(x1, y1, x2, y2);
}

// Fill a triangle
void Canvas::fillTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2) {

	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);
	coord_t rx2 = realX(x2, y2);
	coord_t ry2 = realY(x2, y2);

	//coord_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (ry0 > ry1) {
    	swapCoords(ry0, ry1); swapCoords(rx0, rx1);
    }
    if (ry1 > ry2) {
    	swapCoords(ry2, ry1); swapCoords(rx2, rx1);
    }
    if (ry0 > ry1) {
    	swapCoords(ry0, ry1); swapCoords(rx0, rx1);
    }

    if(ry0 == ry2) { // Handle awkward all-on-same-line case as its own thing
        coord_t a = rx0;
        coord_t b = rx0;
        if(rx1 < a)
        	a = rx1;
        else if(rx1 > b)
        	b = rx1;
        if(rx2 < a)
        	a = rx2;
        else if(rx2 > b)
        	b = rx2;
        writeHLine(a, ry0, b, colors.draw);
        return;
    }

    coord_t dx01 = rx1 - rx0;
    coord_t dy01 = ry1 - ry0;
	coord_t dx02 = rx2 - rx0;
	coord_t dy02 = ry2 - ry0;
	coord_t dx12 = rx2 - rx1;
	coord_t dy12 = ry2 - ry1;
	coord_t sa   = 0;
	coord_t sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
	coord_t last;
    if(ry1 == ry2)
    	last = ry1;   // Include y1 scanline
    else
    	last = ry1-1; // Skip it

    coord_t y=ry0;
    for(; y<=last; y++) {
        coord_t a   = rx0 + sa / dy01;
        coord_t b   = rx0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
        	swapCoords(a, b);
        writeHLine(a, y, b, colors.draw);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - ry1);
    sb = dx02 * (y - ry0);
    for(; y<=ry2; y++) {
    	coord_t a   = rx1 + sa / dy12;
    	coord_t b   = rx0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
        	swapCoords(a, b);
        writeHLine(a, y, b, colors.draw);
    }
}

// Draw a circle outline
void Canvas::drawCircle(coord_t x0, coord_t y0, coord_t r) {
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

void Canvas::drawCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	writePixel(x0 + y + deltaX, y0 + x + deltaY, colors.draw);
	writePixel(x0 + x + deltaX, y0 - y, colors.draw);
	writePixel(x0 - x, y0 + y + deltaY, colors.draw);
	writePixel(x0 - y, y0 - x, colors.draw);

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

void Canvas::fillCircle(coord_t x0, coord_t y0, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);

	writeHLine(rx0 - r, ry0, rx0 + r, colors.draw);
	fillCircleHelper(rx0, ry0, r, 0, 0);
}

// Used to do circles and roundrects
void Canvas::fillCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY) {

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

		writeHLine(x0 - x, y0 + y + deltaY, x0 + x + deltaX, colors.draw);
		writeHLine(x0 - y, y0 + x + deltaY, x0 + y + deltaX, colors.draw);
		writeHLine(x0 - x, y0 - y, x0 + x + deltaX, colors.draw);
		writeHLine(x0 - y, y0 - x, x0 + y + deltaX, colors.draw);
	}
}

// Draw a rectangle
void Canvas::drawRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	sortCoords(rx0, rx1);
	sortCoords(ry0, ry1);

	writeHLine(rx0, ry0, rx1, colors.draw);
	writeHLine(rx0, ry1, rx1, colors.draw);
	writeVLine(rx0, ry0 + 1, ry1 - 1, colors.draw);
	writeVLine(rx1, ry0 + 1, ry1 - 1, colors.draw);
}

// Draw a rounded rectangle
void Canvas::drawRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	sortCoords(rx0, rx1);
	sortCoords(ry0, ry1);

	// smarter version
	writeHLine(rx0 + r, ry0, rx1 - r - 1, colors.draw); // Top
	writeHLine(rx0 + r + 1, ry1, rx1 - r, colors.draw); // Bottom
	writeVLine(rx0, ry0 + r + 1, ry1 - r, colors.draw); // Left
	writeVLine(rx1, ry0 + r, ry1 - r - 1, colors.draw); // Right

	// draw four corners
	drawCircleHelper(rx0 + r, ry0 + r, r, rx1 - rx0 - 2 * r, ry1 - ry0 - 2 * r);
}

// Fill a rounded rectangle
void Canvas::fillRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t r) {
	coord_t rx0 = realX(x0, y0);
	coord_t ry0 = realY(x0, y0);
	coord_t rx1 = realX(x1, y1);
	coord_t ry1 = realY(x1, y1);

	sortCoords(rx0, rx1);
	sortCoords(ry0, ry1);

	for (int16_t y = ry0 + r; y <= ry1 - r; y++) {
		writeHLine(rx0, y, rx1, colors.draw);
	}

	// draw four corners
	fillCircleHelper(rx0 + r, ry0 + r, r, rx1 - rx0 - 2 * r, ry1 - ry0 - 2 * r);
}

// BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

// Draw a RAM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void Canvas::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h) {
	//TODO draw all pixels
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
void Canvas::drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {
	//TODO use mask
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
void Canvas::drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h) {
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
void Canvas::drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {
	//TODO draw all pixels
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
void Canvas::drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, coord_t w, coord_t h) {
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
void Canvas::drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, const uint8_t *mask, coord_t w, coord_t h) {
	//TODO use mask
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
void Canvas::drawChar(coord_t x, coord_t y, unsigned char c, coord_t size) {
	ColorSafe tmp(*this);

	bool opaque = (colors.text != colors.textbg);

	if ((x >= _width) || // Clip right
			(y >= _height) || // Clip bottom
			((x + 6 * size - 1) < 0) || // Clip left
			((y + 8 * size - 1) < 0))   // Clip top
		return;

	for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
		uint8_t line = glcdfont[c * 5 + i];
		for (int8_t j = 0; j < 8; j++, line >>= 1) {
			bool draw = line & 1;
			colors.draw = draw ? colors.text : colors.textbg;
			if (opaque || draw) {
				if (size == 1) {
					drawPixel(x + i, y + j);
				} else {
					int16_t xp = x + i * size;
					int16_t yp = y + i * size;
					fillRect(xp, yp, xp + size - 1, yp + size - 1);
				}
			}
		}
	}
	if (opaque) { // If opaque, draw vertical line for last column
		colors.draw = colors.textbg;
		if (size == 1) {
			drawLine(x + 5, y, x + 6 - 1, y + 8 - 1);
		} else {
			fillRect(x + 5 * size, y, x + 6 * size - 1, y + 8 * size - 1);
		}
	}
}

void Canvas::drawGlyph(coord_t x, coord_t y, GFXglyph *glyph, coord_t size) {
	ColorSafe tmp(*this);

	// Character is assumed previously filtered by write() to eliminate
	// newlines, returns, non-printable characters, etc.  Calling
	// drawChar() directly with 'bad' characters of font may cause mayhem!

	uint32_t bo = glyph->bitmapOffset;
	coord_t xs = glyph->xOffset, ys = glyph->yOffset;
	coord_t xe = xs + glyph->width, ye = ys + glyph->height;
	uint8_t bits = 0, bit = 0;

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

	colors.draw = colors.text;

	printf("drawing %d\n", glyph->bitmapOffset);

	uint8_t *bitmap = gfxFont->bitmap;
	for (coord_t yy = ys; yy < ye; yy++) {
		for (coord_t xx = xs; xx < xe; xx++) {
			if (!(bit++ & 7)) {
				bits = bitmap[bo++];
			}
			if (bits & 0x80) {
				if (size == 1) {
					drawPixel(x + xx, y + yy);
				} else {
					fillRect(x + xx * size, y + yy * size, x + (xx + 1) * size - 1, y + (yy + 1) * size - 1);
				}
			}
			bits <<= 1;
		}
	}
}

void Canvas::write(const char *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		this->Canvas::write(data[i]);
	}
}

void Canvas::write(char c2) {
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
				GFXglyph *glyph = gfxFont->glyph + c - first;
				uint8_t w = glyph->width, h = glyph->height;
				if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
					int16_t xo = glyph->xOffset; // sic
					if (wrap && ((cursor_x + textheight * (xo + w)) > _width)) {
						cursor_x = 0;
						cursor_y += (int16_t) textheight * gfxFont->yAdvance;
					}
					drawGlyph(cursor_x, cursor_y, glyph, textheight);
				}
				cursor_x += glyph->xAdvance * (int16_t) textheight;
			}
		}

	}
}

void Canvas::setCursor(coord_t x, coord_t y) {
	cursor_x = x;
	cursor_y = y;
}

coord_t Canvas::getCursorX(void) const {
	return cursor_x;
}

coord_t Canvas::getCursorY(void) const {
	return cursor_y;
}

void Canvas::setTextSize(coord_t s) {
	textheight = (s > 0) ? s : 1;
}

void Canvas::setBgColor(color_t b) {
	colors.drawbg = translateColor(b);
}

void Canvas::setDrawColor(color_t c) {
	colors.draw = translateColor(c);
}

void Canvas::setTextColor(color_t c, color_t b) {
	colors.text = translateColor(c);
	colors.textbg = translateColor(b);
}

void Canvas::setTextWrap(bool w) {
	wrap = w;
}

uint8_t Canvas::getRotation(void) const {
	return rotation;
}

void Canvas::setRotation(uint8_t x) {
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

void Canvas::setFont(const GFXfont *f) {
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
void Canvas::charBounds(char c, coord_t *x, coord_t *y, coord_t *minx, coord_t *miny, coord_t *maxx, coord_t *maxy) {

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
void Canvas::getTextBounds(char *str, coord_t x, coord_t y, coord_t *x1, coord_t *y1, coord_t *w, coord_t *h) {
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
coord_t Canvas::getWidth(void) const {
	return _width;
}

coord_t Canvas::getHeight(void) const {
	return _height;
}

void Canvas::flush() {
	//nothing
}

/***************************************************************************/
// code for the GFX button UI element
Button::Button(void) {
	_gfx = 0;
}

Button::~Button() {
	//nothing
}

// Classic initButton() function: pass center & size
void Button::initButton(Canvas *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor,
		std::string label, uint8_t textsize) {
	// Tweak arguments and pass to the newer initButtonUL() function...
	initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void Button::initButtonUL(Canvas *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor,
		std::string label, uint8_t textsize) {
	_x1 = x1;
	_y1 = y1;
	_w = w;
	_h = h;
	_outlinecolor = outline;
	_fillcolor = fill;
	_textcolor = textcolor;
	_textsize = textsize;
	_gfx = gfx;
	_label = label;
}

void Button::drawButton(bool inverted) {
	Canvas::ColorSafe tmp(*_gfx);
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
	_gfx->setDrawColor(fill);
	_gfx->fillRoundRect(_x1, _y1, _w, _h, r);
	_gfx->setDrawColor(outline);
	_gfx->drawRoundRect(_x1, _y1, _w, _h, r);

	_gfx->setCursor(_x1 + (_w / 2) - (_label.length() * 3 * _textsize), _y1 + (_h / 2) - (4 * _textsize));
	_gfx->setTextColor(text, text);
	_gfx->setTextSize(_textsize);
	_gfx->print(_label);
}

bool Button::contains(int16_t x, int16_t y) {
	return ((x >= _x1) && (x < (_x1 + _w)) && (y >= _y1) && (y < (_y1 + _h)));
}

void Button::press(bool p) {
	laststate = currstate;
	currstate = p;
}

bool Button::isPressed() {
	return currstate;
}
bool Button::justPressed() {
	return (currstate && !laststate);
}
bool Button::justReleased() {
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

Canvas1bpp::Canvas1bpp(uint16_t w, uint16_t h) :
		Canvas(w, h) {
	size_t linelength = (WIDTH + 7) / 8;
	uint16_t bytes = linelength * h;
	buffer = new uint8_t[bytes];
	initColors();
}

Canvas1bpp::~Canvas1bpp(void) {
	delete buffer;
}

uint8_t* Canvas1bpp::getBuffer(void) {
	return buffer;
}

color_t Canvas1bpp::translateColor(color_t color) {
	color_t x = (color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF);
	return x < 3 * 128 ? 0 : 1;
}

void Canvas1bpp::writePixel(coord_t x, coord_t y, color_t color) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;

	size_t linelength = (WIDTH + 7) / 8;
	uint8_t *ptr = buffer + (x / 8) + y * linelength;
	uint8_t off = x & 7;
	*ptr = (*ptr & (0x7F7F >> off)) | ((color & 1) << (7 - off));
}

Canvas4bpp::Canvas4bpp(uint16_t w, uint16_t h) :
		Canvas(w, h) {
	size_t linelength = (WIDTH + 1) / 2;
	uint16_t bytes = linelength * h;
	buffer = new uint8_t[bytes];
	initColors();
}

Canvas4bpp::~Canvas4bpp(void) {
	delete buffer;
}

uint8_t* Canvas4bpp::getBuffer(void) {
	return buffer;
}

color_t Canvas4bpp::translateColor(color_t color) {
	color_t x = (color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF);
	return x / 3;
}

void Canvas4bpp::writePixel(coord_t x, coord_t y, color_t color) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;

	size_t linelength = (WIDTH + 1) / 2;
	uint8_t *ptr = buffer + (x / 2) + y * linelength;
	uint8_t shift = (x & 1) << 2;
	*ptr = (*ptr & (0xF0F >> shift)) | ((color & 0xF) << (4 - shift));
}

void Canvas4bpp::writeHLine(coord_t x0, coord_t y0, coord_t x1, color_t color) {
	if (y0 < 0 || y0 >= HEIGHT)
		return;
	if (x0 < 0)
		x0 = 0;
	if (x1 >= WIDTH)
		x1 = WIDTH-1;

	uint8_t shift0 = (x0 & 1) << 2;
	uint8_t shift1 = (x1 & 1) << 2;
	uint8_t amask0 = (0xF00 >> shift0);
	uint8_t amask1 = (0x00F >> shift1);

	uint8_t omask = (color & 0xF) | ((color & 0xF) << 4);

	size_t linelength = (WIDTH + 1) / 2;
	uint8_t *ptr0 = buffer + (x0 / 2) + y0 * linelength;
	uint8_t *ptr1 = buffer + (x1 / 2) + y0 * linelength;

	if (ptr0 == ptr1) {
		uint8_t amask = amask0 | amask1;
		*ptr0 = (*ptr0 & amask) | (omask & ~amask);
	} else if (ptr0 < ptr1) {
		*ptr0 = (*ptr0 & amask0) | (omask & ~amask0);
		for (ptr0++; ptr0 < ptr1; ptr0++) {
			*ptr0 = omask;
		}
		*ptr1 = (*ptr1 & amask1) | (omask & ~amask1);
	}
}

void Canvas4bpp::writeVLine(coord_t x0, coord_t y0, coord_t y1, color_t color) {
	if (x0 < 0 || x0 >= WIDTH)
		return;
	if (y0 < 0)
		x0 = 0;
	if (y1 >= HEIGHT)
		y1 = HEIGHT-1;

	uint8_t shift = (x0 & 1) << 2;
	uint8_t amask = (0xF0F >> shift);
	uint8_t omask = (color & 0xF) << (4 - shift);

	size_t linelength = (WIDTH + 1) / 2;
	uint8_t *ptr1 = buffer + (x0 / 2) + y1 * linelength;
	for (uint8_t *ptr0 = buffer + (x0 / 2) + y0 * linelength; ptr0 <= ptr1; ptr0 += linelength) {
		*ptr0 = (*ptr0 & amask) | omask;
	}
}


Canvas8bpp::Canvas8bpp(uint16_t w, uint16_t h) :
		Canvas(w, h) {
	uint32_t bytes = w * h;
	buffer = new uint8_t[bytes];
	initColors();
}

Canvas8bpp::~Canvas8bpp(void) {
	delete buffer;
}

uint8_t* Canvas8bpp::getBuffer(void) {
	return buffer;
}

color_t Canvas8bpp::translateColor(color_t color) {
	color_t x = (color & 0xFF) + ((color >> 8) & 0xFF) + ((color >> 16) & 0xFF);
	return x / 3;
}

void Canvas8bpp::writePixel(coord_t x, coord_t y, color_t color) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;

	buffer[x + y * WIDTH] = color;
}

Canvas16bpp::Canvas16bpp(uint16_t w, uint16_t h) :
		Canvas(w, h) {
	uint32_t bytes = w * h;
	buffer = new uint16_t[bytes];
	initColors();
}

Canvas16bpp::~Canvas16bpp(void) {
	delete buffer;
}

uint16_t* Canvas16bpp::getBuffer(void) {
	return buffer;
}

color_t Canvas16bpp::translateColor(color_t color) {
	return ((color & 0xF8) >> 3)
			| ((color & 0xFC00) >> 5)
			| ((color & 0xF80000) >> 8);
}

void Canvas16bpp::writePixel(coord_t x, coord_t y, color_t color) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return;

	buffer[x + y * WIDTH] = color;
}
