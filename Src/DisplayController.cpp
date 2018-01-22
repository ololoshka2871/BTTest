#include <functional>
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
    RawImageDisplayer(DisplayController* controlle) {
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

    auto cmd = new RawImageDisplayer<512>(&_controller);

    while(1) {
        while(xQueueSendToBack(fs_queue, &cmd, portMAX_DELAY) == errQUEUE_FULL);
        vTaskDelay(1000);
    }
}

DisplayController::DisplayController()
{

}



