#ifndef BSP_UTILITY_H_
#define BSP_UTILITY_H_


//Usart
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

#define countof(a)   (sizeof(a) / sizeof(*(a)))


#define TxBufferSize1   (countof(TxBuffer1))
#define TxBufferSize2   (countof(TxBuffer2))








void DumpUsartRegs(USART_TypeDef* pUsart);

void DumpDmaRegs(DMA_TypeDef* pDma);

void DumpDmaChannelRegs(DMA_Channel_TypeDef* pDmaChannel);


TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

#endif
