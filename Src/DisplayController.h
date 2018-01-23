#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

#include <memory>
#include <stdlib.h>
#include <stdint.h>

#include "IThread.h"
#include <queue.h>

class SdFat;
class Rectungle;
class SEP525_DMA_FreeRTOS;
class FatFile;
class SDWorker;
class DisplaThread;
class imgdata;

class IPipeLine {
public:
    virtual bool processFS(SdFat& fs) { return true; }
    virtual void processDisplay(SEP525_DMA_FreeRTOS& display) {}
    virtual ~IPipeLine() {}
};

class DisplayController : public IThread
{
public:
    DisplayController();

    static void DisplayControllerThread(void *args);

    uint32_t LoadImage(const char* path, const Rectungle &pos);
    uint32_t LoadImage(std::shared_ptr<FatFile> &file, const Rectungle &pos);
    uint32_t DrawRectungle(const Rectungle& rect, uint16_t color);
    uint32_t DrawImage(const imgdata *data, int x, int y);

private:
    QueueHandle_t fs_queue, display_queue;
    SDWorker *sdthread;
    DisplaThread *dispthread;

protected:
    void run();
};

#endif // DISPLAYCONTROLLER_H
