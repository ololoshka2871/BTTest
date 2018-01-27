#include "MenuItem.h"

#include "SEP525_DMA_FreeRTOS.h"
#include <FatLib/FatFile.h>
#include "ButtonsThread.h"
#include "PrintUtils.h"

#include "TENSController.h"

///////////////////////////////////////////////////////////////////////////

class ExecScreen : public IMenuEntry {
public:
    struct ProcedureTime {
        ProcedureTime(uint8_t m, uint8_t s) : minutes(m), seconds(s) {}
        ProcedureTime(uint32_t s) : minutes(s / 60), seconds(s % 60) {}
        uint32_t minutes, seconds;
        uint32_t toSeconds() const {
            return minutes * 60 + seconds;
        }
    };

    struct TenmentScreens {
        ProcedureTime procedure_time;
        uint32_t screens_count;
        uint32_t loopCount;
        const char* const *filelist;

        uint32_t picturesRotationPeriod() const {
            return procedure_time.toSeconds() / screens_count / loopCount;
        }
    };

    ExecScreen(uint32_t parent)
        : ScreenN(parent), pt{screens[ScreenN].procedure_time}, animation_frame(0),
          screen_timer(screens[ScreenN].picturesRotationPeriod()), screenIndex(0) {}

    uint32_t Display(DisplayController& controller) {
        std::shared_ptr<FatFile> file(new FatFile);

        if(!file->open(&controller.getScreensBaseDir(), screens[ScreenN].filelist[0], O_READ))
            return 0;

        uint32_t taken = controller.LoadImage(file, controller.getScreen().geomety());

        TENSController::instance()->enable(true,
                                   #if MEDICAL_EDITION
                                           true
                                   #else
                                           ScreenN == 1 // только целюлит
                                   #endif
                                           );

        return taken;
    }
    IMenuEntry *onButton(const ButtonMessage &msg);
    IMenuEntry *playAnimation(uint32_t tick, DisplayController& controller);

#if MEDICAL_EDITION
    static const TenmentScreens screens[1];
#else
    static const TenmentScreens screens[2];
#endif

    static const uint32_t FPS = 100;

private:
    uint32_t ScreenN;

    ProcedureTime pt;
    uint32_t animation_frame;
    uint32_t screen_timer;
    uint32_t screenIndex;

    static const size_t timer_y_pos = 143;

    void drawTimer(SEP525_DMA_FreeRTOS& display) {
        display.fillRect(33, timer_y_pos, display.width() - 66, 17, IScreen::pb_color);

        display.setTextColor(0xFFFF);
        display.setTextSize(2);
        display.setCursor(40, 146);

        char buf[8];
        bufprint(buf, sizeof(buf), "%01d:%02d", pt.minutes, pt.seconds);
        display.println(buf);
    }

    void nextScreen(DisplayController& controller) {
        std::shared_ptr<FatFile> file(new FatFile);
        if(file->open(&controller.getScreensBaseDir(), screens[ScreenN].filelist[screenIndex], O_READ)) {
            controller.LoadImage(file, controller.getScreen().geomety().setHeight(timer_y_pos));
        }
        screenIndex = (screenIndex + 1) % screens[ScreenN].screens_count;
    }
};

///////////////////////////////////////////////////////////////////////////

class Menu1Lvl : public IMenuEntry {
public:
    Menu1Lvl(uint32_t position) : position(position) {}

    uint32_t Display(DisplayController& controller) {
        std::shared_ptr<FatFile> file(new FatFile);

        while(!file->open(&controller.getScreensBaseDir(), menuitemFiles[position], O_READ)) {
            controller.tryResetFs();
            taskYIELD();
#if 0
            if(!file->open(&controller.getScreensBaseDir(), menuitemFiles[position], O_READ))
                return 0;
#endif
        }

        return controller.LoadImage(file, controller.getScreen().geomety());
    }
    IMenuEntry* onButton(const ButtonMessage &msg) {
        switch (msg.button) {
        case LEFT_BUTTON:
            position = position > 0 ? position - 1 : sizeof(menuitemFiles) / sizeof(const char*) - 1;
            return new Menu1Lvl(position);
        case RIGHT_BUTTON:
            return new Menu1Lvl((position + 1) % (sizeof(menuitemFiles) / sizeof(const char*)));

        case SEL_BUTTON:
            if (position < sizeof(ExecScreen::screens) / sizeof(ExecScreen::TenmentScreens))
                return new ExecScreen(position);
            else
                return nullptr;
        default:
            return nullptr;
        }
    }

#if MEDICAL_EDITION
    static const char* const menuitemFiles[2];
#else
    static const char* const menuitemFiles[3];
#endif

private:
    uint32_t position;
};

///////////////////////////////////////////////////////////////////////////

IMenuEntry *IMenuEntry::getMenuRoot() {
    return new Menu1Lvl(0);
}

///////////////////////////////////////////////////////////////////////////

const char* const Menu1Lvl::menuitemFiles
#if MEDICAL_EDITION
[2] = {"M_1.565", "M_stub.565"};
#else
[3] = {"C_1.565", "C_2.565", "C_stub.565"};
#endif

///////////////////////////////////////////////////////////////////////////

static const char* const C1_picture_list[] = {"C_1_2.565", "C_1_3.565", "C_1_4.565", "C_1_5.565", "C_1_6.565", "C_1_7.565",
                                              "C_1_8.565", "C_1_9.565", "C_1_A.565", "C_1_B.565", "C_1_C.565", "C_1_D.565",
                                              "C_1_E.565"};
static const char* const C2_picture_list[] = {"C_2_2.565", "C_2_3.565", "C_2_4.565", "C_2_5.565", "C_2_6.565", "C_2_7.565", "C_2_8.565",
                                              "C_2_9.565", "C_2_A.565"};


static const char* const C1[] = {C1_picture_list[0], C1_picture_list[1], C1_picture_list[2],
                                 #if 1
                                 C1_picture_list[1], C1_picture_list[0], C1_picture_list[1],
                                 C1_picture_list[2], C1_picture_list[1], C1_picture_list[0],
                                 C1_picture_list[3], C1_picture_list[4], C1_picture_list[5], C1_picture_list[6],
                                 C1_picture_list[3], C1_picture_list[4], C1_picture_list[5], C1_picture_list[6],
                                 C1_picture_list[3], C1_picture_list[4], C1_picture_list[5], C1_picture_list[6],
                                 C1_picture_list[7], C1_picture_list[8], C1_picture_list[11], C1_picture_list[12],
                                 C1_picture_list[7], C1_picture_list[8], C1_picture_list[11], C1_picture_list[12],
                                 C1_picture_list[7], C1_picture_list[8], C1_picture_list[11], C1_picture_list[12],
                                 #endif
                                };
static const char* const C2[] = {C2_picture_list[0], C2_picture_list[1], C2_picture_list[2], C2_picture_list[3],
                                 #if 1
                                 C2_picture_list[0], C2_picture_list[1], C2_picture_list[2], C2_picture_list[3],
                                 C2_picture_list[0], C2_picture_list[1], C2_picture_list[2], C2_picture_list[3],
                                 C2_picture_list[0], C2_picture_list[1], C2_picture_list[2], C2_picture_list[3],
                                 C2_picture_list[4], C2_picture_list[5], C2_picture_list[4], C2_picture_list[5],
                                 C2_picture_list[4], C2_picture_list[5], C2_picture_list[4], C2_picture_list[5],
                                 C2_picture_list[7], C2_picture_list[8], C2_picture_list[7], C2_picture_list[8],
                                 C2_picture_list[7], C2_picture_list[8], C2_picture_list[7], C2_picture_list[8],
                                 #endif
                                };

static const char* M1[] = {"M_1_2.565", "M_1_3.565", "M_1_4.565", "M_1_5.565", "M_1_6.565"};

const ExecScreen::TenmentScreens ExecScreen::screens
#if MEDICAL_EDITION
[1] = {ExecScreen::ProcedureTime(sizeof(M1) / sizeof(const char*) * 10), sizeof(M1) / sizeof(const char*), 1, M1};
#else
[2] = {
    {ExecScreen::ProcedureTime(sizeof(C1) / sizeof(const char*)), sizeof(C1) / sizeof(const char*), 1, C1},
    {ExecScreen::ProcedureTime(sizeof(C2) / sizeof(const char*)), sizeof(C2) / sizeof(const char*), 1, C2}
};
#endif


IMenuEntry* ExecScreen::onButton(const ButtonMessage &msg) {
    if (msg.button == RETURN_BUTTON) {
        TENSController::instance()->enable(false, false);

        return new Menu1Lvl(ScreenN);
    }
    else
        return nullptr;
}

IMenuEntry *ExecScreen::playAnimation(uint32_t tick, DisplayController& controller) {
    if (!animation_frame) {
        animation_frame = FPS;

        if (screen_timer > 1)
            --screen_timer;
        else {
            nextScreen(controller);
            screen_timer = screens[ScreenN].picturesRotationPeriod();
        }

        if (pt.seconds > 0) {
            --pt.seconds;
        } else {
            if (pt.minutes > 0) {
                --pt.minutes;
                pt.seconds = 59;
            }
            else {
                TENSController::instance()->enable(false, false);
                return new Menu1Lvl(ScreenN);
            }
        }
    } else {
        --animation_frame;
        if (animation_frame == FPS / 2) {
            drawTimer(controller.getScreen());
        }
    }

    return nullptr;
}
