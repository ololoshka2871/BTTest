#include "io_pin.h"


IO_Pin::IO_Pin(GPIO_TypeDef *port, uint32_t pin_msk) : Pin() {
    this->port = port;
    this->pin = pin_msk;

    if (port == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if(port == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();

    LL_GPIO_SetPinOutputType(port, pin, LL_GPIO_OUTPUT_PUSHPULL);
    setDirection(D_INPUT);
}

bool IO_Pin::value() const {
    return LL_GPIO_GetPinMode(port, pin) == LL_GPIO_MODE_OUTPUT ?
                !!LL_GPIO_IsOutputPinSet(port, pin) :
                !!LL_GPIO_IsInputPinSet(port, pin);
}

Pin::Direction IO_Pin::direction() const {
    uint32_t mode = LL_GPIO_GetPinMode(port, pin);
    if (mode == LL_GPIO_MODE_OUTPUT)
        return D_OUTPUT;
    else
        switch (LL_GPIO_GetPinPull(port, pin)) {
        case LL_GPIO_PULL_UP:
            return static_cast<Pin::Direction>(D_INPUT | D_PULL_UP);
        case LL_GPIO_PULL_DOWN:
            return static_cast<Pin::Direction>(D_INPUT | D_PULL_DOWN);
        default:
            return D_INPUT;
        }
}

void IO_Pin::setDirection(Pin::Direction dir) {
    LL_GPIO_SetPinMode(port, pin, dir & D_OUTPUT ? LL_GPIO_MODE_OUTPUT : LL_GPIO_MODE_INPUT);
    if (!(dir & D_OUTPUT))
        LL_GPIO_SetPinPull(port, pin, dir & D_PULL_UP ?
                               LL_GPIO_PULL_UP : dir & D_PULL_DOWN ?
                                   LL_GPIO_PULL_UP : LL_GPIO_PULL_NO);
}

void IO_Pin::setValue(bool v) {
    if (v)
        LL_GPIO_SetOutputPin(port, pin);
    else
        LL_GPIO_ResetOutputPin(port, pin);
}
