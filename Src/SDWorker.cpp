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

void SDWorkerThread(SDThreadArg *arg)
{
    IO_Pin cs(GPIOB, LL_GPIO_PIN_12);
    SPIClass spi(SPI2);

    auto driver = AltSDFAtSPIDriver::instance();
    SdFat sd(driver);

    driver->begin(&spi, &cs);
    sd.begin();

    while(1) {
        IPipeLine* c;
        if (xQueueReceive(arg->rx_queue, &c, portMAX_DELAY)) {
            if (c->processFS(sd))
                while(xQueueSendToBack(arg->tx_queue, &c, portMAX_DELAY) == errQUEUE_FULL);
            else
                delete c; // error occured, remove object no piping next
        }
    }
}
