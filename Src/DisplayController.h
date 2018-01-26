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
    FatFile &getScreensBaseDir() const;
    SEP525_DMA_FreeRTOS &getScreen() const;
    void tryResetFs();

private:
    QueueHandle_t fs_queue, display_queue;
    SDWorker *sdthread;
    DisplaThread *dispthread;
    std::unique_ptr<FatFile> screensBase;

protected:
    void run();
};

#endif // DISPLAYCONTROLLER_H
