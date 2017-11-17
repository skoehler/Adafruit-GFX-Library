#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../OLEDDisplay.h"

using std::chrono::steady_clock;
using namespace GFX;

void testFPS() {
	OLEDDisplay lcd(128, 128);

	lcd.setBgColor(COLOR_BLACK);
	lcd.setDrawColor(COLOR_WHITE);
	lcd.clearScreen();
	lcd.enable();

	lcd.drawCircle( 1,  5, 0);
	lcd.drawCircle( 4,  5, 1);
	lcd.drawCircle( 9,  5, 2);
	lcd.drawCircle(16,  5, 3);
	lcd.drawCircle(25,  5, 4);
	lcd.drawCircle(36,  5, 5);
	lcd.fillCircle( 1, 18, 0);
	lcd.fillCircle( 4, 18, 1);
	lcd.fillCircle( 9, 18, 2);
	lcd.fillCircle(16, 18, 3);
	lcd.fillCircle(25, 18, 4);
	lcd.fillCircle(36, 18, 5);

	lcd.flush();
	std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main(void) {
	printf("Starting\n");
	testFPS();
	return 0;
}
