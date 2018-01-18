#ifndef DISPLAYDEMO_H
#define DISPLAYDEMO_H

class Adafruit_GFX;

class DisplayDemo
{
private:
    DisplayDemo(Adafruit_GFX *display) : display(display) {}

public:
    static void vDisplayDemoThreadFunc(void* pvParameters);

    // demos
    unsigned long testFillScreen();
    unsigned long testText();
    unsigned long testLines(uint16_t color);
    unsigned long testFastLines(uint16_t color1, uint16_t color2);
    unsigned long testRects(uint16_t color);
    unsigned long testFilledRects(uint16_t color1, uint16_t color2);
    unsigned long testFilledCircles(uint8_t radius, uint16_t color);
    unsigned long testCircles(uint8_t radius, uint16_t color);
    unsigned long testTriangles();
    unsigned long testFilledTriangles();
    unsigned long testRoundRects();
    unsigned long testFilledRoundRects();

    unsigned long playDemo(int demo);
    static const int demo_count = 12;

private:
    Adafruit_GFX* display;
};

#endif // DISPLAYDEMO_H
