#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <chrono>

#include "../OLEDDisplay.h"

using namespace GFX;

void testCircle() {
	OLEDDisplay disp(128, 128);

	disp.setBgColor(COLOR_BLACK);
	disp.setDrawColor(COLOR_WHITE);
	disp.clearScreen();
	disp.enable();

	for (int i=0, x=1; i<11; i++) {
		disp.drawCircle( x,  10, i);
		x += 2*i+3;
	}
	for (int i=0, x=1; i<11; i++) {
		disp.fillCircle( x,  34, i);
		x += 2*i+3;
	}

	disp.drawRect(1, 44, 1+30, 44+20);
	disp.fillRect(35, 44, 35+30, 44+20);

	disp.drawRoundRect(1, 67, 1+30, 67+20, 7);
	disp.fillRoundRect(35, 67, 35+30, 67+20, 7);


	disp.flush();
	std::this_thread::sleep_for(std::chrono::seconds(60));
}

int main(void) {
	printf("Starting\n");
	testCircle();
	return 0;
}
