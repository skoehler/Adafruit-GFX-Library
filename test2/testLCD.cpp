#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../OLEDDisplay.h"

using std::chrono::steady_clock;
using namespace GFX;

void testBasicCommon(Canvas &lcd, color_t bg, color_t fg) {
	lcd.setBgColor(bg);
	lcd.setDrawColor(fg);
	lcd.clearScreen();

	lcd.drawRect(10, 40, 30, 50);
	lcd.drawCircle(20, 20, 10);
	lcd.fillCircle(50, 20, 10);
	lcd.fillRect(40, 40, 60, 50);

	lcd.drawLine(10, 10, 56, 56);

	lcd.setCursor(10, 60);
	lcd.setTextColor(COLOR_WHITE);
	lcd.print("Hello World!");

	lcd.setCursor(15, 65);
	lcd.setTextColor(COLOR_WHITE, COLOR_BLACK);
	lcd.print("Hello World!");

	lcd.setCursor(20, 70);
	lcd.setTextColor(COLOR_WHITE);
	lcd.print("Hello World!");

	lcd.flush();
	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void testBasic1() {
	OLEDDisplay lcd(128, 128);
	lcd.enable();
	testBasicCommon(lcd, COLOR_WHITE, COLOR_BLACK);
}

void testBasic2() {
	OLEDDisplay lcd(128, 128);
	lcd.enable();
	testBasicCommon(lcd, COLOR_BLACK, COLOR_WHITE);
}

void testFPS() {
	OLEDDisplay lcd(128, 128);
	lcd.enable();

	lcd.setBgColor(COLOR_BLACK);
	lcd.setDrawColor(COLOR_WHITE);

	steady_clock::time_point lastTime = steady_clock::now();
	uint32_t lastFC = lcd.getFrameCounter();

	const int radius=10;
	const int minX = radius;
	const int maxX = lcd.getWidth() - radius;
	const int maxY = lcd.getHeight() - radius;

	float posx = 30;
	float posy = 30;
	float speedx = 0.5;
	float speedy = 0;
	float accely = 0.11;

	while (true) {
		posx += speedx;
		posy += speedy;
		speedy += accely;

		if (posx < minX) {
			posx = 2*minX - posx;
			speedx = -speedx;
		}
		if (posx > maxX) {
			posx = 2*maxX - posx;
			speedx = -speedx;
		}
		if (posy > maxY) {
			//posy = 2*maxY - posy;
			//speedx = 1.0 * speedx;
			posy = maxY;
			speedy = -0.97 * speedy;
		}

		lcd.clearScreen();
		lcd.fillCircle(posx, posy, radius);
		lcd.flush();

		steady_clock::time_point curTime = steady_clock::now();
		if (curTime - lastTime >= std::chrono::seconds(1)) {
			uint32_t curFC = lcd.getFrameCounter();
			printf("fps: %d\n", curFC - lastFC);
			lastTime = curTime;
			lastFC = curFC;
		}
	}
}

int main(void) {
	printf("Starting\n");
	testBasic1();
	testBasic2();
	testFPS();
	return 0;
}
