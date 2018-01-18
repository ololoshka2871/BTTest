# Build rules for GPS logger target.
# App specific compiler/linker settings are also defined here

SET(SOURCE_FILES
	# Screens and screen management stuff
#	Screens/AltitudeScreen.cpp
#	Screens/AltitudeScreen.h
#	Screens/CurrentPositionScreen.cpp
#	Screens/CurrentPositionScreen.h
#	Screens/CurrentTimeScreen.cpp
#	Screens/CurrentTimeScreen.h
#	Screens/DebugScreen.cpp
#	Screens/DebugScreen.h
#	Screens/DisplayDriver.cpp
#	Screens/DisplayDriver.h
#	Screens/MemDumpScreen.cpp
#	Screens/MemDumpScreen.h
#	Screens/OdometerActionScreen.cpp
#	Screens/OdometerActionScreen.h
#	Screens/OdometerScreen.cpp
#	Screens/OdometerScreen.h
#	Screens/ParentScreen.cpp
#	Screens/ParentScreen.h
#	Screens/SatellitesScreen.cpp
#	Screens/SatellitesScreen.h
#	Screens/Screen.cpp
#	Screens/Screen.h
#       Screens/ScreenManager.cpp
#       Screens/ScreenManager.h
#	Screens/SelectorScreen.cpp
#	Screens/SelectorScreen.h
#	Screens/SettingsGroupScreen.cpp
#	Screens/SettingsGroupScreen.h
#	Screens/SpeedScreen.cpp
#	Screens/SpeedScreen.h
#	Screens/TimeZoneScreen.cpp
#	Screens/TimeZoneScreen.h

#	8x12Font.cpp
#	8x12Font.h
        BoardInit.cpp
        BoardInit.h
#	ButtonsThread.cpp
#	ButtonsThread.h
##	FontTest.cpp
##	FontTest.h
        FreeRTOSHelpers.h
        main.cpp
        IdleThread.cpp
        IdleThread.h
#       LEDThread.cpp
#       LEDThread.h
        PrintUtils.cpp
        PrintUtils.h
#	SdFatSPIDriver.cpp
#	SdFatSPIDriver.h
#       SdMscDriver.cpp
#       SdMscDriver.h
#       SDThread.cpp
#       SDThread.h
        SerialDebugLogger.cpp
        SerialDebugLogger.h
#	TimeFont.cpp
#	TimeFont.h
#       USBDebugLogger.cpp
#       USBDebugLogger.h
        hardfault.cpp

        displaydemo.h
        displaydemo.cpp
)

INCLUDE_DIRECTORIES(
	.
        ${LIBS_DIR}/AdafruitGFX
        ${LIBS_DIR}/SEPS525-OLED-Library
        ${LIBS_DIR}/Pin
        ${LIBS_DIR}/SdFat
        ${LIBS_DIR}/STM32generic/libraries/SPI/src
        ${LIBS_DIR}/STM32generic/libraries/FreeRTOS/src
        ${LIBS_DIR}/USB/Core
        ${LIBS_DIR}/USB/CDC
        ${LIBS_DIR}/USB/MSC
        ${LIBS_DIR}/USB/MSC_CDC
)


# Do not link to libc or newlib-nano - we are not using anything from that
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nosys.specs")

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME}
	FreeRTOS
	SDFAT
        SEPS525_GFX
	AdafruitGFX
        Pin
	USB
	STM32generic
	HAL
	m
)

TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PRIVATE
	-DENABLE_ARDUINO_FEATURES=0	#Disable SdFat stuff
)

STM32_GENERATE_MAP_FILE(${CMAKE_PROJECT_NAME})
STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_PRINT_SIZE_OF_TARGETS(${CMAKE_PROJECT_NAME})

# Additional handy targets
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})
STM32_ADD_DUMP_TARGET(${CMAKE_PROJECT_NAME})
