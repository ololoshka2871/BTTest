
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
class RawImageDisplayer : public IPipeLine {
public:
    RawImageDisplayer(DisplayController* controller) {
        this->controller = controller;
    }

    bool processFS(SdFat& fs) {
        FatFile f;

        if (!f.open("/1/akne1.565", O_READ))
            return false;
        f.seekSet(0);
        f.read(buf, sizeof(buf));
        f.close();
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        display.drawImage((uint16_t*)buf, 0, 0, 160, 3);
    }

protected:
    DisplayController *controller;

    uint8_t buf[FragmentSize];
};

//////////////////////////////////

template<int FragmentSize>
class RawImageLoader : public IPipeLine {
public:
    RawImageLoader(DisplayController* controller,
                   const std::shared_ptr<FatFile> &file,
                   const Rectungle& rect,
                   const uint32_t offset)
        : file(file), position(rect)
    {
        this->controller = controller;
        this->offset = offset;
        read = 0;
    }

    bool processFS(SdFat& fs) {
        if (!file->isOpen())
            return false;

        if (!file->seekSet(offset))
            return false;

        read = file->read(buf, min(file->available(), FragmentSize));
        if (read < 0)
            return false;

        if (file->available() == 0)
            file->close();

        return true;
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

    uint8_t buf[FragmentSize];
};

/////////////////////////////////

void DisplayController::DisplayControllerThread(void *args)
{
    DisplayController _controller;

    // create queues
    _controller.fs_queue       = xQueueCreate(1, sizeof(IPipeLine*));
    _controller.display_queue  = xQueueCreate(1, sizeof(IPipeLine*));

    DisplayThreadArg display_thread_args = {.rx_queue = _controller.display_queue,
                                            .rotation = 0
                                           };
    SDThreadArg sd_thread_args = { .rx_queue = _controller.fs_queue,
                                   .tx_queue = _controller.display_queue
                                 };

    // start DisplayThreadFunc
    xTaskCreate((TaskFunction_t)DisplayThreadFunc, "DisplayThread",
                configMINIMAL_STACK_SIZE * 2, &display_thread_args, tskIDLE_PRIORITY + 3, NULL);
    // start SDWorkerThread
    xTaskCreate((TaskFunction_t)SDWorkerThread, "DisplayCtrl",
                configMINIMAL_STACK_SIZE * 2 + 512, &sd_thread_args, tskIDLE_PRIORITY + 3, NULL);

    FatFile baseDir;
    std::shared_ptr<FatFile> file(new FatFile);

    while(1) {
        if (!baseDir.isOpen())
            if (!baseDir.open("/1"))
                continue;
        if (!file->openNext(&baseDir, O_READ)) {
            baseDir.close();
            continue;
        }

        uint32_t time = _controller.LoadImage(file, Rectungle(0, 0, 160, 128));
        serialDebugWrite("Load img took %d ticks\n\r", time);
        vTaskDelay(1000);
    }
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
    uint32_t start = micros();
    size_t toRead = min(file->fileSize(), pos.size());

    size_t offset = 0;
    while (toRead) {
        auto cmd = new RawImageLoader<IO_BLOCK_SIZE>(this, file, pos, offset);
        while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);
        offset += IO_BLOCK_SIZE;
        toRead = toRead >= IO_BLOCK_SIZE ? toRead - IO_BLOCK_SIZE : 0;
    }
    return micros() - start;
}




