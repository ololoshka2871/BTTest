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

void testtaskvoid(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(2000);
    }
}

int main(void)
{
	InitBoard();

	initDebugSerial();

	portENABLE_INTERRUPTS(); // To allow halt() use HAL_Delay()

    // Set up threads
    xTaskCreate(testtaskvoid, "Test Thread", configMINIMAL_STACK_SIZE * 2, (void*)0x12345678, tskIDLE_PRIORITY + 1, NULL);


	// Run scheduler and all the threads
	vTaskStartScheduler();

	// Never going to be here
	return 0;
}
