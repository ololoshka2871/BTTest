#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>

#include <SEPS525_OLED.h>
#include <Pin.h>
#include <SPI.h>
#include "hw_includes.h"

#include "displaydemo.h"
#include "SerialDebugLogger.h"
#include "SEP525_DMA_FreeRTOS.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


void DisplayDemo::vDisplayDemoThreadFunc(void *pvParameters)
{
    auto d = SEP525_DMA_FreeRTOS::instance();

    d->begin();
    DisplayDemo demo(d);

    int counter = 0;
    unsigned long time;

    while(1) {
        time = demo.playDemo(counter);
        serialDebugWrite("Test #%d: took %d\n\r", counter++, time);
        vTaskDelay(100);
    }
}

//////// DEMOS ////////////////

unsigned long DisplayDemo::testFillScreen() {
    unsigned long start = micros();
    display->fillScreen(BLACK);
    display->fillScreen(RED);
    display->fillScreen(GREEN);
    display->fillScreen(BLUE);
    display->fillScreen(BLACK);
    return micros() - start;
}

unsigned long DisplayDemo::testText() {
    display->fillScreen(BLACK);
    unsigned long start = micros();
    display->setCursor(0, 0);
    display->setTextColor(WHITE);  display->setTextSize(1);
    display->println("Hello World!");
    display->setTextColor(YELLOW); display->setTextSize(2);
    display->println(1234.56);
    display->setTextColor(RED);    display->setTextSize(3);
    display->println(0xDEADBEEF, HEX);
    display->setTextColor(GREEN);
    display->setTextSize(5);
    display->println("Groop");
    return micros() - start;
}

unsigned long DisplayDemo::testLines(uint16_t color) {
    unsigned long start, t;
    int           x1, y1, x2, y2,
            w = display->width(),
            h = display->height();

    display->fillScreen(BLACK);

    x1 = y1 = 0;
    y2    = h - 1;
    start = micros();
    for(x2=0; x2<w; x2+=6) display->drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for(y2=0; y2<h; y2+=6) display->drawLine(x1, y1, x2, y2, color);
    t     = micros() - start; // fillScreen doesn't count against timing

    display->fillScreen(BLACK);

    x1    = w - 1;
    y1    = 0;
    y2    = h - 1;
    start = micros();
    for(x2=0; x2<w; x2+=6) display->drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for(y2=0; y2<h; y2+=6) display->drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    display->fillScreen(BLACK);

    x1    = 0;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for(x2=0; x2<w; x2+=6) display->drawLine(x1, y1, x2, y2, color);
    x2    = w - 1;
    for(y2=0; y2<h; y2+=6) display->drawLine(x1, y1, x2, y2, color);
    t    += micros() - start;

    display->fillScreen(BLACK);

    x1    = w - 1;
    y1    = h - 1;
    y2    = 0;
    start = micros();
    for(x2=0; x2<w; x2+=6) display->drawLine(x1, y1, x2, y2, color);
    x2    = 0;
    for(y2=0; y2<h; y2+=6) display->drawLine(x1, y1, x2, y2, color);

    return micros() - start;
}

unsigned long DisplayDemo::testFastLines(uint16_t color1, uint16_t color2) {
    unsigned long start;
    int           x, y, w = display->width(), h = display->height();

    display->fillScreen(BLACK);
    start = micros();
    for(y=0; y<h; y+=5) display->drawFastHLine(0, y, w, color1);
    for(x=0; x<w; x+=5) display->drawFastVLine(x, 0, h, color2);

    return micros() - start;
}

unsigned long DisplayDemo::testRects(uint16_t color) {
    unsigned long start;
    int           n, i, i2,
            cx = display->width()  / 2,
            cy = display->height() / 2;

    display->fillScreen(BLACK);
    n     = min(display->width(), display->height());
    start = micros();
    for(i=2; i<n; i+=6) {
        i2 = i / 2;
        display->drawRect(cx-i2, cy-i2, i, i, color);
    }

    return micros() - start;
}

unsigned long DisplayDemo::testFilledRects(uint16_t color1, uint16_t color2) {
    unsigned long start, t = 0;
    int           n, i, i2,
            cx = display->width()  / 2 - 1,
            cy = display->height() / 2 - 1;

    display->fillScreen(BLACK);
    n = min(display->width(), display->height());
    for(i=n; i>0; i-=6) {
        i2    = i / 2;
        start = micros();
        display->fillRect(cx-i2, cy-i2, i, i, color1);
        t    += micros() - start;
        // Outlines are not included in timing results
        display->drawRect(cx-i2, cy-i2, i, i, color2);
    }

    return t;
}

unsigned long DisplayDemo::testFilledCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int x, y, w = display->width(), h = display->height(), r2 = radius * 2;

    display->fillScreen(BLACK);
    start = micros();
    for(x=radius; x<w; x+=r2) {
        for(y=radius; y<h; y+=r2) {
            display->fillCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long DisplayDemo::testCircles(uint8_t radius, uint16_t color) {
    unsigned long start;
    int           x, y, r2 = radius * 2,
            w = display->width()  + radius,
            h = display->height() + radius;

    // Screen is not cleared for this one -- this is
    // intentional and does not affect the reported time.
    start = micros();
    for(x=0; x<w; x+=r2) {
        for(y=0; y<h; y+=r2) {
            display->drawCircle(x, y, radius, color);
        }
    }

    return micros() - start;
}

unsigned long DisplayDemo::testTriangles() {
    unsigned long start;
    int           n, i, cx = display->width()  / 2 - 1,
            cy = display->height() / 2 - 1;

    display->fillScreen(BLACK);
    n     = min(cx, cy);
    start = micros();
    for(i=0; i<n; i+=5) {
        display->drawTriangle(
                    cx    , cy - i, // peak
                    cx - i, cy + i, // bottom left
                    cx + i, cy + i, // bottom right
                    SEPS525_OLED::color565(0, 0, i));
    }

    return micros() - start;
}

unsigned long DisplayDemo::testFilledTriangles() {
    unsigned long start, t = 0;
    int           i, cx = display->width()  / 2 - 1,
            cy = display->height() / 2 - 1;

    display->fillScreen(BLACK);
    start = micros();
    for(i=min(cx,cy); i>10; i-=5) {
        start = micros();
        display->fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                              SEPS525_OLED::color565(0, i, i));
        t += micros() - start;
        display->drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                              SEPS525_OLED::color565(i, i, 0));
    }

    return t;
}

unsigned long DisplayDemo::testRoundRects() {
    unsigned long start;
    int           w, i, i2,
            cx = display->width()  / 2 - 1,
            cy = display->height() / 2 - 1;

    display->fillScreen(BLACK);
    w     = min(display->width(), display->height());
    start = micros();
    for(i=0; i<w; i+=6) {
        i2 = i / 2;
        display->drawRoundRect(cx-i2, cy-i2, i, i, i/8, SEPS525_OLED::color565(i, 0, 0));
    }

    return micros() - start;
}

unsigned long DisplayDemo::testFilledRoundRects() {
    unsigned long start;
    int           i, i2,
            cx = display->width()  / 2 - 1,
            cy = display->height() / 2 - 1;

    display->fillScreen(BLACK);
    start = micros();
    for(i=min(display->width(), display->height()); i>20; i-=6) {
        i2 = i / 2;
        display->fillRoundRect(cx-i2, cy-i2, i, i, i/8, SEPS525_OLED::color565(0, i, 0));
    }

    return micros() - start;
}

/////////////////////////////////////////////

unsigned long DisplayDemo::playDemo(int demo)
{
    demo %= demo_count;
    switch (demo) {
    case 0:
        return testFillScreen();
    case 1:
        return testText();
    case 2:
        return testLines(CYAN);
    case 3:
        return testFastLines(RED, BLUE);
    case 4:
        return testRects(GREEN);
    case 5:
        return testFilledRects(YELLOW, MAGENTA);
    case 6:
        return testFilledCircles(10, MAGENTA);
    case 7:
        return testCircles(10, WHITE);
    case 8:
        return testTriangles();
    case 9:
        return testFilledTriangles();
    case 10:
        return testRoundRects();
    case 11:
        return testFilledRoundRects();
    default:
        return 0;
    }
}
