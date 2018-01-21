#ifndef ALTSDFATSPIDRIVER_H
#define ALTSDFATSPIDRIVER_H

#include <stdint.h>
#include <stdlib.h>
#include "SpiDriver/SdSpiBaseDriver.h"
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

class Pin;
class SPIClass;

class AltSDFAtSPIDriver : public SdSpiBaseDriver
{
private:
    AltSDFAtSPIDriver();

public:
    static AltSDFAtSPIDriver *instance();

    void activate();
    virtual void begin(SPIClass *spi, Pin* csPin);
    void deactivate();
    uint8_t receive();
    uint8_t receive(uint8_t* buf, size_t n);
    void send(uint8_t data);
    void send(const uint8_t* buf, size_t n);
    void select();
    void setSpiSettings(const SPISettings & spiSettings);
    void unselect();

protected:
    void begin(uint8_t chipSelectPin = 0) {}

private:
    Pin *cs;
    SPIClass* SPI;
    static AltSDFAtSPIDriver* inst;

    static void DMA_callback();
    static SemaphoreHandle_t mutex;
};

#endif // ALTSDFATSPIDRIVER_H
