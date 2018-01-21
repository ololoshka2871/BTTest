#include <Arduino_FreeRTOS.h>

#include "Screens/ScreenManager.h"
#include "GPS/GPSThread.h"

#include "BoardInit.h"
#include "LEDThread.h"
#include "ButtonsThread.h"
#include "SDThread.h"
#include "USBDebugLogger.h"
#include "SerialDebugLogger.h"
#include "SdMscDriver.h"

#include "displaydemo.h"
#include "DisplayController.h"


int main(void)
{
	InitBoard();

    initDebugSerial();

	portENABLE_INTERRUPTS(); // To allow halt() use HAL_Delay()


    //initUSB();

    // Set up threads
    //xTaskCreate(DisplayDemo::vDisplayDemoThreadFunc, "Display Task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(DisplayController::DisplayControllerThread, "DisplayCtrl",
                configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, NULL);

	// Run scheduler and all the threads
    vTaskStartScheduler();

	// Never going to be here
	return 0;
}

// halt weak
__attribute__((weak)) void halt(uint8_t status) {
    while (1) ;
}
