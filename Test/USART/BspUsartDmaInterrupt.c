
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>

#include "BspUtility.h"
#include "BspUsartDmaInterrupt.h"


//extern void MainDelay(unsigned int n);




/* Private variables ---------------------------------------------------------*/
static USART_InitTypeDef USART_InitStructure;
static uint8_t TxBuffer1[] = "USART DMA Interrupt: USARTy -> USARTz using DMA Tx and Rx Flag";
static uint8_t TxBuffer2[] = "USART DMA Interrupt: USARTz -> USARTy using DMA Tx and Rx Interrupt";
static uint8_t RxBuffer1[TxBufferSize2];
uint8_t RxBuffer2[TxBufferSize1];
uint8_t NbrOfDataToRead = TxBufferSize1;
static uint32_t index = 0;
static volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;

/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);
static void DMA_Configuration(void);
//TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void UsartDmaInterruptTest(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
		this is done through SystemInit() function which is called from startup
		file (startup_stm32f10x_xx.s) before to branch to application main.
		To reconfigure the default setting of SystemInit() function, refer to
		system_stm32f10x.c file
	*/     

	/* System Clocks Configuration */
	RCC_Configuration();

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* Configure the DMA */
	DMA_Configuration();

	/* USARTy and USARTz configuration -------------------------------------------*/
	/* USARTy and USARTz configured as follow:
	- BaudRate = 230400 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/

	USART_InitStructure.USART_BaudRate = 230400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USARTy */
	USART_Init(USARTy, &USART_InitStructure);

	/* Configure USARTz */
	USART_Init(USARTz, &USART_InitStructure);

	/* Enable USARTy DMA TX request */
	USART_DMACmd(USARTy, USART_DMAReq_Tx, ENABLE);

	/* Enable USARTz DMA TX request */
	USART_DMACmd(USARTz, USART_DMAReq_Tx, ENABLE);

	/* Enable the USARTz Receive Interrupt */
	USART_ITConfig(USARTz, USART_IT_RXNE, ENABLE);

	/* Enable USARTy */
	USART_Cmd(USARTy, ENABLE);

	/* Enable USARTz */
	USART_Cmd(USARTz, ENABLE);

	/* Enable USARTy DMA TX Channel */
	DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);

	/* Enable USARTz DMA TX Channel */
	DMA_Cmd(USARTz_Tx_DMA_Channel, ENABLE);

	/* Receive the TxBuffer2 */
	while(index < TxBufferSize2)
	{
		while(USART_GetFlagStatus(USARTy, USART_FLAG_RXNE) == RESET)
		{
		}
		RxBuffer1[index++] = USART_ReceiveData(USARTy);  
	}

	/* Wait until USARTy TX DMA1 Channel  Transfer Complete */
	while (DMA_GetFlagStatus(USARTy_Tx_DMA_FLAG) == RESET)
	{
	}
		
	/* Wait until USARTz TX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTz_Tx_DMA_FLAG) == RESET)
	{
	}

	/* Check the received data with the send ones */
	TransferStatus1 = Buffercmp(TxBuffer2, RxBuffer1, TxBufferSize2);
	/* TransferStatus1 = PASSED, if the data transmitted from USARTz and  
	received by USARTy are the same */
	/* TransferStatus1 = FAILED, if the data transmitted from USARTz and 
	received by USARTy are different */
	TransferStatus2 = Buffercmp(TxBuffer1, RxBuffer2, TxBufferSize1);
	/* TransferStatus2 = PASSED, if the data transmitted from USARTy and  
	received by USARTz are the same */
	/* TransferStatus2 = FAILED, if the data transmitted from USARTy and 
	received by USARTz are different */
	
	printf("TransferStatus1 = 0x%x\n", TransferStatus1);
	printf("TransferStatus2 = 0x%x\n", TransferStatus2);

}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{    
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTz_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);


  /* Enable USARTy Clock */
  RCC_APB1PeriphClockCmd(USARTy_CLK, ENABLE); 

  /* Enable USARTz Clock */
  RCC_APB1PeriphClockCmd(USARTz_CLK, ENABLE);  
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USARTy Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = USARTy_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

	/* Configure USARTz Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = USARTz_RxPin;
	
	GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);  

	/* Configure USARTy Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USARTy_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

	/* Configure USARTz Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = USARTz_TxPin;
	
	GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);  
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USARTz Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USARTz_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
	DMA_DeInit(USARTy_Tx_DMA_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);

	/* USARTz_Tx_DMA_Channel (triggered by USARTz Tx event) Config */
	DMA_DeInit(USARTz_Tx_DMA_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = TxBufferSize2;
	
	DMA_Init(USARTz_Tx_DMA_Channel, &DMA_InitStructure);
}
