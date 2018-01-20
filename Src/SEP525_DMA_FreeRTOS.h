#ifndef SEP525_DMA_FREERTOS_H
#define SEP525_DMA_FREERTOS_H

#include <SEPS525_OLED.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

class SEP525_DMA_FreeRTOS : public SEPS525_OLED {
private:
    SEP525_DMA_FreeRTOS();

    static SEP525_DMA_FreeRTOS *inst;

    static SemaphoreHandle_t mutex;

    static void DMA_callback();

public:
    static SEP525_DMA_FreeRTOS *instance();

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

protected:
    void setup();
};
#endif // SEP525_DMA_FREERTOS_H
