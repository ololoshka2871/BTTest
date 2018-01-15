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

int main(void)
{
	InitBoard();

	initDebugSerial();

	portENABLE_INTERRUPTS(); // To allow halt() use HAL_Delay()
#if 0
    // TODO: Set up threads

	// Run scheduler and all the threads
	vTaskStartScheduler();

#endif
    while (1);
	// Never going to be here
	return 0;
}
