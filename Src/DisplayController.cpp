#include <stdlib.h>
#include "SDWorker.h"
#include "DisplayThread.h"

#include "DisplayController.h"
#include "SEP525_DMA_FreeRTOS.h"
#include "RawImageLoader.h"

#include "Screens/LogoScreen.h"
#include "Screens/SelfTestScreen.h"

#include <SdFat.h>
#include <FatLib/FatFile.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <SerialDebugLogger.h>


#define IO_BLOCK_SIZE   512

//////////////////////////////////

class DrawRectCMD : public IPipeLine {
public:
    DrawRectCMD(const Rectungle& rect, uint16_t color)
        : rect(rect), color(color)
    {
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        for (int i = 0; i < 10; ++i) {
            display.drawPixel(i, i, color);
        }
        //display.drawRect(rect.x1(), rect.y1(), rect.width(), rect.heigth(), color);
    }

private:
    Rectungle rect;
    uint16_t color;
};

/////////////////////////////////


#include "IMGData.h"

class imgdisplayer : public IPipeLine {
public:
    imgdisplayer(const imgdata* data, int x, int y)
        :x(x), y(y), data(data)
    {
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        display.drawImage((const uint16_t *)data->pixel_data, x, y, data->width, data->height);
    }

private:
    int x, y;
    const imgdata* data;
};

/////////////////////////////////

DisplayController::DisplayController()
    : IThread(configMINIMAL_STACK_SIZE + 128, "Controller", tskIDLE_PRIORITY + 5), screensBase(new FatFile)
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
    const uint8_t bpp = dispthread->getDisplay().BytesPrePixel();
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

uint32_t DisplayController::DrawRectungle(const Rectungle &rect, uint16_t color)
{
    const uint32_t start = micros();

    auto cmd = new DrawRectCMD(rect, color);
    while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);

    return micros() - start;
}

uint32_t DisplayController::DrawImage(const imgdata* data, int x, int y)
{
    const uint32_t start = micros();

    auto cmd = new imgdisplayer(data, x, y);
    while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);

    return micros() - start;
}

FatFile &DisplayController::getScreensBaseDir() const
{
    return *screensBase;
}

SEP525_DMA_FreeRTOS& DisplayController::getScreen() const {
    return dispthread->getDisplay();
}

void DisplayController::run()
{
    sdthread->begin();
    dispthread->begin();

    dispthread->start();
    sdthread->start();

    dispthread->getDisplay().setRotation(1);

    std::shared_ptr<FatFile> file(new FatFile);

    screensBase->open("/1");

    while(1) {
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
            vTaskDelay(100);
            getScreen().fillScreen(0xffff);
            vTaskDelay(10000);
        }

        vTaskDelay(500);
    }
}

//////////////////////////////////



