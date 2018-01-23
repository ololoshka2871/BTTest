#ifndef ITHREAD_H
#define ITHREAD_H

#include <FreeRTOS.h>
#include <task.h>

class IThread
{
public:
    IThread(const uint16_t stackSize = configMINIMAL_STACK_SIZE,
            const char* name = "Thread",
            const UBaseType_t priority = tskIDLE_PRIORITY + 1)
        : stackSize(stackSize), startPrio(priority), name(name) {}
    virtual void start() {
        xTaskCreate(_initiator, name, stackSize, this, startPrio, NULL);
    }

    // call this method to initialise part of thread what required what
    // shaduler allready started
    virtual void begin() {}

protected:
    static void _initiator(void* arg) {
        IThread* thread = static_cast<IThread*>(arg);
        thread->run();
        while(1);
    }

    virtual void run() = 0;

private:
    uint16_t stackSize;
    UBaseType_t startPrio;
    const char *name;
};

#endif // ITHREAD_H
