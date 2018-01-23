#ifndef SCREEN_H
#define SCREEN_H

#include "DisplayController.h"

class FatFile;

class IScreen {
public:
    virtual ~IScreen() {}
    virtual uint32_t Display(DisplayController& controller) = 0;
};

#endif // SCREEN_H
