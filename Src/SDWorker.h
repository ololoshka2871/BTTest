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

#include <FreeRTOS.h>
#include <queue.h>

struct SDThreadArg
{
    QueueHandle_t rx_queue, tx_queue;
};

void SDWorkerThread(SDThreadArg *arg);

#endif // SDWORKER_H
