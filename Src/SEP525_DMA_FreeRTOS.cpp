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

void SEP525_DMA_FreeRTOS::send_fill_color(uint16_t color, uint32_t size_pixels)
{
    datastart();

    if (size_pixels > DMA_TRESHOLD / BytesPrePixel()) {
        SPI->transfer16(color, nullptr, size_pixels, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
    } else {
        SPI->setDataWidth16(true);
        uint16_t _b[size_pixels];
        memset16(_b, color, size_pixels);
        uint32_t DataSize = SPI->spiHandle.Init.DataSize;
        SPI->spiHandle.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Transmit(&SPI->spiHandle, (uint8_t*)_b, size_pixels, 10);
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
            select_region(currentRegion);
        }
        set_start_pos(start_x, start_y);
    }
}

void SEP525_DMA_FreeRTOS::set_start_pos(int x, int y)
{
    switch (rotation) { // по часовой
    case 0:
        reg(0x20, x);
        reg(0x21, y);
        break;
    case 1:
        reg(0x20, height() - y); // x
        reg(0x21, x); // y
        break;
    case 2:
        reg(0x20, width() - x);
        reg(0x21, height() - y);
        break;
    case 3:
        reg(0x20, y); // x
        reg(0x21, width() - x); // y
        break;
    }
}

void SEP525_DMA_FreeRTOS::select_region(const SEP525_DMA_FreeRTOS::Region &region)
{
    switch (rotation) { // по часовой
    case 0:
    case 2:
        reg(0x17,region.x);
        reg(0x18,region.xs - 1);
        reg(0x19,region.y);
        reg(0x1a,region.ys - 1);
        break;
    case 1:
    case 3:
        reg(0x17,region.y);
        reg(0x18,region.ys - 1);
        reg(0x19,region.x);
        reg(0x1a,region.xs - 1);
        break;
    }
}

void SEP525_DMA_FreeRTOS::set_region(const SEP525_DMA_FreeRTOS::Region &region)
{
    select_region(region);
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
    uint32_t size_16 = size_bytes / BytesPrePixel();
    if (!size_bytes)
        return;

    set_region(x, y, w, h, start_x, start_y);
    datastart();
    if (size_16 > DMA_TRESHOLD) {
        SPI->transfer16((uint16_t*)data, nullptr, size_16, DMA_callback);
        xSemaphoreTake(mutex, portMAX_DELAY);
    } else {
        SPI->setDataWidth16(true);
        uint32_t DataSize = SPI->spiHandle.Init.DataSize;
        SPI->spiHandle.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Transmit(&SPI->spiHandle, (uint8_t*)data, size_16, 10);
        SPI->spiHandle.Init.DataSize = DataSize;
    }
    dataend();
}

void SEP525_DMA_FreeRTOS::drawFragment(const uint16_t *data, size_t size_bytes, const Rectungle &rect, uint16_t start_x, uint16_t start_y)
{
    drawFragment(data, size_bytes, rect.x1(), rect.y1(),
                 rect.width(), rect.heigth(),
                 start_x, start_y);
}

void SEP525_DMA_FreeRTOS::setRotation(uint8_t r)
{
    const uint8_t transfer_method = 0x60;

    Adafruit_GFX::setRotation(r);
    currentRegion = Region(width(), height());

    // HC VC HV
    switch (rotation & 3) {
    case 0: // normal
        reg(0x16, transfer_method | 0x06);
        break;
    case 1: // -90
        reg(0x16, transfer_method | 0x03);
        break;
    case 2: // 180
        reg(0x16, transfer_method | 0x00);
        break;
    case 3: // +90
        reg(0x16, transfer_method | 0x05);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////

size_t Rectungle::width() const
{
    return abs(m_x2 - m_x1);
}

size_t Rectungle::heigth() const
{
    return abs(m_y2 - m_y1);
}

bool Rectungle::isEnd(uint32_t point_n) const
{
    return size() == point_n;
}

bool Rectungle::isPixelInside(uint32_t point_n) const
{
    return size() < point_n;
}

uint32_t Rectungle::PixelsRemaning(uint32_t position) const
{
    return size() - position;
}

uint32_t Rectungle::offset2column(uint32_t offset) const {
    return offset % width();
}

uint32_t Rectungle::offset2row(uint32_t offset) const {
    return offset / width();
}

uint32_t Rectungle::offset2columnAbs(uint32_t offset) const
{
    return offset % width() + m_x1;
}

uint32_t Rectungle::offset2rowAbs(uint32_t offset) const
{
    return offset / width() + m_y1;
}
