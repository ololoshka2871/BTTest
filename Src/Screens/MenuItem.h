#ifndef MENUITEM_H
#define MENUITEM_H

#include <memory>
#include "Screen.h"

class MenuItem : public IScreen
{
private:
    MenuItem(uint32_t position);

public:
    ~MenuItem() {}

    uint32_t Display(DisplayController& controller);

    static const char* menuitemFiles[3];

    static std::unique_ptr<MenuItem> MenuRoot();

    std::unique_ptr<MenuItem> Next();
    std::unique_ptr<MenuItem> Prev();

private:
    uint32_t position;
};

#endif // MENUITEM_H
