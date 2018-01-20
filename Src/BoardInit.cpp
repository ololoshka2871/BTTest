#include "hw_includes.h"

#include "BoardInit.h"

#include <Arduino_FreeRTOS.h>

#define USB_CLK_TARGET  48000000U

// Libc requires _init() to perform some initialization before global constructors run.
// I would love if this symbol is defined as weak in newlib-nano (libc), but it is not.
extern "C" __attribute__((weak)) void _init(void)  {}

#if defined(STM32F1)
// Need c++14! (not working as planned!)

struct sDiv_Mul {
    uint32_t mul, div;
    void runtime_fallback(bool x) {};
    constexpr sDiv_Mul(int32_t m = -1, int32_t d = -1): mul(m), div(d) {
        if((m < 0) || (d < 0))
            runtime_fallback(false);
    }
};

constexpr sDiv_Mul calc_pll_div_mul(const uint32_t div_mul) {
    const uint32_t muls[] = {RCC_PLL_MUL2, RCC_PLL_MUL3, RCC_PLL_MUL4, RCC_PLL_MUL5, RCC_PLL_MUL6, RCC_PLL_MUL7,
                        RCC_PLL_MUL8, RCC_PLL_MUL9, RCC_PLL_MUL10, RCC_PLL_MUL11, RCC_PLL_MUL12, RCC_PLL_MUL13,
                        RCC_PLL_MUL14, RCC_PLL_MUL15, RCC_PLL_MUL16};
    const uint32_t divs[] = {RCC_HSE_PREDIV_DIV1,
                             RCC_HSE_PREDIV_DIV2,
#if defined(STM32F105xC) || defined(STM32F107xC) || defined(STM32F100xB) || defined(STM32F100xE)
                             RCC_HSE_PREDIV_DIV3, RCC_HSE_PREDIV_DIV4,
                             RCC_HSE_PREDIV_DIV5, RCC_HSE_PREDIV_DIV6, RCC_HSE_PREDIV_DIV7, RCC_HSE_PREDIV_DIV8,
                             RCC_HSE_PREDIV_DIV9, RCC_HSE_PREDIV_DIV10, RCC_HSE_PREDIV_DIV11, RCC_HSE_PREDIV_DIV122,
                             RCC_HSE_PREDIV_DIV13, RCC_HSE_PREDIV_DIV14, RCC_HSE_PREDIV_DIV15, RCC_HSE_PREDIV_DIV16
#endif
                             };
    for (uint32_t mul = 2; mul < sizeof(muls) / sizeof(uint32_t) + 2; ++mul)
        for(uint32_t div = 1; div < sizeof(divs) / sizeof(uint32_t) + 1; ++div)
            if (mul / div == div_mul) {
                return sDiv_Mul(muls[mul - 2], divs[div - 1]);
            }
    return sDiv_Mul();
}
#endif

void InitOSC() {
    RCC_OscInitTypeDef RCC_OscInitStruct;
#if defined(STM32F1)
    // HSE_VALUE -> PREDIV1 ->  PLLMUL * 2 -> USB prescaler => USB_CLK_TARGET
    //                   `-> PLLMUL => F_CPU

    // PREDIV1 / PLLMUL = F_CPU / HSE_VALUE
    // USB prescaler = F_CPU * 2 / USB_CLK_TARGET

#   define __USB_PRE   (F_CPU * 2 / USB_CLK_TARGET)
#   if __USB_PRE == 2
#       define _USB_PRE    RCC_USBCLKSOURCE_PLL
#   elif __USB_PRE == 3
#       define _USB_PRE    RCC_USBCLKSOURCE_PLL_DIV1_5
#   else
#       error "Correct value of RCC_USBCLKSOURCE_PLL_DIV not found, check F_CPU value"
#   endif

    // Set up USB clock
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = _USB_PRE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    constexpr auto div_mul = calc_pll_div_mul(F_CPU / HSE_VALUE);
    RCC_OscInitStruct.HSEPredivValue = div_mul.div;
    RCC_OscInitStruct.PLL.PLLMUL = div_mul.mul;

#elif defined(STM32L1)
    // HSE_VALUE -> PLLMUL =>  USB_CLK_TARGET
    //                  `-> PLLDIV => F_CPU

    // PLLMUL = USB_CLK_TARGET / HSE_VALUE
    // PLLDIV = USB_CLK_TARGET / F_CPU

#   define __PLLMUL     (USB_CLK_TARGET / HSE_VALUE)
#   if __PLLMUL == 3
#       define _PLLMUL  RCC_PLL_MUL3
#   elif __PLLMUL == 4
#       define _PLLMUL  RCC_PLL_MUL4
#   elif __PLLMUL == 6
#       define _PLLMUL  RCC_PLL_MUL6
#   elif __PLLMUL == 8
#       define _PLLMUL  RCC_PLL_MUL8
#   elif __PLLMUL == 12
#       define _PLLMUL  RCC_PLL_MUL12
#   elif __PLLMUL == 16
#       define _PLLMUL  RCC_PLL_MUL16
#   elif __PLLMUL == 24
#       define _PLLMUL  RCC_PLL_MUL24
#   elif __PLLMUL == 32
#       define _PLLMUL  RCC_PLL_MUL32
#   elif __PLLMUL == 48
#       define _PLLMUL  RCC_PLL_MUL48
#   else
#       error "Correct value of RCC_PLL_MUL not found, check HSE_VALUE value"
#   endif

#   define __PLLDIV     (USB_CLK_TARGET / F_CPU)
#   if __PLLDIV == 2
#       define _PLLDIV  RCC_PLL_DIV2
#   elif _PLLDIV == 3
#       define _PLLMUL  RCC_PLL_DIV3
#   elif _PLLDIV == 4
#       define _PLLMUL  RCC_PLL_DIV4
#   else
#       error "Correct value of RCC_PLL_DIV not found, check F_CPU value"
#   endif

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = _PLLMUL;
    RCC_OscInitStruct.PLL.PLLDIV = _PLLDIV;
#else
#error "Unsupported CPU family"
#endif
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
}

void Configure_AHB_Clocks() {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

#if defined(STM32F1)
    RCC_ClkInitStruct.APB1CLKDivider = F_CPU > 36000000U ? RCC_HCLK_DIV2 : RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
#elif defined(STM32L1)
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
    //__HAL_FLASH_ACC64_ENABLE();             // enable 64-bit flas access
#else
#error "Unsupported CPU family"
#endif
}

// Set up board clocks
void SystemClock_Config(void)
{
    InitOSC();
    Configure_AHB_Clocks();

	// Set up SysTTick to 1 ms
	// TODO: Do we really need this? SysTick is initialized multiple times in HAL
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	// SysTick_IRQn interrupt configuration - setting SysTick as lower priority to satisfy FreeRTOS requirements
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void SetupInterruptPrioritys() {
#if defined(STM32L1)
    const IRQn_Type list[] = {DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, DMA1_Channel4_IRQn,
                             DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel7_IRQn};
    for (auto irq = 0; irq < sizeof(list) / sizeof(IRQn_Type); ++irq)
        NVIC_SetPriority(list[irq], 90);
#else
#error "SETUP interrupt prioritys first!"
#endif
}

void InitBoard()
{
    // __NVIC_PRIO_BITS - how many prioruty bits avalable
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	// Initialize board and HAL
	HAL_Init();
	SystemClock_Config();
    SetupInterruptPrioritys();
}

extern "C"
void vApplicationStackOverflowHook(xTaskHandle *pxTask,
								   signed char *pcTaskName)
{
	/* This function will get called if a task overflows its stack.
	 * If the parameters are corrupt then inspect pxCurrentTCB to find
	 * which was the offending task. */

	(void) pxTask;
	(void) pcTaskName;

	while (1)
		;
}

extern "C" void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.*/
	//taskDISABLE_INTERRUPTS();
	for( ;; );
}

#ifdef STM32L1
extern "C" uint32_t HAL_PWREx_GetVoltageRange(void) {
    return PWR->CR & PWR_CR_VOS;
}
#endif
