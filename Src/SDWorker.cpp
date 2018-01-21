#include <functional>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <SPI.h>
#include "AltSDFAtSPIDriver.h"
#include "SDWorker.h"
#include "io_pin.h"


FakePrint Serial;

#include <SdFat.h>

void SDWorkerThread(void *arg)
{
    auto getter = static_cast<std::function<void(QueueHandle_t&, QueueHandle_t&)>*>(arg);

    QueueHandle_t inq, outq;
    (*getter)(inq, outq);

    IO_Pin cs(GPIOB, LL_GPIO_PIN_12);
    SPIClass spi(SPI2);
    auto driver = AltSDFAtSPIDriver::instance();
    SdFat sd(driver);

    driver->begin(&spi, &cs);
    sd.begin();

    while(1) {
        FSCommand* c;
        if (xQueueReceive(inq, &c, portMAX_DELAY)) {
            if (c->processFS(sd))
                while(xQueueSendToBack(outq, &c, portMAX_DELAY) == errQUEUE_FULL);
            else
                delete c; // error occured, remove object from pipeline
        }
    }
}
