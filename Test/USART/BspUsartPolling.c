
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>

#include "BspUtility.h"
#include "BspUsartPolling.h"


extern void MainDelay(unsigned int n);

/* Private typedef -----------------------------------------------------------*/
//typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define TxBufferSize   (countof(TxBuffer))

/* Private macro -------------------------------------------------------------*/
//#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
static USART_InitTypeDef USART_InitStructure;
static uint8_t TxBuffer[] = "Buffer Send from USARTy to USARTz using Flags";
static uint8_t RxBuffer[TxBufferSize + 1] = {0};
static __IO uint8_t TxCounter = 0, RxCounter = 0;  
static volatile TestStatus TransferStatus = FAILED;  

/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
//static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
__IO uint8_t index = 0;
  
/* Private functions ---------------------------------------------------------*/
/*
static  void DumpRegs(void)
{
	printf("USART2 Regs:\n");
	DumpUsartRegs(USART2);
	
	printf("USART3 Regs:\n");
	DumpUsartRegs(USART3);
}

*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void UsartPollingTest(void)
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

	USART_DeInit(USARTy);
	USART_DeInit(USARTz);

	/* USARTy and USARTz configuration ------------------------------------------------------*/
	/* USARTy and USARTz configured as follow:
		- BaudRate = 230400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- Even parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
//	USART_InitStructure.USART_BaudRate = 230400;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USARTy */
	USART_Init(USARTy, &USART_InitStructure);
	
//	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	/* Configure USARTz */
	USART_Init(USARTz, &USART_InitStructure);

	/* Enable the USARTy */
	USART_Cmd(USARTy, ENABLE);

	/* Enable the USARTz */
	USART_Cmd(USARTz, ENABLE);

	while(TxCounter < TxBufferSize)
	{   
		/* Send one byte from USARTy to USARTz */
		USART_SendData(USARTy, TxBuffer[TxCounter++]);

		/* Loop until USARTy DR register is empty */ 
		while(USART_GetFlagStatus(USARTy, USART_FLAG_TXE) == RESET)
		{
		}

		/* Loop until the USARTz Receive Data Register is not empty */
		while(USART_GetFlagStatus(USARTz, USART_FLAG_RXNE) == RESET)
		{
		}

		/* Store the received byte in RxBuffer */
		RxBuffer[RxCounter++] = (USART_ReceiveData(USARTz) & 0x7F);  

	}
#if 0

	while(TxCounter < TxBufferSize)
	{   
		/* Send one byte from USARTy to USARTz */
		USART_SendData(USARTz, TxBuffer[TxCounter++]);
		
		printf("Logs1\n");
	//	DumpRegs();

		/* Loop until USARTy DR register is empty */ 
		while(USART_GetFlagStatus(USARTz, USART_FLAG_TXE) == RESET)
		{
		}
		printf("Logs2\n");
	//	DumpRegs();
		/* Loop until the USARTz Receive Data Register is not empty */
		while(USART_GetFlagStatus(USARTy, USART_FLAG_RXNE) == RESET)
		{
			printf("Logs2 in while\n");
			MainDelay(5000);
//			DumpRegs();
		}
		printf("Logs3\n");
	//	DumpRegs();
		/* Store the received byte in RxBuffer */
		RxBuffer[RxCounter++] = (USART_ReceiveData(USARTy) & 0x7F);  

	}

#endif
	
	/* Check the received data with the send ones */
	TransferStatus = Buffercmp(TxBuffer, RxBuffer, TxBufferSize);
	/* TransferStatus = PASSED, if the data transmitted from USARTy and  
	 received by USARTz are the same */
	/* TransferStatus = FAILED, if the data transmitted from USARTy and 
	 received by USARTz are different */

	printf("TransferStatus = 0x%x\n", TransferStatus);
	printf("RxBuffer = %s\n", RxBuffer);
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{    
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

