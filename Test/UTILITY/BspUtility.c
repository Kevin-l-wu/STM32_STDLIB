#include "stm32_eval.h"
#include "BspUtility.h"

#include <stdio.h>



void DumpUsartRegs(USART_TypeDef* pUsart)
{
	printf("pUsart->SR = 0x%x\n", pUsart->SR);
	printf("pUsart->DR = 0x%x\n", pUsart->DR);
	printf("pUsart->BRR = 0x%x\n", pUsart->BRR);
	printf("pUsart->CR1 = 0x%x\n", pUsart->CR1);
	printf("pUsart->CR2 = 0x%x\n", pUsart->CR2);
	printf("pUsart->CR3 = 0x%x\n", pUsart->CR3);
	printf("pUsart->GTPR = 0x%x\n", pUsart->GTPR);
}

void DumpDmaRegs(DMA_TypeDef* pDma)
{
	printf("pDma->ISR = 0x%x\n", pDma->ISR);
	printf("pDma->IFCR = 0x%x\n", pDma->IFCR);
}

void DumpDmaChannelRegs(DMA_Channel_TypeDef* pDmaChannel)
{
	printf("pDmaChannel->CCR = 0x%x\n", pDmaChannel->CCR);
	printf("pDmaChannel->CNDTR = 0x%x\n", pDmaChannel->CNDTR);
	printf("pDmaChannel->CPAR = 0x%x\n", pDmaChannel->CPAR);
	printf("pDmaChannel->CMAR = 0x%x\n", pDmaChannel->CMAR);
}


/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while(BufferLength--)
	{
		if((*pBuffer1) != (*pBuffer2))
		{
			return FAILED;
		}
	//	printf("(*pBuffer1) = 0x%02x, (*pBuffer2) = 0x%02x\n", (*pBuffer1), (*pBuffer2));
		pBuffer1++;
		pBuffer2++;
	}

	return PASSED;  
}
