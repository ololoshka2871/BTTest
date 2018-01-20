#ifndef DISPLAYDEMO_H
#define DISPLAYDEMO_H

class SEP525_DMA_FreeRTOS;

class DisplayDemo
{
private:
    DisplayDemo(SEP525_DMA_FreeRTOS *display) : display(display) {}

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
    unsigned long testdrawBMP();

    unsigned long playDemo(int demo);
    static const int demo_count = 13;

private:
    SEP525_DMA_FreeRTOS* display;
};

#endif // DISPLAYDEMO_H
