#include <functional>
#include <FreeRTOS.h>
#include <task.h>
#include <SPI.h>
#include "AltSDFAtSPIDriver.h"
#include "SDWorker.h"
#include "io_pin.h"
#include <DisplayController.h>

FakePrint Serial;

#include <SdFat.h>

SDWorker* SDWorker::inst = nullptr;

SDWorker::SDWorker(QueueHandle_t rx_queue, QueueHandle_t tx_queue)
    : IThread(configMINIMAL_STACK_SIZE * 3, "SDWorker"), rx_queue(rx_queue), tx_queue(tx_queue)
{
    cs = new IO_Pin(GPIOB, LL_GPIO_PIN_12);
    spi = new SPIClass(SPI2);
}

SDWorker *SDWorker::instance(QueueHandle_t rx_queue, QueueHandle_t tx_queue)
{
    if (!inst)
        inst = new SDWorker(rx_queue, tx_queue);
    return inst;
}

void SDWorker::reset()
{
    sd->cardBegin();
}

void SDWorker::begin()
{
    auto driver = AltSDFAtSPIDriver::instance();
    sd = new SdFat(driver);

    driver->begin(spi, cs);
    sd->begin();
}

void SDWorker::run()
{
    while(1) {
        IPipeLine* c;
        if (xQueueReceive(rx_queue, &c, portMAX_DELAY) == pdTRUE) {
            if (c->processFS(*sd))
                while(xQueueSendToBack(tx_queue, &c, portMAX_DELAY) == errQUEUE_FULL);
            else
                delete c; // error occured, remove object no piping next
        }
    }
}

