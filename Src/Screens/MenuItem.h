#ifndef MENUITEM_H
#define MENUITEM_H

#include <memory>
#include "Screen.h"

struct ButtonMessage;

class IMenuEntry : public IScreen {
public:
    virtual IMenuEntry* onButton(const ButtonMessage &msg) = 0;

    static IMenuEntry* getMenuRoot();
};

#endif // MENUITEM_H
