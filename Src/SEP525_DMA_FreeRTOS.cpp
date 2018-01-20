#include "SEP525_DMA_FreeRTOS.h"

#include <task.h>
#include <Pin.h>
#include <SPI.h>
#include "io_pin.h"

SemaphoreHandle_t SEP525_DMA_FreeRTOS::mutex;
SEP525_DMA_FreeRTOS *SEP525_DMA_FreeRTOS::inst;

SEP525_DMA_FreeRTOS::SEP525_DMA_FreeRTOS() : SEPS525_OLED(
                                                 new SPIClass(SPI1),
                                                 new IO_Pin(GPIOB, LL_GPIO_PIN_10), // RS B10
                                                 new IO_Pin(GPIOA, LL_GPIO_PIN_4), // SS
                                                 new IO_Pin(GPIOB, LL_GPIO_PIN_11), // Reset
                                                 new DummyPin()
                                                 ), currentRegion(width(), height()) {
    mutex = xSemaphoreCreateBinary();
}

void SEP525_DMA_FreeRTOS::DMA_callback() {
    xSemaphoreGiveFromISR(mutex, NULL);
}

SEP525_DMA_FreeRTOS *SEP525_DMA_FreeRTOS::instance() {
    if (!inst)
        inst = new SEP525_DMA_FreeRTOS();
    return inst;
}

void SEP525_DMA_FreeRTOS::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    set_region(x, y, w, h);
    datastart();

    uint32_t size = w * h;

    SPI->transfer16(color, nullptr, size, DMA_callback);
    xSemaphoreTake(mutex, portMAX_DELAY);

    dataend();
}

void SEP525_DMA_FreeRTOS::setup()
{
    const IRQn_Type list[] = {DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn};

    SEPS525_OLED::setup();

    for (size_t irq = 0; irq < sizeof(list) / sizeof(IRQn_Type); ++irq)
        NVIC_SetPriority(list[irq], configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1);
}

void SEP525_DMA_FreeRTOS::set_region(int x, int y, int w, int h)
{
    if ((w == 1) && (h == 1)){
        if (!currentRegion.contains(x, y)) {
            currentRegion.fill();
            set_region(currentRegion);
        }
        set_start_pos(x, y);
    } else {
        if (!currentRegion.compare(x, y, x + w, y + h)) {
            currentRegion.update(x, y, x + w, y + h);
            set_region(currentRegion);
        }
    }
}

void SEP525_DMA_FreeRTOS::select_region(const SEP525_DMA_FreeRTOS::Region &region)
{
    reg(0x17,region.x);
    reg(0x18,region.xs-1);
    reg(0x19,region.y);
    reg(0x1a,region.ys-1);
}

void SEP525_DMA_FreeRTOS::set_region(const SEP525_DMA_FreeRTOS::Region &region)
{
    // draw region
    select_region(region);

    // start position
    set_start_pos(region.x, region.y);
}

void SEP525_DMA_FreeRTOS::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    if (h) {
        set_region(x, y, 1, h);
        datastart();
        SPI->transfer16(color, nullptr, h, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
        dataend();
    }
}

void SEP525_DMA_FreeRTOS::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    if (w) {
        set_region(x, y, w, 1);
        datastart();
        SPI->transfer16(color, nullptr, w, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
        dataend();
    }
}

void SEP525_DMA_FreeRTOS::drawImage(const uint16_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint32_t image_size = w * h;
    if (!image_size)
        return;

    set_region(x, y, w, h);
    datastart();
    SPI->transfer16((uint16_t*)data, nullptr, image_size, DMA_callback);
    xSemaphoreTake(mutex, portMAX_DELAY);
    dataend();
}
