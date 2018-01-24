#include "MenuItem.h"

#include "SEP525_DMA_FreeRTOS.h"
#include <FatLib/FatFile.h>

const char* MenuItem::menuitemFiles[3] = {
    "akne1.565", "Arthritic-Knee1.565", "cellulite1.565"
};

MenuItem::MenuItem(uint32_t position)
    : position(position)
{
}

uint32_t MenuItem::Display(DisplayController &controller) {
    std::shared_ptr<FatFile> file(new FatFile);

    if(!file->open(&controller.getScreensBaseDir(), menuitemFiles[position], O_READ))
        return 0;

    return controller.LoadImage(file, controller.getScreen().geomety());
}

std::unique_ptr<MenuItem> MenuItem::MenuRoot()
{
    return std::unique_ptr<MenuItem>(new MenuItem(0));
}

std::unique_ptr<MenuItem> MenuItem::Next()
{
    uint32_t nextpos = (position + 1) % (sizeof(menuitemFiles) / sizeof(const char*));
    return std::unique_ptr<MenuItem>(new MenuItem(nextpos));
}

std::unique_ptr<MenuItem> MenuItem::Prev()
{
    int32_t nextpos = position - 1;
    nextpos = (nextpos < 0) ? sizeof(menuitemFiles) / sizeof(const char*) : nextpos;
    return std::unique_ptr<MenuItem>(new MenuItem(nextpos));
}
