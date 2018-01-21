#include <FreeRTOS.h>
#include <queue.h>
#include <memory>
#include <functional>

#include "SEP525_DMA_FreeRTOS.h"

#include "DisplayThread.h"

void DisplayThreadFunc(void* arg)
{
    auto getter = static_cast<std::function<void(QueueHandle_t&)>*>(arg);

    QueueHandle_t commandQueue;
    (*getter)(commandQueue);

    auto display = SEP525_DMA_FreeRTOS::instance();
    display->begin();

    while (1) {
        DispayCommand* p;
        if (xQueueReceive(commandQueue, &p, portMAX_DELAY)) {
            p->processDisplay(*display);
        }
    }
}

