#ifndef _SEPS525_OLED_H_
#define _SEPS525_OLED_H_

#include <Adafruit_GFX.h>

class Pin;

class SEPS525_OLED : public Adafruit_GFX {
	public:
        SEPS525_OLED(Pin* pinRS, Pin* pinSS, Pin* pinReset, Pin *pinVddEnable);

		void begin(void);

		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

private:
    Pin *pinRS, *pinSS, *pinReset, *pinVddEnable;
};

#endif
