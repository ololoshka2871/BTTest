#include <FreeRTOS.h>
#include <task.h>
#include <Print.h>
#include <SPI.h>
#include "AltSDFAtSPIDriver.h"
#include "SDWorker.h"
#include "io_pin.h"

// SdFat needs Serial for its interface. Provide our one
// (must be declared before including SdFat.h)
class FakePrint : public Print
{
    virtual size_t write(uint8_t c)
    {
        return 1;
    }
};

static FakePrint Serial;

#include <SdFat.h>


SDWorker::SDWorker()
{

}

void SDWorker::SDWorkerThread(void *pvParameters)
{
    IO_Pin cs(GPIOB, LL_GPIO_PIN_12);
    SPIClass spi(SPI2);
    auto driver = AltSDFAtSPIDriver::instance();
    SdFat sd(driver);

    driver->begin(&spi, &cs);
    sd.begin();

    while(1) {
        vTaskDelay(1000);
    }
}
