#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#include <cstdint>

#include "Print.h"
#include "gfxfont.h"

namespace GFX {

typedef int32_t coord_t;
typedef uint32_t color_t;

static const color_t COLOR_BLACK = 0x000000;
static const color_t COLOR_GRAY1 = 0x111111;
static const color_t COLOR_GRAY2 = 0x222222;
static const color_t COLOR_GRAY3 = 0x333333;
static const color_t COLOR_GRAY4 = 0x444444;
static const color_t COLOR_GRAY5 = 0x555555;
static const color_t COLOR_GRAY6 = 0x666666;
static const color_t COLOR_GRAY7 = 0x777777;
static const color_t COLOR_GRAY8 = 0x888888;
static const color_t COLOR_GRAY9 = 0x999999;
static const color_t COLOR_GRAY10 = 0xAAAAAA;
static const color_t COLOR_GRAY11 = 0xBBBBBB;
static const color_t COLOR_GRAY12 = 0xCCCCCC;
static const color_t COLOR_GRAY13 = 0xDDDDDD;
static const color_t COLOR_GRAY14 = 0xEEEEEE;
static const color_t COLOR_WHITE = 0xFFFFFF;
static const color_t COLOR_RED   = 0xFF0000;
static const color_t COLOR_GREEN = 0x00FF00;
static const color_t COLOR_BLUE  = 0x0000FF;

class Canvas: public Print {
private:
	GFXfont *gfxFont;
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

	coord_t cursor_x;
	coord_t cursor_y;
	coord_t textheight;
	bool wrap;

	void drawChar(coord_t x, coord_t y, unsigned char c, coord_t size);
	void drawGlyph(coord_t x, coord_t y, GFXglyph *glyph, coord_t size);

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
	const coord_t WIDTH, HEIGHT; // This is the 'raw' display w/h - never changes

	virtual void write(char);
	virtual void write(const char *, size_t);
	void charBounds(char c, coord_t *x, coord_t *y, coord_t *minx, coord_t *miny, coord_t *maxx, coord_t *maxy);

	void initColors();

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
	class ColorSafe {
	private:
		colors_t colors;
		Canvas &ref;
	public:
		ColorSafe(Canvas &ref) : colors(ref.colors), ref(ref) {
			//nothing
		}
		~ColorSafe() {
			ref.colors = colors;
		}
	};


	Canvas(coord_t w, coord_t h); // Constructor
	virtual ~Canvas();

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
	void drawTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
	void fillTriangle(coord_t x0, coord_t y0, coord_t x1, coord_t y1, coord_t x2, coord_t y2);
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

	virtual void flush();
};

class Button {

public:
	Button(void);
	virtual ~Button();

	// "Classic" initButton() uses center & size
	void initButton(Canvas *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
			uint16_t outline, uint16_t fill, uint16_t textcolor, std::string label,
			uint8_t textsize);
	// New/alt initButton() uses upper-left corner & size
	void initButtonUL(Canvas *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h,
			uint16_t outline, uint16_t fill, uint16_t textcolor, std::string label,
			uint8_t textsize);
	void drawButton(bool inverted = false);
	bool contains(int16_t x, int16_t y);

	void press(bool p);
	bool isPressed();
	bool justPressed();
	bool justReleased();

private:
	Canvas *_gfx;
	int16_t _x1, _y1; // Coordinates of top-left corner
	uint16_t _w, _h;
	uint8_t _textsize;
	uint16_t _outlinecolor, _fillcolor, _textcolor;
	std::string _label;

	bool currstate, laststate;
};

class Canvas1bpp: public Canvas {
public:
	Canvas1bpp(uint16_t w, uint16_t h);
	~Canvas1bpp(void);
	uint8_t *getBuffer(void);
protected:
	virtual color_t translateColor(color_t color);
	virtual void writePixel(coord_t x, coord_t y, color_t color);
private:
	uint8_t *buffer;
};

class Canvas4bpp: public Canvas {
public:
	Canvas4bpp(uint16_t w, uint16_t h);
	~Canvas4bpp(void);
	uint8_t *getBuffer(void);
protected:
	virtual color_t translateColor(color_t color);
	virtual void writePixel(coord_t x, coord_t y, color_t color);
	virtual void writeHLine(coord_t x0, coord_t y0, coord_t x1, color_t color);
	virtual void writeVLine(coord_t x0, coord_t y0, coord_t y1, color_t color);
private:
	uint8_t *buffer;
};

class Canvas8bpp: public Canvas {
public:
	Canvas8bpp(uint16_t w, uint16_t h);
	~Canvas8bpp(void);
	uint8_t *getBuffer(void);
protected:
	virtual color_t translateColor(color_t color);
	virtual void writePixel(coord_t x, coord_t y, color_t color);
private:
	uint8_t *buffer;
};

class Canvas16bpp: public Canvas {
public:
	Canvas16bpp(uint16_t w, uint16_t h);
	~Canvas16bpp(void);
	uint16_t *getBuffer(void);
protected:
	virtual color_t translateColor(color_t color);
	virtual void writePixel(coord_t x, coord_t y, color_t color);
private:
	uint16_t *buffer;
};


}

#endif // _ADAFRUIT_GFX_H
