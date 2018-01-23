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

    void seps525_init();

protected:
    SPIClass* SPI;

    virtual void reg(int idx, int value);
    virtual void datastart(void);
    virtual void data(int value);
    virtual void dataend(void);
    virtual void set_region(int x, int y, int w, int h);
    virtual void setup();
    virtual void set_start_pos(int x, int y);
    virtual void select_region(int x, int y, int w, int h);
};

#endif
