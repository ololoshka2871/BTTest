#include <memory>
#include "DisplayController.h"

#include "SEP525_DMA_FreeRTOS.h"

#include "DisplayThread.h"


DisplaThread *DisplaThread::inst = nullptr;

DisplaThread::DisplaThread(QueueHandle_t rx_queue)
    : IThread(configMINIMAL_STACK_SIZE * 2, "DisplayThread", tskIDLE_PRIORITY + 3),
      display(SEP525_DMA_FreeRTOS::instance()), rx_queue(rx_queue)
{
}

DisplaThread *DisplaThread::instance(QueueHandle_t rx_queue)
{
    if (!inst)
        inst = new DisplaThread(rx_queue);
    return inst;
}

void DisplaThread::setRotation(uint8_t r)
{
    display->setRotation(r);
}

void DisplaThread::begin()
{
    display->begin();
}

void DisplaThread::run()
{
    IPipeLine* p;
    while (1) {
        if (xQueueReceive(rx_queue, &p, portMAX_DELAY)) {
            p->processDisplay(*display);
            delete p;
        }
    }
}
