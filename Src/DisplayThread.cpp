#include <memory>
#include "DisplayController.h"

#include "SEP525_DMA_FreeRTOS.h"

#include "DisplayThread.h"

void DisplayThreadFunc(DisplayThreadArg *arg)
{
    auto display = SEP525_DMA_FreeRTOS::instance();
    display->begin();
    display->setRotation(arg->rotation);

    IPipeLine* p;
    while (1) {
        if (xQueueReceive(arg->rx_queue, &p, portMAX_DELAY)) {
            p->processDisplay(*display);
            delete p;
        }
    }
}

