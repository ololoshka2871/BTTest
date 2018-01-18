#ifndef _SEPS525_OLED_H_
#define _SEPS525_OLED_H_

#include <Adafruit_GFX.h>

class Pin;
class SPIClass;

class SEPS525_OLED : public Adafruit_GFX {
	public:
        SEPS525_OLED(SPIClass* spi, Pin* pinRS, Pin* pinSS, Pin* pinReset, Pin *pinVddEnable);

		void begin(void);

		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
        static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

private:
    Pin *pinRS, *pinSS, *pinReset, *pinVddEnable;
    SPIClass* SPI;

    void seps525_init();
    void setup();

    void reg(int idx, int value);
    inline void datastart(void);
    inline void data(int value);
    inline void dataend(void);
    void set_region(int x, int y, int xs, int ys);
};

#endif
