#ifndef IO_PIN_H
#define IO_PIN_H

#include <Pin.h>
#include <stdint.h>
#include "hw_includes.h"

class IO_Pin : public Pin {
public:
    IO_Pin(GPIO_TypeDef *port, uint32_t pin_msk);

    bool value() const;

    Direction direction() const;

    void setDirection(Direction dir);

    void setValue(bool v);
private:
    GPIO_TypeDef *port;
    uint32_t pin;
};

#endif // IO_PIN_H
