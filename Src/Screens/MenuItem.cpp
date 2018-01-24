#include "MenuItem.h"

#include "SEP525_DMA_FreeRTOS.h"
#include <FatLib/FatFile.h>
#include "ButtonsThread.h"

///////////////////////////////////////////////////////////////////////////

class ExecScreen : public IMenuEntry {
public:
    ExecScreen(uint32_t parent) : ScreenN(parent) {}
    uint32_t Display(DisplayController& controller) {
        std::shared_ptr<FatFile> file(new FatFile);

        if(!file->open(&controller.getScreensBaseDir(), screens[ScreenN], O_READ))
            return 0;

        return controller.LoadImage(file, controller.getScreen().geomety());
    }
    IMenuEntry *onButton(const ButtonMessage &msg);

    static const char* screens[3];

private:
    uint32_t ScreenN;
};

///////////////////////////////////////////////////////////////////////////

class Menu1Lvl : public IMenuEntry {
public:
    Menu1Lvl(uint32_t position) : position(position) {}

    uint32_t Display(DisplayController& controller) {
        std::shared_ptr<FatFile> file(new FatFile);

        if(!file->open(&controller.getScreensBaseDir(), menuitemFiles[position], O_READ))
            return 0;

        return controller.LoadImage(file, controller.getScreen().geomety());
    }
    IMenuEntry* onButton(const ButtonMessage &msg) {
        switch (msg.button) {
        case LEFT_BUTTON:
            return new Menu1Lvl(position > 0 ? position - 1 :
                                            sizeof(menuitemFiles) / sizeof(const char*));
        case RIGHT_BUTTON:
            return new Menu1Lvl((position + 1) %
                                (sizeof(menuitemFiles) / sizeof(const char*)));

        case SEL_BUTTON:
            return new ExecScreen(position);
        default:
            return nullptr;
        }
    }

    static const char* menuitemFiles[3];

private:
    uint32_t position;
};

///////////////////////////////////////////////////////////////////////////

IMenuEntry *IMenuEntry::getMenuRoot() {
    return new Menu1Lvl(0);
}

///////////////////////////////////////////////////////////////////////////

const char* Menu1Lvl::menuitemFiles[3] = {
    "akne1.565", "Arthritic-Knee1.565", "cellulite1.565"
};

///////////////////////////////////////////////////////////////////////////

const char* ExecScreen::screens[3] = {
    "akne2.565", "Arthritic-Knee2.565", "cellulite2.565"
};

IMenuEntry* ExecScreen::onButton(const ButtonMessage &msg) {
    if (msg.button == RETURN_BUTTON)
        return new Menu1Lvl(ScreenN);
    else
        return nullptr;
}
