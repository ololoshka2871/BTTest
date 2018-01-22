#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <stdlib.h>
#include <stdint.h>

class SdFat;
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

    void DisplayWriten(size_t bytes);
    uint32_t nextOffset() const;

private:
    IPipeLine* tail;
};

#endif // DISPLAYCONTROLLER_H
