#include "stm32_def.h"
#include "stm32_dma.h"

#include <FreeRTOSConfig.h>

#if defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
#include "stm32_dma_F2F4F7.h"
#elif defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32L1)
#include "stm32_dma_F0F1F3L1.h"
#elif defined(STM32L0)
#include "stm32_dma_L0.h"
#elif defined(STM32L4)
#include "stm32_dma_L4.h"
#else
#error "Unknown chip"
#endif

#include "DMAHalper.h"

static const dma_request_to_instance_t* findDMADesc(dmaRequest request, void* periphInstance) {
    for(size_t i=0; i<sizeof(dmaRequestToStream) / sizeof(dmaRequestToStream[0]); i++) {
        auto dma_request_to_instance = &dmaRequestToStream[i];
        if (dma_request_to_instance->request == request
                && dma_request_to_instance->periphInstance == periphInstance)
            return dma_request_to_instance;
    }
    return nullptr;
}


void FixSPI_DMA_IRQ_Prio(SPI_HandleTypeDef* spihandle, uint32_t prio = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1) {
    DMA_HandleTypeDef h;

    const dmaRequest reqs[] = {SPI_RX, SPI_TX};
    for (int i = 0; i < sizeof(reqs) / sizeof(dmaRequest); ++i) {
        auto def = findDMADesc(reqs[i], spihandle->Instance);
        if (def)
            NVIC_SetPriority((IRQn_Type)def->irqN, prio);
    }
}
