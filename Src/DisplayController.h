#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <stdlib.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

class SdFat;
class Rectungle;
class SEP525_DMA_FreeRTOS;

class IPipeLine {
public:
    virtual bool processFS(SdFat& fs) = 0;
    virtual void processDisplay(SEP525_DMA_FreeRTOS& display) = 0;
    virtual ~IPipeLine() {}
};

class DisplayController
{
private:
    DisplayController();

public:
    static void DisplayControllerThread(void *args);

    void LoadImage(const char* path, const Rectungle &pos);

private:
    QueueHandle_t fs_queue, display_queue;
};

#endif // DISPLAYCONTROLLER_H
