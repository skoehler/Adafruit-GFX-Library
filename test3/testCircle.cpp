#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../OLEDDisplay.h"

using std::chrono::steady_clock;
using namespace GFX;

void testCircle() {
	OLEDDisplay lcd(128, 128);

	lcd.setBgColor(COLOR_BLACK);
	lcd.setDrawColor(COLOR_WHITE);
	lcd.clearScreen();
	lcd.enable();

	for (int i=0, x=0; i<10; i++) {
		lcd.drawCircle( x,  10, i);
		x += 2*i+3;
	}
	for (int i=0, x=0; i<10; i++) {
		lcd.fillCircle( x,  32, i);
		x += 2*i+3;
	}

	lcd.drawRect(1, 42, 1+30, 42+20);
	lcd.fillRect(35, 42, 35+30, 42+20);

	lcd.drawRoundRect(1, 65, 1+30, 65+20, 7);
	lcd.fillRoundRect(35, 65, 35+30, 65+20, 7);


	lcd.flush();
	std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main(void) {
	printf("Starting\n");
	testCircle();
	return 0;
}
