#include "AltSDFAtSPIDriver.h"

#include <SPI.h>
#include <Pin.h>
#include <task.h>

#include "DMAHalper.h"


#ifndef DMA_TRESHOLD
#define DMA_TRESHOLD 16
#endif

SemaphoreHandle_t AltSDFAtSPIDriver::mutex;
AltSDFAtSPIDriver* AltSDFAtSPIDriver::inst;

AltSDFAtSPIDriver::AltSDFAtSPIDriver()
{
    mutex = xSemaphoreCreateBinary();
}

AltSDFAtSPIDriver *AltSDFAtSPIDriver::instance()
{
    if (!inst)
        inst = new AltSDFAtSPIDriver();
    return inst;
}

void AltSDFAtSPIDriver::DMA_callback() {
    xSemaphoreGiveFromISR(mutex, NULL);
}

void AltSDFAtSPIDriver::activate()
{
    // No special activation needed
}

void AltSDFAtSPIDriver::begin(SPIClass *spi, Pin *csPin)
{
    SPI = spi;
    cs = csPin;

    SPI->begin();

    cs->setValue(HIGH);
    cs->setDirection(Pin::D_OUTPUT);

    FixSPI_DMA_IRQ_Prio(&SPI->spiHandle, configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1);
}

void AltSDFAtSPIDriver::deactivate()
{
    // No special activation needed
}

uint8_t AltSDFAtSPIDriver::receive()
{
    return SPI->transfer(0xff);
}

uint8_t AltSDFAtSPIDriver::receive(uint8_t *buf, size_t n)
{
    if (!n)
        return 0;

    if (n > DMA_TRESHOLD) {
        SPI->transfer(nullptr, buf, n, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
        return 0;
    } else {
        return HAL_SPI_Receive(&SPI->spiHandle, buf, n, 10) == HAL_OK ? 0 : 1;
    }
}

void AltSDFAtSPIDriver::send(uint8_t data)
{
    SPI->transfer(data);
}

void AltSDFAtSPIDriver::send(const uint8_t *buf, size_t n)
{
    if (!n)
        return;

    if (n > DMA_TRESHOLD) {
        SPI->transfer((uint8_t*)buf, nullptr, n, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
    } else {
        HAL_SPI_Transmit(&SPI->spiHandle, (uint8_t*)buf, n, 10);
    }
}

void AltSDFAtSPIDriver::select()
{
    cs->setValue(LOW);
}

void AltSDFAtSPIDriver::setSpiSettings(const SPISettings &spiSettings)
{
    SPI->beginTransaction(spiSettings);
}

void AltSDFAtSPIDriver::unselect()
{
    cs->setValue(HIGH);
}
