#include <FreeRTOS.h>
#include <task.h>

#include "SelfTestScreen.h"

#include "TENSController.h"

#include "SEP525_DMA_FreeRTOS.h"

const uint16_t SelfTestScreen::iv_positions[6] = {55, 65, 75, 85, 95, 105};

const SelfTestScreen::action SelfTestScreen::action_order[8] = {
    &SelfTestScreen::Init,
    &SelfTestScreen::Test<0>,
    &SelfTestScreen::Test<1>,
    &SelfTestScreen::Test<2>,
    &SelfTestScreen::Test<3>,
    &SelfTestScreen::Test<4>,
    &SelfTestScreen::Test<5>,
    &SelfTestScreen::TestPassed,
};


void SelfTestScreen::PrintOK(int vpos)
{
    screen->setTextColor(green);
    screen->setCursor(100, vpos);
    screen->println("OK");
}

void SelfTestScreen::Blink_Warning(bool Display)
{
    const uint16_t warning_color = 0xB1C7;

    if (Display) {
        screen->setTextColor(warning_color);
        screen->setTextSize(2);

        screen->setCursor(20, 10);
        screen->println("WARNING!");

        screen->setTextSize(1);
        screen->setCursor(16, 30);
        screen->println("It's a PROTOTYPE!");
    } else
        screen->fillRect(0, 0, screen->width(), 45, background_color);
}

void SelfTestScreen::DrawProgressBar()
{
    const uint16_t progress_bar_x = 20;
    const uint16_t progress_bar_y = 120;
    const uint16_t progress_bar_w = screen->width() - progress_bar_x * 2;
    const uint16_t progress_bar_h = 5;

    const uint16_t progress_bar_color = 0xE0C5;

    screen->drawRect(progress_bar_x, progress_bar_y, progress_bar_w, progress_bar_h, progress_bar_color);
    screen->fillRect(progress_bar_x, progress_bar_y, progress_bar_w * percent / 100, progress_bar_h, progress_bar_color);
}

void SelfTestScreen::pointsClear()
{
    screen->fillRect(pointspos_x, pointspos_y, screen->width() - pointspos_x,
                    screen->height() - pointspos_y, background_color);
}

void SelfTestScreen::drawPoint(uint8_t n)
{
    screen->setTextSize(1);
    screen->setTextColor(black);
    screen->setCursor(pointspos_x + 3 * n, pointspos_y);
    screen->print('.');
}

void SelfTestScreen::Init()
{
    const uint16_t text_x_offset = 16;

    screen->fillScreen(background_color);
    DrawProgressBar();
    Blink_Warning(true);

    screen->setTextColor(black);
    screen->setTextSize(1);

    screen->setCursor(text_x_offset, iv_positions[0]);
    screen->println("LASER");

    screen->setCursor(text_x_offset, iv_positions[1]);
    screen->println("INFRARED");

    screen->setCursor(text_x_offset, iv_positions[2]);
    screen->println("RED");

    screen->setCursor(text_x_offset, iv_positions[3]);
    screen->println("BLUE");

    screen->setCursor(text_x_offset, iv_positions[4]);
    screen->println("MAGNET");

    screen->setCursor(text_x_offset, iv_positions[5]);
    screen->println("TENS");


    screen->setCursor(5, 135);
    screen->println("System Self-checking");
    screen->setCursor(28, 146);
    screen->println("Please wait");
}

void SelfTestScreen::TestCommon(uint8_t test_num) {
    const uint8_t points_count = 3;
    const uint8_t smoothing = 2;

    pointsClear();

    if (test_num == 3 || test_num == 4) {
        TENSController::instance()->enable(true, false);
    }

    for (auto i = 0; i < points_count * smoothing; ++i) {
        Blink_Warning(i % 3);
        percent = (test_num * points_count * smoothing + i) * 100 /
                (sizeof(iv_positions) / sizeof(uint16_t) * points_count * smoothing);
        DrawProgressBar();
        if (i % 2)
            drawPoint(i >> 1);
        vTaskDelay(150);
    }

    TENSController::instance()->enable(false, false);

    PrintOK(iv_positions[test_num]);
}

void SelfTestScreen::TestPassed()
{
    pointsClear();
    percent = 100;
    DrawProgressBar();
}

uint32_t SelfTestScreen::Display(DisplayController &controller)
{
    screen = &controller.getScreen();

    unsigned long start = micros();

    for (size_t i = 0; i < sizeof(action_order) / sizeof(action); ++i) {
        (*this.*action_order[i])();
    }

    return micros() - start;
}

