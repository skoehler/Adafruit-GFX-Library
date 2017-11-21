#include <stdio.h>

#include <chrono>

#include "../OLEDDisplay.h"

using std::chrono::steady_clock;
using namespace GFX;

void testBasicCommon(color_t bg, color_t fg) {
	OLEDDisplay disp(128, 128);
	disp.setBgColor(bg);
	disp.setDrawColor(fg);
	disp.clearScreen();
	disp.enable();

	disp.drawRect(10, 40, 30, 50);
	disp.drawCircle(20, 20, 10);
	disp.fillCircle(50, 20, 10);
	disp.fillRect(40, 40, 60, 50);

	disp.drawLine(10, 10, 56, 56);

	disp.setCursor(10, 60);
	disp.setTextColor(fg);
	disp.print("Hello World!");

	disp.setCursor(15, 65);
	disp.setTextColor(fg, bg);
	disp.print("Hello World!");

	disp.setCursor(20, 70);
	disp.setTextColor(fg);
	disp.print("Hello World!");

	disp.flush();
	std::this_thread::sleep_for(std::chrono::seconds(5));
}

void testBasic1() {
	testBasicCommon(COLOR_WHITE, COLOR_BLACK);
}

void testBasic2() {
	testBasicCommon(COLOR_BLACK, COLOR_WHITE);
}

void testFPS() {
	OLEDDisplay disp(128, 128);
	disp.enable();

	disp.setBgColor(COLOR_BLACK);
	disp.setDrawColor(COLOR_WHITE);

	steady_clock::time_point lastTime = steady_clock::now();
	uint32_t lastFC = disp.getFrameCounter();

	const int radius=10;
	const int minX = radius;
	const int maxX = disp.getWidth() - radius;
	const int maxY = disp.getHeight() - radius;

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

		disp.clearScreen();
		disp.fillCircle(posx, posy, radius);
		disp.flush();

		steady_clock::time_point curTime = steady_clock::now();
		if (curTime - lastTime >= std::chrono::seconds(1)) {
			uint32_t curFC = disp.getFrameCounter();
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
