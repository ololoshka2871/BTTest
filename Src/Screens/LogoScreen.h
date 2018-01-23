#ifndef LOGOSCREEN_H
#define LOGOSCREEN_H

#include <memory>
#include "Screen.h"

class FatFile;

class LogoScreen : public IScreen
{
public:
    LogoScreen();
    ~LogoScreen() {}

    uint32_t Display(DisplayController& controller);

private:
    std::shared_ptr<FatFile> file;
};

#endif // LOGOSCREEN_H
