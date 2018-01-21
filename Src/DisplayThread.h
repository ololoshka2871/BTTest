#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H

class SEP525_DMA_FreeRTOS;

void DisplayThreadFunc(void *pvParameters);

class DispayCommand {
public:
    virtual ~DispayCommand() {}
    virtual void processDisplay(SEP525_DMA_FreeRTOS& display) = 0;
};

#endif // DISPLAYTHREAD_H
