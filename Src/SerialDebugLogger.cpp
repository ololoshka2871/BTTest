#include "hw_includes.h"

#include <string.h> // strlen
#include <stdarg.h> // VA

#include "SerialDebugLogger.h"
#include "PrintUtils.h"



void initDebugSerial()
{
	// Enable clocking of corresponding periperhal
	__HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

	// Init pins in alternate function mode
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE); //TX pin
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_INPUT); //RX pin

#ifdef STM32L1
    // remap pins to UART
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_7);
#endif

	// Prepare for initialization
    LL_USART_Disable(USART1);

	// Init
    // USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t OverSampling, uint32_t BaudRate
#if defined(STM32F1)
    LL_USART_SetBaudRate(USART1, HAL_RCC_GetPCLK1Freq(), CONFIG_UART_BAUD);
#elif defined(STM32L1)
    LL_USART_SetBaudRate(USART1, HAL_RCC_GetPCLK1Freq(), LL_USART_GetOverSampling(USART1), CONFIG_UART_BAUD);
#else
#   error "Unsupported chip family"
#endif
    LL_USART_SetDataWidth(USART1, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART1, LL_USART_STOPBITS_1);
    LL_USART_SetParity(USART1, LL_USART_PARITY_NONE);
    LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);

	// Finally enable the peripheral
    LL_USART_Enable(USART1);

    serialDebugWrite("\n\n\n\n\r=================================================\n\r"
                     "CPU clock: %d Hz\n\r", HAL_RCC_GetPCLK1Freq());
}

struct SerialTarget: public PrintTarget
{
	virtual void operator()(char c)
	{
		if(c != '\0')
		{
            while (!LL_USART_IsActiveFlag_TXE(USART1))
				;

            LL_USART_TransmitData8(USART1, c);
		}
	}
};

extern "C" void serialDebugWrite(const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	SerialTarget target;
	print(target, fmt, args);
	va_end(args);
}

extern "C" void serialDebugWriteC(char c)
{
	SerialTarget target;
	target(c);
}

