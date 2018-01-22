#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

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
};

#endif // DISPLAYCONTROLLER_H
