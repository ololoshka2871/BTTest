// Single pin manipulation
#include "Pin.h"


bool DummyPin::value() const
{
    return false;
}

DummyPin::Direction DummyPin::direction() const
{
    return Pin::D_INPUT;
}

void DummyPin::setDirection(DummyPin::Direction)
{
}

void DummyPin::setValue(bool)
{
}
