#ifndef DISPLAYCONTROLLER_H
#define DISPLAYCONTROLLER_H

class DisplayController
{
private:
    DisplayController();

public:
    static void DisplayControllerThread(void *args);
};

#endif // DISPLAYCONTROLLER_H
