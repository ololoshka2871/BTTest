#ifndef SDWORKER_H
#define SDWORKER_H

class SdFat;

void SDWorkerThread(void *arg);

class FSCommand {
public:
    virtual ~FSCommand() {}
    virtual bool processFS(SdFat& fs) = 0;
};

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

#endif // SDWORKER_H
