#include <SEPS525_OLED.h>
#include <Pin.h>
#include <SPI.h>

void SEPS525_OLED::reg(int idx, int value)
{
    pinSS->setValue(LOW);
    pinRS->setValue(LOW);
    SPI->transfer(idx);
    pinRS->setValue(HIGH);
    pinSS->setValue(HIGH);

    pinSS->setValue(LOW);
    SPI->transfer(value);
    pinSS->setValue(HIGH);
}

void SEPS525_OLED::datastart()
{
    pinSS->setValue(LOW);
    pinRS->setValue(LOW);
    SPI->transfer(0x22);
    pinRS->setValue(HIGH);
}

void SEPS525_OLED::data(int value)
{
    SPI->transfer((value>>8)& 0xFF);
    SPI->transfer(value & 0xFF);
}

void SEPS525_OLED::dataend()
{
    pinSS->setValue(HIGH);
}

void SEPS525_OLED::set_region(int x, int y, int xs, int ys)
{
    // draw region
    reg(0x17,x);
    reg(0x18,x+xs-1);
    reg(0x19,y);
    reg(0x1a,y+ys-1);

    // start position
    reg(0x20,x);
    reg(0x21,y);
}

void SEPS525_OLED::setup(void)
{
    SPI->begin();
    SPI->beginTransaction(SPISettings(24000000UL, MSBFIRST, SPI_MODE3));
    //SPI->setBitOrder(MSBFIRST);
    //SPI->setDataMode(SPI_MODE3);
    //SPI->setClockDivider(SPI_CLOCK_DIV2);

    // pin for switcher enable (off by default)
    pinVddEnable->setDirection(Pin::D_OUTPUT);
    pinVddEnable->setValue(HIGH);

    // pins for seps525
    pinRS->setDirection(Pin::D_OUTPUT);
    pinSS->setDirection(Pin::D_OUTPUT);
    pinReset->setDirection(Pin::D_OUTPUT);

    pinRS->setValue(HIGH);
    pinSS->setValue(HIGH);
    pinReset->setValue(HIGH);
}

void SEPS525_OLED::seps525_init(void)
{
    setup();

    // NOTE: this procedure is from Densitron
    // DD-160128FC-2B datasheet

    pinReset->setValue(LOW);
    delay(2);
    pinReset->setValue(HIGH);
    delay(1);

    // display off, analog reset
    reg(0x04, 0x01);
    delay(1);
    // normal mode
    reg(0x04, 0x00);
    delay(1);
    // display off
    reg(0x06, 0x00);
    // turn on internal oscillator using external resistor
    reg(0x02, 0x01);
    // 90 hz frame rate, divider 0
    reg(0x03, 0x30);
    // duty cycle 127
    reg(0x28, 0x7f);
    // start on line 0
    reg(0x29, 0x00);
    // rgb_if
    reg(0x14, 0x31);
    // memory write mode
    reg(0x16, 0x66);

    // driving current r g b (uA)
    reg(0x10, 0x45);
    reg(0x11, 0x34);
    reg(0x12, 0x33);

    // precharge time r g b
    reg(0x08, 0x04);
    reg(0x09, 0x05);
    reg(0x0a, 0x05);

    // precharge current r g b (uA)
    reg(0x0b, 0x9d);
    reg(0x0c, 0x8c);
    reg(0x0d, 0x57);

    reg(0x80, 0x00);

    // mode set
    reg(0x13, 0x00);


    set_region(0, 0, 160, 128);

    datastart();
    int n;
    for(n = 0; n < 160*128; n++) {
        data(0xffff);
    }
    dataend();

    pinVddEnable->setValue(LOW);
    delay(100);

    reg(0x06, 0x01);
}

SEPS525_OLED::SEPS525_OLED(SPIClass *spi, Pin* pinRS, Pin* pinSS, Pin* pinReset, Pin* pinVddEnable) : Adafruit_GFX(160, 128)
{
    this->SPI = spi;
    this->pinRS = pinRS;
    this->pinSS = pinSS;
    this->pinReset = pinReset;
    this->pinVddEnable = pinVddEnable;
}

void SEPS525_OLED::begin(void)
{
    seps525_init();
}

void SEPS525_OLED::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    set_region(x, y, 1, 1);
    datastart();
    data(color);
    dataend();
}

void SEPS525_OLED::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    set_region(x, y, 1, h);
    datastart();
    int n;
    for(n = 0; n < h; n++) data(color);
    dataend();
}

void SEPS525_OLED::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    set_region(x, y, w, 1);
    datastart();
    int n;
    for(n = 0; n < w; n++) data(color);
    dataend();
}

void SEPS525_OLED::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    set_region(x, y, w, h);
    datastart();
    int n;
    for(n = 0; n < h*w; n++) data(color);
    dataend();
}

uint16_t SEPS525_OLED::color565(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 11) | (g << 5) | b;
}
