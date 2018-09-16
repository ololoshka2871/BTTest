#include <Arduino_FreeRTOS.h>

#include "BoardInit.h"
#include "demo.h"

#include <IThread.h>

// запихал ардуино-функции в поток, чтобы и FreeRTOS работал, это  нужно чтобы на скорую руку запустить
// системный таймер и получать время. + еще там настраиваются приоритеры прерываний и инициализируется
// менеджмент памяти
class ArduinoTask : public IThread {
public:
    ArduinoTask() : IThread(1024) {}

protected:
    void run() override {
        _setup();
        while (1)
          _loop();
    }
} t;

int main(void) {
  InitBoard(); // тут все по старому

  portENABLE_INTERRUPTS(); // To allow halt() use HAL_Delay()

  t.start(); // запускаем единственный поток.

  vTaskStartScheduler();

  // Never going to be here
  return 0;
}

// halt weak
__attribute__((weak)) void halt(uint8_t status) {
  while (1)
    ;
}
