#ifndef TENSCONTROLLER_H
#define TENSCONTROLLER_H

#include <io_pin.h>
#include <hw_includes.h>

class TENSController
{
private:
    TENSController();

public:
    static TENSController *instance();
    void begin();
    void enable(bool enabled = true);
    void reset();
    IO_Pin Led_Pin, TENSout_Pin;
    TIM_HandleTypeDef tim2, tim3;

    static const uint16_t TENSFreq = 195;
    static const uint16_t PulseDuration = 1;

private:
    static TENSController* inst;
};

#endif // TENSCONTROLLER_H
