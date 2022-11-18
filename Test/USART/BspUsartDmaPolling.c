
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>

#include "BspUtility.h"

#include "BspUsartDmaPolling.h"

extern void MainDelay(unsigned int n);

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
//#define TxBufferSize1   (countof(TxBuffer1))
//#define TxBufferSize2   (countof(TxBuffer2))

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
static USART_InitTypeDef USART_InitStructure;
static uint8_t TxBuffer1[] = "USART DMA Polling: USARTy -> USARTz using DMA";
static uint8_t TxBuffer2[] = "USART DMA Polling: USARTz -> USARTy using DMA";
static uint8_t RxBuffer1[TxBufferSize2] = {0};
static uint8_t RxBuffer2[TxBufferSize1] = {0};
static volatile TestStatus TransferStatus1 = FAILED;
static volatile TestStatus TransferStatus2 = FAILED; 

/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void DMA_Configuration(void);


/* Private functions ---------------------------------------------------------*/



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void UsartDmaPollingTest(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     
	   
	/* System Clocks Configuration */
	RCC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* Configure the DMA */
	DMA_Configuration();

	/* USARTy and USARTz configuration ------------------------------------------------------*/
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

	/* Enable USARTy DMA Rx and TX request */
	USART_DMACmd(USARTy, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
	/* Enable USARTz DMA Rx and TX request */
	USART_DMACmd(USARTz, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

	/* Enable USARTy TX DMA1 Channel */
	DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);
	
	/* Enable USARTz TX DMA1 Channel */
	DMA_Cmd(USARTz_Tx_DMA_Channel, ENABLE);
	
	/* Enable USARTy RX DMA1 Channel */
	DMA_Cmd(USARTy_Rx_DMA_Channel, ENABLE);

	
	/* Enable USARTz RX DMA1 Channel */
	DMA_Cmd(USARTz_Rx_DMA_Channel, ENABLE);

	/* Enable the USARTy */
	USART_Cmd(USARTy, ENABLE);
	/* Enable the USARTz */
	USART_Cmd(USARTz, ENABLE);
	
	
	printf("USART2 Regs:\n");
	DumpUsartRegs(USART2);
	
	printf("USART3 Regs:\n");
	DumpUsartRegs(USART3);
	
	printf("DMA1 Regs:\n");
	DumpDmaRegs(DMA1);
	
	printf("DMA1_Channel6 Regs:(USART2_RX)\n");
	DumpDmaChannelRegs(DMA1_Channel6);
	
	printf("DMA1_Channel7 Regs:(USART2_TX)\n");
	DumpDmaChannelRegs(DMA1_Channel7);
	
	printf("DMA1_Channel2 Regs:(USART3_TX)\n");
	DumpDmaChannelRegs(DMA1_Channel2);
	
	printf("DMA1_Channel3 Regs:(USART3_RX)\n");
	DumpDmaChannelRegs(DMA1_Channel3);
	
	
	printf("Log1\n");
	/* Wait until USARTy TX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTy_Tx_DMA_FLAG) == RESET)
	{
	}
	printf("Log2\n");
	
	/* Wait until USARTz RX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTz_Rx_DMA_FLAG) == RESET)
	{
	}
	printf("Log3\n");
	
	printf("TxBuffer1 = %s\n", TxBuffer1);
	
	printf("RxBuffer1 = %s\n", RxBuffer1);
	
	printf("TxBuffer2 = %s\n", TxBuffer2);
	
	printf("RxBuffer2 = %s\n", RxBuffer2);
	
	/* Wait until USARTz TX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTz_Tx_DMA_FLAG) == RESET)
	{
	}
	
	printf("Log4\n");
	
	/* Wait until USARTy RX DMA1 Channel Transfer Complete */
	while (DMA_GetFlagStatus(USARTy_Rx_DMA_FLAG) == RESET)
	{
		MainDelay(5000);

		printf("USART2 Regs:\n");
		DumpUsartRegs(USART2);
		
		printf("USART3 Regs:\n");
		DumpUsartRegs(USART3);
		
		printf("DMA1 Regs:\n");
		DumpDmaRegs(DMA1);
		
		printf("DMA1_Channel6 Regs:(USART2_RX)\n");
		DumpDmaChannelRegs(DMA1_Channel6);
		
		printf("DMA1_Channel7 Regs:(USART2_TX)\n");
		DumpDmaChannelRegs(DMA1_Channel7);
		
		printf("DMA1_Channel2 Regs:(USART3_TX)\n");
		DumpDmaChannelRegs(DMA1_Channel2);
		
		printf("DMA1_Channel3 Regs:(USART3_RX)\n");
		DumpDmaChannelRegs(DMA1_Channel3);
	
	}
	
	printf("Log5\n");
	

	

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
static void RCC_Configuration(void)
{    
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTz_GPIO_CLK, ENABLE);

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
static void GPIO_Configuration(void)
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
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
static void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
	DMA_DeInit(USARTy_Tx_DMA_Channel);
	
	printf("USARTy_DR_Base = 0x%x\n", USARTy_DR_Base);
	printf("USARTz_DR_Base = 0x%x\n", USARTz_DR_Base);
	
	printf("USART2->DR = 0x%x\n", USART2->DR);
	printf("USART3->DR = 0x%x\n", USART3->DR);
	
	printf("TxBufferSize1 = 0x%x\n", TxBufferSize1);
	printf("TxBufferSize2 = 0x%x\n", TxBufferSize2);
	
	printf("TxBuffer1 = 0x%x\n", TxBuffer1);
	printf("TxBuffer2 = 0x%x\n", TxBuffer2);
	printf("RxBuffer1 = 0x%x\n", RxBuffer1);
	printf("RxBuffer2 = 0x%x\n", RxBuffer2);
	
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

	/* USARTy RX DMA1 Channel (triggered by USARTy Rx event) Config */
	DMA_DeInit(USARTy_Rx_DMA_Channel);  
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = TxBufferSize2;
	
	DMA_Init(USARTy_Rx_DMA_Channel, &DMA_InitStructure);

	/* USARTz TX DMA1 Channel (triggered by USARTz Tx event) Config */
	DMA_DeInit(USARTz_Tx_DMA_Channel);  
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = TxBufferSize2;  
	
	DMA_Init(USARTz_Tx_DMA_Channel, &DMA_InitStructure);

	/* USARTz RX DMA1 Channel (triggered by USARTz Rx event) Config */
	DMA_DeInit(USARTz_Rx_DMA_Channel);  
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = TxBufferSize1;
	
	DMA_Init(USARTz_Rx_DMA_Channel, &DMA_InitStructure);  
}
