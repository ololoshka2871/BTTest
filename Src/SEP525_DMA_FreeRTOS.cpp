#include "SEP525_DMA_FreeRTOS.h"

#include <task.h>
#include <Pin.h>
#include <SPI.h>

#include "io_pin.h"
#include "memset16.h"
#include "DMAHalper.h"

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

void SEP525_DMA_FreeRTOS::send_fill_color(uint16_t color, uint32_t size)
{
    datastart();

    if (size > DMA_TRESHOLD) {
        SPI->transfer16(color, nullptr, size, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
    } else {
        SPI->setDataWidth16(true);
        uint16_t _b[size];
        memset16(_b, color, size);
        uint32_t DataSize = SPI->spiHandle.Init.DataSize;
        SPI->spiHandle.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Transmit(&SPI->spiHandle, (uint8_t*)_b, size, 10);
        SPI->spiHandle.Init.DataSize = DataSize;
    }

    dataend();
}

void SEP525_DMA_FreeRTOS::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    set_region(x, y, w, h);
    send_fill_color(color, w * h);
}

void SEP525_DMA_FreeRTOS::setup()
{
    const IRQn_Type list[] = {DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn};

    SEPS525_OLED::setup();

    FixSPI_DMA_IRQ_Prio(&SPI->spiHandle, configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 2);
}

void SEP525_DMA_FreeRTOS::set_region(int x, int y, int w, int h)
{
    set_region(x, y, w, h, x, y);
}

void SEP525_DMA_FreeRTOS::set_region(int x, int y, int w, int h, int start_x, int start_y)
{
    if ((w == 1) && (h == 1)){
        if (!currentRegion.contains(x, y)) {
            currentRegion.fill();
            set_region(currentRegion);
        }
        set_start_pos(start_x, start_y);
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
        send_fill_color(color, h);
    }
}

void SEP525_DMA_FreeRTOS::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    if (w) {
        set_region(x, y, w, 1);
        send_fill_color(color, w);
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

void SEP525_DMA_FreeRTOS::drawFragment(
        const uint16_t *data, size_t size_bytes, uint16_t x, uint16_t y,
        uint16_t w, uint16_t h, uint16_t start_x, uint16_t start_y)
{
    if (!size_bytes)
        return;

    set_region(x, y, w, h, start_x, start_y);
    datastart();
    SPI->transfer16((uint16_t*)data, nullptr, size_bytes, DMA_callback);
    xSemaphoreTake(mutex, portMAX_DELAY);

    if (size_bytes > DMA_TRESHOLD) {
        SPI->transfer16((uint16_t*)data, nullptr, size_bytes, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
    } else {
        SPI->setDataWidth16(true);
        uint32_t DataSize = SPI->spiHandle.Init.DataSize;
        SPI->spiHandle.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Transmit(&SPI->spiHandle, (uint8_t*)data, size_bytes, 10);
        SPI->spiHandle.Init.DataSize = DataSize;
    }

    dataend();
}

void SEP525_DMA_FreeRTOS::drawFragment(const uint16_t *data, size_t size_bytes, const Rectungle &rect, uint16_t start_x, uint16_t start_y)
{
    drawFragment(data, size_bytes, rect.x1, rect.y1, rect.width(), rect.heigth(), start_x, start_y);
}

size_t Rectungle::size() const
{
    return heigth() * width();
}

size_t Rectungle::width() const
{
    return abs(x2 - x1);
}

size_t Rectungle::heigth() const
{
    return abs(y2 - y1);
}

uint32_t Rectungle::offset2column(uint32_t offset) const {
    return offset % width();
}

uint32_t Rectungle::offset2row(uint32_t offset) const {
    return offset / width();
}

uint32_t Rectungle::offset2columnAbs(uint32_t offset) const
{
    return offset % width() + x1;
}

uint32_t Rectungle::offset2rowAbs(uint32_t offset) const
{
    return offset / width() + y1;
}
