#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "Print.h"
#include "gfxfont.h"

class Adafruit_GFX: public Print {
public:
	typedef int32_t coord_t;
	typedef uint32_t color_t;

	static const color_t COLOR_BLACK = 0x000000;
	static const color_t COLOR_WHITE = 0xFFFFFF;
	static const color_t COLOR_RED   = 0xFF0000;
	static const color_t COLOR_GREEN = 0x00FF00;
	static const color_t COLOR_BLUE  = 0x0000FF;

private:
	void drawChar(coord_t x, coord_t y, unsigned char c, coord_t size);

	coord_t dirX(coord_t x, coord_t y) {
		return mrot[0] * x + mrot[1] * y;
	}
	coord_t dirY(coord_t x, coord_t y) {
		return mrot[2] * x + mrot[3] * y;
	}
	coord_t realX(coord_t x, coord_t y) {
		return dirX(x, y) + vtrans[0];
	}
	coord_t realY(coord_t x, coord_t y) {
		return dirY(x, y) + vtrans[1];
	}

protected:
	virtual void write(char);
	virtual void write(const char *, size_t);
	void charBounds(char c, coord_t *x, coord_t *y, coord_t *minx, coord_t *miny, coord_t *maxx, coord_t *maxy);

	// TRANSACTION API / CORE DRAW API
	// Colors are translated, coordinates are unroated.
	// This MUST be defined by the subclass:
	virtual color_t translateColor(color_t color) = 0;
	virtual void writePixel(coord_t x, coord_t y, color_t color) = 0;
	// These MAY be overridden by the subclass to provide device-specific
	// optimized code.  Otherwise 'generic' versions are used.
	virtual void writeHLine(coord_t x0, coord_t y0, coord_t x1, color_t color);
	virtual void writeVLine(coord_t x0, coord_t y0, coord_t y1, color_t color);
	virtual void writeLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

	void drawCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY);
	void fillCircleHelper(coord_t x0, coord_t y0, coord_t r, coord_t deltaX, coord_t deltaY);

public:
	Adafruit_GFX(coord_t w, coord_t h); // Constructor
	virtual ~Adafruit_GFX();

	void setRotation(uint8_t r);
	void setCursor(coord_t x, coord_t y);
	void setBgColor(color_t bg);
	void setDrawColor(color_t c);
	void setTextColor(color_t c, color_t bg);
	void setTextSize(coord_t s);
	void setTextWrap(bool w);
	void setFont(const GFXfont *f = NULL);

	void setTextColor(color_t c) {
		setTextColor(c, c);
	}

	uint8_t getRotation(void) const;
	coord_t getHeight(void) const;
	coord_t getWidth(void) const;
	coord_t getCursorX(void) const;
	coord_t getCursorY(void) const;
	coord_t getTextSize() const;
	void getTextBounds(char *string, coord_t x, coord_t y, coord_t *x0, coord_t *y0, coord_t *w, coord_t *h);

	// These exist only with Adafruit_GFX (no subclass overrides)
	void clearScreen();
	void drawPixel(coord_t x, coord_t y);
	void drawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1);
	void drawRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1);
	void fillRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1);
	void drawCircle(coord_t x0, coord_t y0, coord_t r);
	void fillCircle(coord_t x0, coord_t y0, coord_t r);
	void drawRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t radius);
	void fillRoundRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t radius);
	void drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h);
	void drawBitmap(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h);
	void drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, coord_t w, coord_t h);
	void drawGrayscaleImage(coord_t x, coord_t y, const uint8_t *bitmap, const uint8_t *mask, coord_t w, coord_t h);
	void drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, coord_t w, coord_t h);
	void drawRGBImage(coord_t x, coord_t y, const uint32_t *bitmap, const uint8_t *mask, coord_t w, coord_t h);

protected:
	GFXfont *gfxFont;
	const coord_t WIDTH, HEIGHT; // This is the 'raw' display w/h - never changes
	uint8_t rotation;
	int8_t mrot[4];
	coord_t vtrans[2];
	coord_t _width;
	coord_t _height;

	struct colors_t {
		color_t draw;
		color_t drawbg;
		color_t text;
		color_t textbg;
	};

	colors_t colors;
	std::vector<colors_t> colorstack;

	coord_t cursor_x;
	coord_t cursor_y;
	coord_t textheight;
	bool wrap;
};

class Adafruit_GFX_Button {

public:
	Adafruit_GFX_Button(void);
	// "Classic" initButton() uses center & size
	void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label,
			uint8_t textsize);
	// New/alt initButton() uses upper-left corner & size
	void initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label,
			uint8_t textsize);
	void drawButton(bool inverted = false);
	bool contains(int16_t x, int16_t y);

	void press(bool p);
	bool isPressed();
	bool justPressed();
	bool justReleased();

private:
	Adafruit_GFX *_gfx;
	int16_t _x1, _y1; // Coordinates of top-left corner
	uint16_t _w, _h;
	uint8_t _textsize;
	uint16_t _outlinecolor, _fillcolor, _textcolor;
	char _label[10];

	bool currstate, laststate;
};

class GFXcanvas1: public Adafruit_GFX {
public:
	GFXcanvas1(uint16_t w, uint16_t h);
	~GFXcanvas1(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color);
	uint8_t *getBuffer(void);
private:
	uint8_t *buffer;
};

class GFXcanvas8: public Adafruit_GFX {
public:
	GFXcanvas8(uint16_t w, uint16_t h);
	~GFXcanvas8(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color), writeHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

	uint8_t *getBuffer(void);
private:
	uint8_t *buffer;
};

class GFXcanvas16: public Adafruit_GFX {
public:
	GFXcanvas16(uint16_t w, uint16_t h);
	~GFXcanvas16(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color), fillScreen(uint16_t color);
	uint16_t *getBuffer(void);
private:
	uint16_t *buffer;
};

#endif // _ADAFRUIT_GFX_H
