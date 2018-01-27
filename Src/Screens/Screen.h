#ifndef SCREEN_H
#define SCREEN_H

#include "DisplayController.h"

class FatFile;

class IScreen {
public:
    virtual ~IScreen() {}
    virtual uint32_t Display(DisplayController& controller) = 0;

    static const uint16_t pb_color =
#if MEDICAL_EDITION
            0xE0C4;
#else
            0xC4EC;
#endif
};

#endif // SCREEN_H
