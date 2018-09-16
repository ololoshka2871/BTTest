# Not pushing this file down to libraries in order to keep source tree as is (not populating with extra files, such as CMakeList.txt)
#
# Below each section represents a library with its own settings

###################
# FreeRTOS
###################

SET(FREERTOS_SRC
	FreeRTOS/Arduino_FreeRTOS.c
	FreeRTOS/Arduino_FreeRTOS.h
	FreeRTOS/FreeRTOS.h
	FreeRTOS/FreeRTOSConfig.h
        #FreeRTOS/heap_1.c
        FreeRTOS/heap_4.c
	FreeRTOS/list.c
	FreeRTOS/list.h
	FreeRTOS/mpu_prototypes.h
	FreeRTOS/mpu_wrappers.h
	FreeRTOS/port.c
	FreeRTOS/portable.h
	FreeRTOS/portmacro.h
	FreeRTOS/projdefs.h
	FreeRTOS/queue.c
	FreeRTOS/queue.h
	FreeRTOS/semphr.h
	FreeRTOS/StackMacros.h
	FreeRTOS/task.h
	FreeRTOS/tasks.c
	FreeRTOS/timers.c
	FreeRTOS/timers.h
)
ADD_LIBRARY(FreeRTOS STATIC ${FREERTOS_SRC})

###################
# Pin
###################

ADD_LIBRARY(Pin STATIC
    Pin/Pin.cpp
    Pin/Pin.h
)

TARGET_INCLUDE_DIRECTORIES(Pin PRIVATE
    STM32generic/system/${STM32_FAMILY_LONG}/stm32_chip
)

###################
# STM32 generic - HAL based Arduino API implementation
###################

SET(STM32GENERIC_SRC
        STM32generic/libraries/SPI/src/SPI.cpp
        STM32generic/libraries/stm32_dma/src/stm32_dma.c

	STM32generic/cores/arduino/abi.cpp
        STM32generic/cores/arduino/Arduino.h
##	STM32generic/cores/arduino/hooks.c
##	STM32generic/cores/arduino/itoa.c
##	STM32generic/cores/arduino/itoa.h
#	STM32generic/cores/arduino/main.cpp
##	STM32generic/cores/arduino/new.cpp
##	STM32generic/cores/arduino/new.h
#	STM32generic/cores/arduino/pins_arduino.h
	STM32generic/cores/arduino/Print.cpp
	STM32generic/cores/arduino/Print.h
	STM32generic/cores/arduino/Printable.h
	STM32generic/cores/arduino/Stream.cpp
	STM32generic/cores/arduino/Stream.h
#	STM32generic/cores/arduino/syscalls.c
#	STM32generic/cores/arduino/syscalls.h
##	STM32generic/cores/arduino/wiring_private.h
        STM32generic/cores/arduino/WMath.cpp
##	STM32generic/cores/arduino/WString.cpp
##	STM32generic/cores/arduino/WString.h

#	STM32generic/cores/arduino/stm32/SerialUART.cpp
#	STM32generic/cores/arduino/stm32/SerialUART.h
#	STM32generic/cores/arduino/stm32/SerialUSB.cpp
#	STM32generic/cores/arduino/stm32/SerialUSB.h
##	STM32generic/cores/arduino/stm32/stm32_ADC.c
##	STM32generic/cores/arduino/stm32/stm32_build_defines.h
        STM32generic/cores/arduino/stm32/stm32_clock.h
##	STM32generic/cores/arduino/stm32/stm32_def.h
        STM32generic/cores/arduino/stm32/stm32_gpio.c
        STM32generic/cores/arduino/stm32/stm32_gpio.h
        STM32generic/cores/arduino/stm32/stm32_gpio_af.c
        STM32generic/cores/arduino/stm32/stm32_gpio_af.h
        STM32generic/cores/arduino/stm32/stm32_gpio_af_F0F2F3F4F7L0L1L4.c
        STM32generic/cores/arduino/stm32/stm32_gpio_af_F1.c
        STM32generic/cores/arduino/stm32/stm32_gpio_exti.c
        STM32generic/cores/arduino/stm32/stm32_init.c # SysTick_Handler THERE!
        STM32generic/cores/arduino/stm32/stm32_pin_list.c
        STM32generic/cores/arduino/stm32/stm32_PWM.c
#	STM32generic/cores/arduino/stm32/USBDevice.cpp
#	STM32generic/cores/arduino/stm32/USBDevice.h
 
##	STM32generic/cores/arduino/usb/usb_device.c
##	STM32generic/cores/arduino/usb/usb_device.h
#	STM32generic/cores/arduino/usb/usbd_conf.h
#	STM32generic/cores/arduino/usb/usbd_conf_F1.c
##	STM32generic/cores/arduino/usb/usbd_conf_F4.c
##	STM32generic/cores/arduino/usb/usbd_conf_F7.c
##	STM32generic/cores/arduino/usb/usbd_conf_L0.c
##	STM32generic/cores/arduino/usb/usbd_conf_L4.c
#	STM32generic/cores/arduino/usb/usbd_core.c
#	STM32generic/cores/arduino/usb/usbd_core.h
#	STM32generic/cores/arduino/usb/usbd_ctlreq.c
#	STM32generic/cores/arduino/usb/usbd_ctlreq.h
#	STM32generic/cores/arduino/usb/usbd_def.h
#	STM32generic/cores/arduino/usb/usbd_desc.c
#	STM32generic/cores/arduino/usb/usbd_desc.h
#	STM32generic/cores/arduino/usb/usbd_ioreq.c
#	STM32generic/cores/arduino/usb/usbd_ioreq.h
 
#	STM32generic/cores/arduino/usb/cdc/usbd_cdc.c
#	STM32generic/cores/arduino/usb/cdc/usbd_cdc.h
#	STM32generic/cores/arduino/usb/cdc/usbd_cdc_if.c
#	STM32generic/cores/arduino/usb/cdc/usbd_cdc_if.h


#	STM32generic/variants/BLUEPILL/systemclock_config.c
	STM32generic/variants/BLUEPILL/variant.h
)

ADD_LIBRARY(STM32generic STATIC ${STM32GENERIC_SRC})

TARGET_INCLUDE_DIRECTORIES(STM32generic PRIVATE
        STM32generic/system/${STM32_FAMILY_LONG}/stm32_chip
        STM32generic/libraries/stm32_dma/src
)

TARGET_COMPILE_DEFINITIONS(STM32generic PRIVATE
        -DMENU_USB_SERIAL
)

###################
# CMSIS and HAL
# 
# Note: In order not to make yet another copy of CMSIS and HAL in the source tree I'll use
#       stm32generic's one for now. It seems it is pretty fresh and match the original
###################

SET(STM32_FAMILY_LONG_lx ${STM32_FAMILY_LONG_l}xx)
STRING(TOLOWER ${STM32_FAMILY_X} STM32_FAMILY_Xl)

SET(HAL_SRC
        STM32generic/system/${STM32_FAMILY_LONG}/CMSIS_Inc/${STM32_FAMILY_Xl}.h
        STM32generic/system/${STM32_FAMILY_LONG}/CMSIS_Inc/${STM32_FAMILY_LONG_lx}.h
        STM32generic/system/${STM32_FAMILY_LONG}/CMSIS_Inc/system_${STM32_FAMILY_LONG_lx}.h
        STM32generic/system/${STM32_FAMILY_LONG}/CMSIS_Src/startup_${STM32_FAMILY_Xl}.s

        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_adc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_adc_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_can.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_can_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_cec.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_conf.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_cortex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_crc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_dac.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_dac_ex.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_def.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_dma.h
        ##### FIXME: STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_dma_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_eth.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_flash.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_flash_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_gpio.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_gpio_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_hcd.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_i2c.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_i2s.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_irda.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_iwdg.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_mmc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_nand.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_nor.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_pccard.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_pcd.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_pcd_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_pwr.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_rcc.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_rcc_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_rtc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_rtc_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_sd.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_smartcard.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_spi.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_sram.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_tim.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_tim_ex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_uart.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_usart.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_hal_wwdg.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_adc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_bus.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_cortex.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_crc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_dac.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_dma.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_exti.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_fsmc.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_gpio.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_i2c.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_iwdg.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_pwr.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_rcc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_rtc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_sdmmc.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_spi.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_system.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_tim.h
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_usart.h
        #### FIXME: STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_usb.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_utils.h
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Inc/${STM32_FAMILY_LONG_lx}_ll_wwdg.h

        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_adc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_adc_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_can.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_cec.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_cortex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_crc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_dac.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_dac_ex.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_dma.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_eth.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_flash.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_flash_ex.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_gpio.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_gpio_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_hcd.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_i2c.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_i2s.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_irda.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_iwdg.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_mmc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_nand.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_nor.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_pccard.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_pcd.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_pcd_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_pwr.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_rcc.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_rcc_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_rtc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_rtc_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_sd.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_smartcard.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_spi.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_spi_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_sram.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_tim.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_tim_ex.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_uart.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_usart.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_hal_wwdg.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_adc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_crc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_dac.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_dma.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_exti.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_fsmc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_gpio.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_i2c.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_pwr.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_rcc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_rtc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_sdmmc.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_spi.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_tim.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_usart.c
        #### FIXME: STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_usb.c
        #STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/${STM32_FAMILY_LONG_lx}_ll_utils.c
        STM32generic/system/${STM32_FAMILY_LONG}/HAL_Src/system_${STM32_FAMILY_LONG_lx}.c
)

ADD_LIBRARY(HAL STATIC ${HAL_SRC})

TARGET_INCLUDE_DIRECTORIES(HAL PRIVATE

)

TARGET_COMPILE_DEFINITIONS(HAL PRIVATE
        -DVECT_TAB_OFFSET=${VECT_TAB_ADDR}
)

SET(SPBTLE_RF_SRC
    SPBTLE-RF/src/eddystone_beacon.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/hci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_hal_aci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_gap_aci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_updater_aci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_gatt_aci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_l2cap_aci.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_utils.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/controller/bluenrg_IFR.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/hci/hci_le.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/utils/gp_timer.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/utils/ble_list.c
    SPBTLE-RF/src/STM32_BlueNRG/SimpleBlueNRG_HCI/utils/osal.c

    SPBTLE-RF/src/bluenrg_interface.c
    SPBTLE-RF/src/SPBTLE_RF.cpp
    SPBTLE-RF/src/sensor_service.cpp
    SPBTLE-RF/src/beacon_service.cpp
    )

add_library(SPBTLE_RF STATIC ${SPBTLE_RF_SRC})

target_include_directories(SPBTLE_RF PUBLIC
    SPBTLE-RF/src
    FreeRTOS
    STM32generic/libraries/SPI/src
    )

TARGET_COMPILE_DEFINITIONS(SPBTLE_RF PUBLIC
    -DBLUENRG_CONFIG
    -DBLUENRG_MS=0
)

SET(LIBS_DIR            ${CMAKE_CURRENT_SOURCE_DIR}             PARENT_SCOPE)
