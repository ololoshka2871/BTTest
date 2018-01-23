#include <stdlib.h>
#include "SDWorker.h"
#include "DisplayThread.h"

#include "DisplayController.h"
#include "SEP525_DMA_FreeRTOS.h"

#include <SdFat.h>
#include <FatLib/FatFile.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <SerialDebugLogger.h>


#define IO_BLOCK_SIZE   512

//////////////////////////////////

template<int FragmentSize>
class RawImageLoader : public IPipeLine {
public:
    RawImageLoader(DisplayController* controller,
                   const std::shared_ptr<FatFile> &file,
                   const Rectungle& rect,
                   const uint32_t offset,
                   const uint8_t bytesPrePixel = 1)
        : file(file), position(rect), bytesPrePixel(bytesPrePixel)
    {
        this->controller = controller;
        this->offset = offset;
        read = 0;
    }

    bool processFS(SdFat& fs) {
        if (!file->isOpen())
            return false;

        volatile uint32_t points_to_read;
        uint32_t bytes_to_read;

        if (!file->seekSet(offset))
            goto fail;

        points_to_read = position.PixelsRemaning(offset / bytesPrePixel);
        if (points_to_read <= 0) {
            goto fail;
        }
        bytes_to_read = min(
                    min(file->available(), points_to_read * bytesPrePixel),
                    FragmentSize);

        read = file->read(buf, bytes_to_read);
        if (read < 0)
            goto fail;

        points_to_read = position.PixelsRemaning((offset + read) / bytesPrePixel);
        if (points_to_read == 0)
            file->close();

        return true;

        fail:
        file->close();
        return false;
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        display.drawFragment((const uint16_t*)buf, read, position,
                             position.offset2columnAbs(offset), position.offset2columnAbs(offset));
    }

protected:
    DisplayController *controller;
    std::shared_ptr<FatFile> file;
    uint32_t offset, read;
    Rectungle position;
    const uint8_t bytesPrePixel;

    uint8_t buf[FragmentSize];
};

/////////////////////////////////

DisplayController::DisplayController()
    : IThread(configMINIMAL_STACK_SIZE + 128, "Controller", tskIDLE_PRIORITY + 5)
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

void DisplayController::run()
{
    sdthread->begin();
    dispthread->begin();

    dispthread->start();
    sdthread->start();

    FatFile baseDir;
    std::shared_ptr<FatFile> file(new FatFile);

    while(1) {
        if (!baseDir.isOpen()) {
            if (!baseDir.open("/1"))
                continue;
        }
        if (!file->openNext(&baseDir, O_READ)) {
            baseDir.seekSet(0);
            continue;
        }

        uint32_t time = LoadImage(file, Rectungle(0, 0, 160, 128));
        serialDebugWrite("Load img took %d ticks\n\r", time);
        vTaskDelay(1000);
    }
}




