#include "LogoScreen.h"

#include "SEP525_DMA_FreeRTOS.h"
#include <FatLib/FatFile.h>


LogoScreen::LogoScreen()
    : file(new FatFile)
{
}

uint32_t LogoScreen::Display(DisplayController& controller)
{
    if (!file->open(&controller.getScreensBaseDir(), "Logo_red.565", O_READ))
        return 0;

    return controller.LoadImage(file, controller.getScreen().geomety());
}
