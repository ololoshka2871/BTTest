#include <functional>
#include <memory>
#include "SDWorker.h"
#include "DisplayThread.h"

#include "DisplayController.h"
#include "SEP525_DMA_FreeRTOS.h"

#include <SdFat.h>
#include <FatLib/FatFile.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

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
                   const uint32_t offset)
        : file(file)
    {
        this->controller = controller;
        this->offset = offset;
    }

    bool processFS(SdFat& fs) {

    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {

    }

protected:
    DisplayController *controller;
    std::shared_ptr<FatFile> file;
    uint32_t offset;

    uint8_t buf[FragmentSize];
};

/////////////////////////////////

void DisplayController::DisplayControllerThread(void *args)
{
    DisplayController _controller;

    // create queues
    _controller.fs_queue       = xQueueCreate(1, sizeof(IPipeLine*));
    _controller.display_queue  = xQueueCreate(1, sizeof(IPipeLine*));

    std::function<void(QueueHandle_t&, QueueHandle_t&)> fs_queue_getter =
            [=](QueueHandle_t &r_fs_queue, QueueHandle_t &r_display_queue)
    { r_fs_queue = fs_queue; r_display_queue = display_queue; };
    std::function<void(QueueHandle_t&)> display_queue_retter =
            [=](QueueHandle_t &r_display_queue)
    { r_display_queue = display_queue; };

    // start DisplayThreadFunc
    xTaskCreate(DisplayThreadFunc, "DisplayThread",
                configMINIMAL_STACK_SIZE * 2, &display_queue_retter, tskIDLE_PRIORITY + 3, NULL);
    // start SDWorkerThread
    xTaskCreate(SDWorkerThread, "DisplayCtrl",
                configMINIMAL_STACK_SIZE * 2 + 512, &fs_queue_getter, tskIDLE_PRIORITY + 3, NULL);

    auto cmd = new RawImageLoader<512>(&_controller, "/1/akne1.565");

    while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);
    while(1) {
        vTaskDelay(1000);
    }
}

void DisplayController::LoadImage(const char *path, const Rectungle& pos)
{
    std::shared_ptr<FatFile> file(new FatFile);
    if (!file->open(path, O_READ))
        return;

    size_t toRead = min(file->fileSize(), pos.size());

    size_t offset = 0;
    while (toRead) {
        auto cmd = new RawImageLoader<512>(this, file, offset);
        while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);
        offset += 512;
        toRead -= 512;
    }
}




