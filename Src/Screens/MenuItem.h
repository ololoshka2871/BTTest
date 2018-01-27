#ifndef MENUITEM_H
#define MENUITEM_H

#include <memory>
#include "Screen.h"

struct ButtonMessage;

class IMenuEntry : public IScreen {
public:
    virtual IMenuEntry* onButton(const ButtonMessage &msg) = 0;
    virtual IMenuEntry * playAnimation(uint32_t tick, DisplayController& controller) { return nullptr; }

    static IMenuEntry* getMenuRoot();
};

#endif // MENUITEM_H
