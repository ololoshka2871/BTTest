#ifndef SELFTESTSCREEN_H
#define SELFTESTSCREEN_H

#include "Screen.h"

class SelfTestScreen : public IScreen
{
public:
    typedef void (SelfTestScreen::*action)();

    static const uint16_t background_color = 0xffff;
    static const uint16_t black = 0;
    static const uint16_t green = 0x3C44;
    static const uint16_t pointspos_x = 95;
    static const uint16_t pointspos_y = 146;
    static const uint16_t iv_positions[6];

    SelfTestScreen() { percent = 0; }
    ~SelfTestScreen() {}

    uint32_t Display(DisplayController& controller);

private:
    void PrintOK(int vpos);
    void Blink_Warning(bool Display = true);
    void DrawProgressBar();
    void pointsClear();
    void drawPoint(uint8_t n);

    //////////////////////

    void Init();

    template<uint8_t test_num>
    void Test() {
        TestCommon(test_num);
    }
    void TestCommon(uint8_t test_num);

    void TestPassed();

    //////////////////////

    static const action action_order[8];

    uint8_t percent;

    SEP525_DMA_FreeRTOS* screen;
};

#endif // SELFTESTSCREEN_H
