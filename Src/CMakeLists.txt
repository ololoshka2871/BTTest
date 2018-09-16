# Build rules for GPS logger target.
# App specific compiler/linker settings are also defined here

SET(SOURCE_FILES
    main.cpp
    SPBTLE_SensorDemo.cpp
    BoardInit.cpp
    IdleThread.cpp
    DMAHalper.cpp
    hardfault.cpp
    cpp_new_hook.cpp
)

INCLUDE_DIRECTORIES(
    .
    ${LIBS_DIR}/STM32generic/libraries/FreeRTOS/src
    ${LIBS_DIR}/STM32generic/libraries/SPI/src
    ${LIBS_DIR}/STM32generic/libraries/stm32_dma/src
)


# Do not link to libc or newlib-nano - we are not using anything from that
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nosys.specs")

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME}
    FreeRTOS
        -Wl,--whole-archive
        SPBTLE_RF
        STM32generic
        HAL
        -Wl,--no-whole-archive
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
