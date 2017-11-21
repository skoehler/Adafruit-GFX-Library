#ifndef OLEDDISPLAY_H_
#define OLEDDISPLAY_H_

#include <mutex>
#include <thread>
#include <condition_variable>

#include <mraa/gpio.hpp>
#include <mraa/pwm.hpp>
#include <mraa/spi.hpp>

#include "Canvas.h"

namespace GFX {

class OLEDDisplay : public Canvas4bpp{
private:
	mraa::Gpio rst; // Reset
	mraa::Gpio dc; // Data/Command
	mraa::Spi spi;

	/**
	 * 2 additional Framebuffers
	 */
	std::unique_ptr<uint8_t[]> cmdBuf[2];

	std::mutex refreshMutex;
	std::thread dispThread;
	std::condition_variable refreshCond;
	// thread has been started and has not been joined
	bool refreshEnabled;
	// thread is running
	bool refreshRunning;
	// thread should terminate
	bool refreshTerminate;
	// count frames to compute fps
	uint32_t frameCounter;
	uint8_t cmdBufIndex;
	bool cmdBufUsed[2];

	void refreshDisplay();

public:
	OLEDDisplay(int width, int height);
	virtual ~OLEDDisplay();

	void enable();
	void disable();

	uint32_t getFrameCounter();

	virtual void flush();
};

}



#endif /* OLEDDISPLAY_H_ */
