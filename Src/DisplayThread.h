#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H

#include <FreeRTOS.h>
#include <queue.h>


struct DisplayThreadArg
{
    QueueHandle_t rx_queue;
    uint8_t rotation;
};

void DisplayThreadFunc(DisplayThreadArg *arg);

#endif // DISPLAYTHREAD_H
