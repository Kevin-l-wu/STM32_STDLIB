
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>

#include "BspUtility.h"
#include "BspDmaMtom.h"



/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup DMA_SPI_RAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define TEST_BUF_LEN 16

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static DMA_InitTypeDef    DMA_InitStructure;

static uint8_t testTxBuf[TEST_BUF_LEN] = {
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10
};

static uint8_t testRxBuf[TEST_BUF_LEN] = {0};


/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void BspDmaMtomTest(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     


	/* System Clocks Configuration */
	RCC_Configuration();


	/* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)testTxBuf;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)testRxBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 16;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);


	/* Enable DMA channels */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Transfer complete */
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	
	
	printf("testTxBuf[0] = 0x%x\n", testTxBuf[0]);
	printf("testTxBuf[15] = 0x%x\n", testTxBuf[15]);
	
	printf("testRxBuf[0] = 0x%x\n", testRxBuf[0]);
	printf("testRxBuf[15] = 0x%x\n", testRxBuf[15]);
	
 
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}
