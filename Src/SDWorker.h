#ifndef SDWORKER_H
#define SDWORKER_H

// FIXME: ВРЕМЯНКА
// SdFat needs Serial for its interface. Provide our one
// (must be declared before including SdFat.h)
#include <Print.h>

class FakePrint : public Print
{
    virtual size_t write(uint8_t c)
    {
        return 1;
    }
};

extern FakePrint Serial;

#include "IThread.h"
#include <queue.h>

class Pin;
class SPIClass;
class SdFat;

class SDWorker : public IThread {
private:
    SDWorker(QueueHandle_t rx_queue, QueueHandle_t tx_queue);

    Pin* cs;
    SPIClass* spi;
    SdFat *sd;

    QueueHandle_t rx_queue, tx_queue;

    static SDWorker* inst;

public:
    static SDWorker *instance(QueueHandle_t rx_queue, QueueHandle_t tx_queue);
    void reset();

    // call only after shaduler was started!
    void begin();

protected:
    void run();
};

#endif // SDWORKER_H
