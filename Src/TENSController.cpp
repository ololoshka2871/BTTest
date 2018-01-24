#include "TENSController.h"
#include <FreeRTOS.h>

#include <assert.h>


TENSController* TENSController::inst = nullptr;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    if(tim_baseHandle->Instance==TIM2)
    {
        /* USER CODE BEGIN TIM2_MspInit 0 */

        /* USER CODE END TIM2_MspInit 0 */
        /* TIM2 clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();

        /* TIM2 interrupt Init */
        NVIC_SetPriority(TIM2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 3);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        /* USER CODE BEGIN TIM2_MspInit 1 */

        /* USER CODE END TIM2_MspInit 1 */
    }
    else if(tim_baseHandle->Instance==TIM3)
    {
        /* USER CODE BEGIN TIM3_MspInit 0 */

        /* USER CODE END TIM3_MspInit 0 */
        /* TIM3 clock enable */
        __HAL_RCC_TIM3_CLK_ENABLE();

        /* TIM3 interrupt Init */
        NVIC_SetPriority(TIM2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 3);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        /* USER CODE BEGIN TIM3_MspInit 1 */

        /* USER CODE END TIM3_MspInit 1 */
    }
}

// костыль
extern void (*pwm_callback_func)();


extern "C" void _TIM2_IRQHandler() {
    static uint16_t LedCounter = 0;

    if (LedCounter >= 10)
    {
        auto pin = TENSController::instance()->Led_Pin;
        pin.setValue(!pin.value());
        LedCounter = 0;
    }
    ++LedCounter;
    __HAL_TIM_CLEAR_FLAG(&TENSController::instance()->tim2, TIM_FLAG_UPDATE);
}

extern "C" void TIM3_IRQHandler() {
    static uint16_t TENSCounter = 0;

    if (TENSCounter >= TENSController::TENSFreq)
    {
        TENSCounter = 0;
        TENSController::instance()->TENSout_Pin.setValue(1);
    }
    else
    {
        if (TENSCounter >= TENSController::PulseDuration)
            TENSController::instance()->TENSout_Pin.setValue(0);
        TENSCounter++;
    }

    __HAL_TIM_CLEAR_FLAG(&TENSController::instance()->tim3, TIM_FLAG_UPDATE);
}


TENSController::TENSController()
    : Led_Pin(GPIOA, GPIO_PIN_10), TENSout_Pin(GPIOA, GPIO_PIN_9)
{
    tim2.Instance = TIM2;
    tim2.Init.Prescaler = 128;
    tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim2.Init.Period = 4690; //6250;
    tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    tim3.Instance = TIM3;
    tim3.Init.Prescaler = 32;
    tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim3.Init.Period = 36;
    tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    pwm_callback_func = _TIM2_IRQHandler;
}

TENSController *TENSController::instance()
{
    if (!inst) {
        inst = new TENSController();
    }
    return inst;
}

void TENSController::begin()
{
    Led_Pin.setDirection(Pin::D_OUTPUT);
    TENSout_Pin.setDirection(Pin::D_OUTPUT);

    // configure timers
    TIM_ClockConfigTypeDef sClockSourceConfig = {
        .ClockSource = TIM_CLOCKSOURCE_INTERNAL
    };

    TIM_MasterConfigTypeDef sMasterConfig = {
        .MasterOutputTrigger = TIM_TRGO_RESET,
        .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE
    };

    assert(HAL_TIM_Base_Init(&tim2) == HAL_OK);
    assert(HAL_TIM_ConfigClockSource(&tim2, &sClockSourceConfig) == HAL_OK);
    assert(HAL_TIMEx_MasterConfigSynchronization(&tim2, &sMasterConfig) == HAL_OK);

    assert(HAL_TIM_Base_Init(&tim3) == HAL_OK);
    assert(HAL_TIM_ConfigClockSource(&tim3, &sClockSourceConfig) == HAL_OK);
    assert(HAL_TIMEx_MasterConfigSynchronization(&tim3, &sMasterConfig) == HAL_OK);
}

void TENSController::enable(bool enabled)
{
    // start timer
    reset();
    if (enabled) {
        HAL_TIM_Base_Start_IT(&tim2);
        HAL_TIM_Base_Start_IT(&tim3);
    }
}

void TENSController::reset()
{
    // disable timers
    HAL_TIM_Base_Stop_IT(&tim2);
    HAL_TIM_Base_Stop_IT(&tim3);

    Led_Pin.setValue(0);
    TENSout_Pin.setValue(0);
}

