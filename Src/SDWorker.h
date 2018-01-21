#ifndef SDWORKER_H
#define SDWORKER_H

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>

class SDWorker
{
private:
    SDWorker();

public:
    static void SDWorkerThread(void* pvParameters);
};

#endif // SDWORKER_H
