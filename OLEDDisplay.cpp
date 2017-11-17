#include "OLEDDisplay.h"

#include <chrono>
#include <cstring>
#include <iostream>

#include <mraa/common.hpp>

using namespace GFX;



static void mR(mraa::Result r) {
	if (r != mraa::SUCCESS) {
		mraa::printError(r);
		throw std::invalid_argument("MRAA error");
	}
}

static void mraa_dir_retry(mraa::Gpio &gpio, mraa::Dir dir) {
	while (gpio.dir(dir) != mraa::SUCCESS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

OLEDDisplay::OLEDDisplay(coord_t width, coord_t height)
	: Canvas4bpp(width, height), rst(15), dc(29), spi(0, 0) {
	refreshEnabled = false;
	refreshRunning = false;
	refreshTerminate = false;
	frameCounter = 0;

	//GPIO Init
	mraa_dir_retry(rst, mraa::DIR_OUT);
	mraa_dir_retry(dc, mraa::DIR_OUT);
	rst.write(0);
	dc.write(0);

	// Set SPI clock to 32MHz
	mR(spi.mode(mraa::SPI_MODE0));
	mR(spi.frequency(32 * 1000 * 1000));
	mR(spi.bitPerWord(8));
	mR(spi.lsbmode(false));

	coord_t rowsize = (WIDTH + 1)/2;
	cmdBuf[0].assign(rowsize * HEIGHT, 0);
	cmdBuf[1] = cmdBuf[0];
	cmdBufIndex = 0;

	flush();
}
OLEDDisplay::~OLEDDisplay() {
	refreshTerminate = true;
	if (refreshEnabled) {
		dispThread.join();
	}
	rst.write(0);
	dc.write(0);
}

void OLEDDisplay::enable() {
	if (refreshEnabled)
		return;

	rst.write(0); //Reset pin low
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //100ms delay
	rst.write(1); //Reset pin high
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //100ms delay
	uint8_t cmd1[] = {
		0xA4, //set normal display mode
		0x81,
		0x7F, //Set contrast to 0x7F (default)
		0xB3,
		0x40, //clock divider/oscillator frequency
		0xAF, //Display on
		0xA0, //set memory addressing mode ...
		0x51, //... to horizontal address increment
			  //..enable column address remap
			  //...enable COM remap
			  //...enable COM split odd even
		0xA1,
		0x00, //set display start line to 0
	};
	dc.write(0); //set D/C# pin low
	spi.transfer(cmd1, NULL, sizeof(cmd1));

	uint8_t cmd2[] = {
		0x15,     //set column address
		0,        //start address
		WIDTH-1,  //end address
		0x75,     //set page address
		0,        //start page
		HEIGHT-1, //stop page
	};
	dc.write(0); //set D/C# pin low
	spi.transfer(cmd2, NULL, sizeof(cmd2));

	// start display thread
	refreshEnabled = true;
	refreshTerminate = false;

	dc.write(1); //set D/C# pin high
	dispThread = std::thread(&OLEDDisplay::refreshDisplay, this,
			cmdBuf[0].data(), cmdBuf[1].data(), cmdBuf[1].size());
}

void OLEDDisplay::disable() {
	refreshTerminate = true;
	if (refreshEnabled) {
		dispThread.join();
		refreshEnabled = false;
	}
	rst.write(0);
	dc.write(0);
}

void OLEDDisplay::refreshDisplay(uint8_t *data0, uint8_t *data1, int len) {
	// for good measure, wait a few ms until things have settled
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	std::unique_lock<std::mutex> refreshLock(refreshMutex);
	refreshRunning = true;
	refreshCond.notify_all();

	uint8_t * const dataPtr[2] = {data0, data1};
	while (!refreshTerminate) {
		uint8_t index = cmdBufIndex;
		uint8_t *data = dataPtr[index];
		cmdBufUsed[index] = true;
		frameCounter++;
		refreshLock.unlock();
		std::chrono::steady_clock::time_point t = std::chrono::steady_clock::now();
		//TODO check for error
		spi.transfer(data, NULL, len);
		refreshCond.notify_all();
		std::this_thread::sleep_until(t + std::chrono::microseconds(16666));
		refreshLock.lock();
	}
	refreshRunning = false;
	refreshCond.notify_all();
}


uint32_t OLEDDisplay::getFrameCounter() {
	std::unique_lock<std::mutex> refreshLock(refreshMutex);
	return this->frameCounter;
}

void OLEDDisplay::flush() {
	uint8_t freeIndex = 1 - cmdBufIndex;
	uint8_t *dst = cmdBuf[freeIndex].data();
	uint8_t *src = this->getBuffer();
	std::memcpy(dst, src, cmdBuf[freeIndex].size());
	std::unique_lock<std::mutex> refreshLock(refreshMutex);
	cmdBufIndex = freeIndex;
	cmdBufUsed[freeIndex] = false;
	while (!cmdBufUsed[freeIndex] && refreshRunning) {
		refreshCond.wait(refreshLock);
	}
}
