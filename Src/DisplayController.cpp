#include <stdlib.h>
#include "SDWorker.h"
#include "DisplayThread.h"

#include "DisplayController.h"
#include "SEP525_DMA_FreeRTOS.h"
#include "RawImageLoader.h"

#include "Screens/LogoScreen.h"
#include "Screens/SelfTestScreen.h"
#include "Screens/MenuItem.h"

#include "ButtonsThread.h"
#include "TENSController.h"

#include <SdFat.h>
#include <FatLib/FatFile.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <SerialDebugLogger.h>


#define IO_BLOCK_SIZE   512

//////////////////////////////////

DisplayController::DisplayController()
    : IThread(configMINIMAL_STACK_SIZE * 3, "Controller", tskIDLE_PRIORITY + 5), screensBase(new FatFile)
{
    fs_queue       = xQueueCreate(1, sizeof(IPipeLine*));
    display_queue  = xQueueCreate(1, sizeof(IPipeLine*));

    sdthread = SDWorker::instance(fs_queue, display_queue);
    dispthread = DisplaThread::instance(display_queue);
}

uint32_t DisplayController::LoadImage(const char *path, const Rectungle& pos)
{    
    std::shared_ptr<FatFile> file(new FatFile);
    if (!file->open(path, O_READ))
        return 0;

    return LoadImage(file, pos);
}

uint32_t DisplayController::LoadImage(std::shared_ptr<FatFile> &file, const Rectungle &pos)
{
    const uint32_t start = micros();
    const uint8_t bpp = getScreen().BytesPrePixel();
    size_t toRead = min(file->fileSize(), pos.size() * bpp);

    size_t offset = 0;
    while (toRead) {
        auto cmd = new RawImageLoader<IO_BLOCK_SIZE>(this, file, pos, offset, bpp);
        while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);
        offset += IO_BLOCK_SIZE;
        toRead = toRead >= IO_BLOCK_SIZE ? toRead - IO_BLOCK_SIZE : 0;
    }
    return micros() - start;
}

FatFile &DisplayController::getScreensBaseDir() const
{
    return *screensBase;
}

SEP525_DMA_FreeRTOS& DisplayController::getScreen() const {
    return dispthread->getDisplay();
}

void DisplayController::tryResetFs()
{
    screensBase->close();
    sdthread->reset();
    screensBase->open("/1");
}

void DisplayController::run()
{
    sdthread->begin();
    dispthread->begin();

    dispthread->start();
    sdthread->start();

    dispthread->getDisplay().setRotation(1);

    screensBase->open("/1");

    IO_Pin Is_dev_powered_on(GPIOA, GPIO_PIN_8);
    Is_dev_powered_on.setDirection(Pin::D_OUTPUT);
    Is_dev_powered_on.setValue(LOW);

    ButtonMessage msg;
    while(1) {
        while (1) {
            if (waitForButtonMessage(&msg, portMAX_DELAY)) {
                if ((msg.button == POWER_BUTTON) &&
                        (msg.event == BUTTON_LONG_PRESS) ||
                        (msg.event == BUTTON_VERY_LONG_PRESS)) {
                    getScreen().Backlight(true);
                    break;
                }
            }
        }
        Is_dev_powered_on.setValue(HIGH);

#if 1
        {
            // display logo
            LogoScreen _logo;
            _logo.Display(*this);
            vTaskDelay(3000);
        }

        {
            // self test
            SelfTestScreen st;
            st.Display(*this);
            vTaskDelay(500);
        }
#endif
        {
            while (waitForButtonMessage(&msg, 0) == pdTRUE); // consume all button events

            std::unique_ptr<IMenuEntry> currentMenuEntry(IMenuEntry::getMenuRoot());

            const uint32_t animation_tick = 10;

            currentMenuEntry->Display(*this);
            while (1) {
                if (waitForButtonMessage(&msg, animation_tick)) {
                    if ((msg.button == POWER_BUTTON) &&
                            (msg.event == BUTTON_LONG_PRESS) ||
                            (msg.event == BUTTON_VERY_LONG_PRESS))
                        break;
                    IMenuEntry * newEntry = currentMenuEntry->onButton(msg);
                    if (newEntry) {
                        currentMenuEntry.reset(newEntry);
                        currentMenuEntry->Display(*this);
                    }
                } else {
                    IMenuEntry * newEntry = currentMenuEntry->playAnimation(animation_tick, *this);
                    if (newEntry) {
                        currentMenuEntry.reset(newEntry);
                        currentMenuEntry->Display(*this);
                    }
                }
            }
            TENSController::instance()->enable(false, false);

            // display off (power down)
            getScreen().Backlight(false);
            Is_dev_powered_on.setValue(LOW);
        }

        vTaskDelay(500);
    }
}

//////////////////////////////////



