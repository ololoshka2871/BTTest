#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H

#include <IThread.h>
#include <FreeRTOS.h>
#include <queue.h>


class SEP525_DMA_FreeRTOS;

class DisplaThread : public IThread {
private:
    DisplaThread(QueueHandle_t rx_queue);

    static DisplaThread* inst;
    SEP525_DMA_FreeRTOS* display;
    QueueHandle_t rx_queue;

public:
    static DisplaThread *instance(QueueHandle_t rx_queue);
    void setRotation(uint8_t r);
    SEP525_DMA_FreeRTOS& getDisplay() { return *display; }

    void begin();

protected:
    void run();
};

#endif // DISPLAYTHREAD_H
