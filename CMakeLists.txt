CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

# Always build in release as it uses -Os
SET(CMAKE_BUILD_TYPE Release)

# All compiler related stuff is defined in the toolchain file
set(STM32_CHIP            "STM32L151CB"
    CACHE STRING "STM32 chip name")

set(F_CPU                  24000000L
    CACHE STRING "CPU Frequency")

set(F_XTAL                 8000000L
    CACHE STRING "CPU Quartz resonator fequency")

set(UART_BAUD               115200
    CACHE STRING "UART1 BaudRate")

# Using Maple's linker script that corresponds maple/stm32duino code
#SET(STM32_LINKER_SCRIPT ${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/variants/BLUEPILL/ldscript_bootloader_2000.ld)
#LINK_DIRECTORIES(${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/variants/BLUEPILL/)

# Flash offset due to bootloader
set(STM32_FLASH_ORIGIN "0x08000000"
    CACHE STRING "Flash memory start address")

SET(VECT_TAB_ADDR "0x2000")

# Load the toolchain file that uses vars above
SET(CMAKE_TOOLCHAIN_FILE cmake/gcc_stm32.cmake)
INCLUDE(cmake/halpers.cmake)

# Project definition
PROJECT(BTTest)

# Misc stuff
ENABLE_LANGUAGE(ASM)

# Handy path variables used in this script and passed to submodules
SET(${CMAKE_PROJECT_NAME}_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Libs)

STM32_GET_FAMITY_X(${STM32_CHIP} STM32_FAMILY_X)
SET(STM32_FAMILY_LONG STM32${STM32_FAMILY})

# Project wide definitions
ADD_DEFINITIONS(
        -D${STM32_FAMILY_LONG}
        -D${STM32_CHIP}
        -D${STM32_FAMILY_X}
#	-DUSE_USB_COMPOSITE
#	-DUSE_USB_MSC
        -DMCU_${STM32_CHIP}
        -DF_CPU=${F_CPU}
        -DHSE_VALUE=${F_XTAL}
	-DARDUINO=10801

)

if (${MEDICAL_EDITION})
    ADD_DEFINITIONS(-DMEDICAL_EDITION=1)
else()
    ADD_DEFINITIONS(-DMEDICAL_EDITION=0)
endif()

# Project wide include directories
INCLUDE_DIRECTORIES(
	# Arduino layer (stm32generic)
	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/cores/arduino
	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/cores/arduino/stm32
#	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/cores/arduino/usb
#	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/cores/arduino/usb/cdc # TODO: code should not call these functions directly and work via arduino layer
	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/variants/BLUEPILL

	# HAL and CMSIS
	${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/system/CMSIS
        ${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/system/${STM32_FAMILY_LONG}/CMSIS_Inc
        ${${CMAKE_PROJECT_NAME}_LIBS_DIR}/STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc
)

###################
# CONFIG
###################

SET(HW_INCLUDES_IN ${CMAKE_CURRENT_SOURCE_DIR}/hw_includes.h.in)
SET(HW_INCLUDES    ${CMAKE_CURRENT_BINARY_DIR}/hw_includes.h)

STRING(TOLOWER ${STM32_FAMILY_LONG} STM32_FAMILY_LONG_l)
configure_file(${HW_INCLUDES_IN} ${HW_INCLUDES} @ONLY)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_SUBDIRECTORY(Libs)
ADD_SUBDIRECTORY(Src)

