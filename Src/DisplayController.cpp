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
    RawImageLoader(DisplayController* controller, const char* path)
        :file(new FatFile),
          buf(new uint8_t[FragmentSize], std::default_delete<uint8_t[]>())
    {
        this->controller = controller;
        this->path = path;
        pos_r = pos_w = 0;
    }

    bool processFS(SdFat& fs) {
        if (!file->isOpen()) {
            if (!file->open(path, O_READ))
                return false;
        }
        if (!file->seekSet(pos_r)) {
            file->close();
            return false;
        }
        int read = file->read(buf.get(), min(file->available(), FragmentSize));
        if (read < 0) {
            file->close();
            return false;
        }
        pos_r += read;
        _eof = !file->available();
        if (_eof)
            file->close();
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        int16_t d_w = display.height();
        int16_t d_h = display.width();
        int16_t pos_x = pos_w % d_w;
        int16_t pos_y = pos_w / d_h;
        uint32_t toWrite = pos_r - pos_w;
        display.drawFragment((const uint16_t*)buf.get(), toWrite,
                             0, 0, d_w, d_h, pos_x, pos_y);
        pos_w = pos_r;

        controller->DisplayWriten(toWrite);
        if (!_eof) {
            auto newCMD = new RawImageLoader(*this);
            newCMD->pos_r = controller->nextOffset();
        } else {

        }
    }


protected:
    uint32_t pos_r, pos_w;
    DisplayController *controller;
    const char* path;
    std::shared_ptr<FatFile> file;
    bool _eof;
    std::shared_ptr<uint8_t[]> buf;
};

/////////////////////////////////

void DisplayController::DisplayControllerThread(void *args)
{
    DisplayController _controller;

    // create queues
    auto fs_queue       = xQueueCreate(1, sizeof(IPipeLine*));
    auto display_queue  = xQueueCreate(1, sizeof(IPipeLine*));

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

void DisplayController::DisplayWriten(size_t bytes)
{

}

uint32_t DisplayController::nextOffset() const
{

}

DisplayController::DisplayController()
{

}



