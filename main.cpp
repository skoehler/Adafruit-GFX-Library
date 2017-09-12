#include <cinttypes>
#include <atomic>
#include <thread>

#include <unistd.h>
#include <mraa.h>

#include "Adafruit_GFX.h"


#define US_OFF  1
#define US_ON   0
#define US_LOW  1
#define US_HIGH 0

mraa_gpio_context us_trig;
mraa_gpio_context us_echo;
std::atomic<double> us_dist;

void debug(const char *txt, mraa_result_t r) {
	printf("%s: %d\n", txt, r);
}
void debug(const char *txt, void *r) {
	printf("%s: %p\n", txt, r);
}

double getTime() {
	struct timespec s;
	clock_gettime(CLOCK_MONOTONIC_RAW, &s);
	return s.tv_sec + s.tv_nsec * 1.0e-9;
}

void gpio_dir(mraa_gpio_context g, mraa_gpio_dir_t dir) {
	while (1) {
		mraa_result_t r = mraa_gpio_dir(g, dir);
		if (r == MRAA_SUCCESS)
			break;
		usleep(100 * 1000);
	}
}
mraa_gpio_context gpio_init(int pin, mraa_gpio_dir_t dir) {
	mraa_gpio_context c = mraa_gpio_init(pin);
	gpio_dir(c, dir);
	return c;
}

void runUS() {
	us_dist.store(0.0/0.0);
	mraa_gpio_write(us_trig, US_OFF);

	while (1) {
		usleep(100 * 1000);

		mraa_gpio_write(us_trig, US_ON);
		usleep(10);
		mraa_gpio_write(us_trig, US_OFF);

		double start = 0, stop = 0;
		while (mraa_gpio_read(us_echo) == US_LOW)
			start = getTime();
		while (mraa_gpio_read(us_echo) == US_HIGH)
			stop = getTime();

		double dist = (stop - start) * 17150;
		us_dist.store(dist);
	}
}

/*********************************************************************************
 Global variables
 *********************************************************************************/
uint8_t disp_buf[20]; //SPI command buffer
uint8_t disp_data[128 * 128 / 2]; //SPI data buffer
const uint8_t disp_lookup[4] = { 0x0, 0xF, 0xF0, 0xFF }; //lookup table

mraa_spi_context disp_spi;
mraa_gpio_context disp_dc;
mraa_gpio_context disp_rst;

/*********************************************************************************
 Function name: initW128128
 Description: Initialization of the display
 *********************************************************************************/

void setReset(int val) {
	mraa_result_t r;
	r = mraa_gpio_write(disp_rst, val);
	debug("setReset", r);
}

void setDC(int val) {
	mraa_result_t r;
	r = mraa_gpio_write(disp_dc, val);
	debug("setDC", r);
}

static inline void waitforemptybuffer() {
	//TODO
}

static inline void ms_delay(int ms) {
	usleep(ms * 1000);
}

static inline void R_RSPI0_Send(uint8_t *buf, size_t len) {
//	mraa_result_t r;
	mraa_spi_transfer_buf(disp_spi, buf, NULL, len);
//	debug("SPI send", r);
}

void initW128128(void) {
	uint16_t i = 0;
	setReset(0); //Reset pin low
	ms_delay(100); //100ms delay
	setReset(1); //Reset pin high
	ms_delay(100); //100ms delay
	disp_buf[i++] = 0xA4; //set normal display mode
	disp_buf[i++] = 0x81;
	disp_buf[i++] = 0x7F; //Set contrast to 0x7F (default)
	disp_buf[i++] = 0xB3;
	disp_buf[i++] = 0x40; //clock divider/oscillator frequency
	disp_buf[i++] = 0xAF; //Display on
	disp_buf[i++] = 0xA0; //set memory addressing mode ...
	disp_buf[i++] = 0x51; //... to horizontal address increment
						  //..enable column address remap
						  //...enable COM remap
						  //...enable COM split odd even
	disp_buf[i++] = 0xA1;
	disp_buf[i++] = 0x00; //set display start line to 0
	waitforemptybuffer(); //Waits until SPI buffer is empty
	setDC(0); //D/C# pin low
	R_RSPI0_Send(disp_buf, i); //send initialization buffer via SPI
}

/*********************************************************************************
 Function name: initWindow
 Description: Initialization of the window in horizontal addressing mode
 *********************************************************************************/
void initWindow(uint8_t startcol, uint8_t stopcol, uint8_t startrow,
		uint8_t stoprow) {
	uint16_t i = 0;
	disp_buf[i++] = 0x15; //set column address
	disp_buf[i++] = startcol; //start address
	disp_buf[i++] = stopcol; //end address
	disp_buf[i++] = 0x75; //set page address
	disp_buf[i++] = startrow; //start page
	disp_buf[i++] = stoprow; //stop page
	waitforemptybuffer(); //waits until SPI buffer is empty
	setDC(0); //set D/C# pin low
	R_RSPI0_Send(disp_buf, i); //send data buffer via SPI
}
/*********************************************************************************
 Function name: sendDataW128128
 Description: Sends data to the display (Initialization of the window before sending data to
 the display -> initWindow()
 Display controller provides 4Bit grayscale -> function only use monochrome data
 *********************************************************************************/
void sendDataW128128(const uint8_t *tx_buf, uint16_t tx_num) {
	uint16_t i, j;
	uint16_t count = 0;
	uint8_t byte;
	//convert monochrome pixel data to 4Bit grayscale 0->0000; 1->1111
	//see example below
	for (i = 0; i < tx_num; i++) {
		byte = tx_buf[i];
		for (j = 0; j < 4; j++) {
			disp_data[count] = disp_lookup[(byte & 0xC0) >> 6];
			byte = byte << 2;
			count++;
		}
	}
	waitforemptybuffer(); //waits until SPI buffer is empty
	R_RSPI0_Send(disp_data, (tx_num << 2)); //send data buffer via SPI
}

#define BT_ON "X"
#define BT_OFF "O"

int main(void) {
	mraa_result_t r;

	disp_spi = mraa_spi_init_raw(0, 0);
	debug("initspi", disp_spi);
	r = mraa_spi_mode(disp_spi, MRAA_SPI_MODE0);
	debug("mode", r);
	r = mraa_spi_frequency(disp_spi, 32 * 1000 * 1000);
	debug("freq", r);
	r = mraa_spi_bit_per_word(disp_spi, 8);
	debug("bits", r);
	r = mraa_spi_lsbmode(disp_spi, 0);
	debug("lsb", r);

	disp_dc = gpio_init(29, MRAA_GPIO_OUT);
	disp_rst = gpio_init(15, MRAA_GPIO_OUT);

	us_trig = gpio_init(12, MRAA_GPIO_OUT);
	us_echo = gpio_init(18, MRAA_GPIO_IN);

	mraa_gpio_context bt_up = gpio_init(36, MRAA_GPIO_IN);
	mraa_gpio_context bt_lt = gpio_init(32, MRAA_GPIO_IN);
	mraa_gpio_context bt_ct = gpio_init(38, MRAA_GPIO_IN);
	mraa_gpio_context bt_rt = gpio_init(40, MRAA_GPIO_IN);
	mraa_gpio_context bt_dn = gpio_init(31, MRAA_GPIO_IN);

	mraa_gpio_context led1 = gpio_init(33, MRAA_GPIO_OUT);
	mraa_gpio_context led2 = gpio_init(35, MRAA_GPIO_OUT);
	mraa_gpio_context led3 = gpio_init(37, MRAA_GPIO_OUT);

	initW128128();
	initWindow(0, 127, 0, 127);
	setDC(1); //set D/C# pin high

	GFXcanvas1 canvas(128, 128);
	char buf[1024];

	std::thread t_us(runUS);

	int up, lt, ct, rt, dn;

	while (1) {
		canvas.fillRect(0, 0, 128, 128, 0);
		canvas.setCursor(0, 0);

		up = mraa_gpio_read(bt_up);
		lt = mraa_gpio_read(bt_lt);
		ct = mraa_gpio_read(bt_ct);
		rt = mraa_gpio_read(bt_rt);
		dn = mraa_gpio_read(bt_dn);

		mraa_gpio_write(led1, up && lt);
		mraa_gpio_write(led2, ct);
		mraa_gpio_write(led3, dn && rt);

		sprintf(buf, "\n"
				"Up:     %s\n"
				"Left:   %s\n"
				"Center: %s\n"
				"Right:  %s\n"
				"Down:   %s\n"
				"\n"
				"US Dist:%7.2f\n",
				up ? BT_OFF : BT_ON,
				lt ? BT_OFF : BT_ON,
				ct ? BT_OFF : BT_ON,
				rt ? BT_OFF : BT_ON,
				dn ? BT_OFF : BT_ON,
				us_dist.load());
		canvas.print(buf);

		sendDataW128128(canvas.getBuffer(), 128*128/8);
		usleep(20 * 1000);
	}

}
