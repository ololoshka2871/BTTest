#include "LogoScreen.h"

#include "SEP525_DMA_FreeRTOS.h"
#include <FatLib/FatFile.h>

#if MEDICAL_EDITION
#define LOGO_FILE "Logo_red.565"
#else
#define LOGO_FILE "Logo_yellow.565"
#endif


LogoScreen::LogoScreen()
    : file(new FatFile)
{
}

uint32_t LogoScreen::Display(DisplayController& controller)
{
    if (!file->open(&controller.getScreensBaseDir(), LOGO_FILE, O_READ))
        return 0;

    return controller.LoadImage(file, controller.getScreen().geomety());
}
