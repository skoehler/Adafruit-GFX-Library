#include <stdio.h>

#include "../OLEDDisplay.h"

using namespace GFX;

void testFonts() {
	OLEDDisplay disp(128, 128);

	disp.setBgColor(COLOR_BLACK);
	disp.setDrawColor(COLOR_WHITE);
	disp.clearScreen();
	disp.enable();

	disp.setCursor(10, 10);
	disp.print("Hello World!");


	disp.setFont(&FreeSerif9pt7b);
	disp.setCursor(10, 30);
	disp.print("Hello World!");

	disp.setFont(&FreeSerif18pt7b);
	disp.setCursor(10, 60);
	disp.print("Hello World!");

	disp.flush();
	std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main(void) {
	printf("Starting\n");
	testFonts();
	return 0;
}
