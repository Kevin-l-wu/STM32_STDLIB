
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include <stdio.h>

#include "TestSpiFlash.h"



/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define  FLASH_WriteAddress     0x700000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
/*
#if defined(USE_STM32100B_EVAL) || defined(USE_STM32100E_EVAL)
  #define  sFLASH_ID       sFLASH_M25P128_ID
#else
  #define  sFLASH_ID       sFLASH_M25P64_ID
#endif
*/

#define sFLASH_W25Q64_ID 0xef4017

#define  sFLASH_ID       sFLASH_W25Q64_ID

#define  BufferSize (countof(Tx_Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
//uint8_t Tx_Buffer[] = "STM32F10x SPI Firmware Library Example: communication with an M25P SPI FLASH";
uint8_t Tx_Buffer[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x00
};
uint8_t  Rx_Buffer[BufferSize];
__IO uint8_t Index = 0x0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
__IO uint32_t FlashID = 0;

/* Private functions ---------------------------------------------------------*/
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


void DumpBuf(unsigned char* pBuf, unsigned int length)
{
	unsigned int index = 0; 
	
	if((pBuf != NULL) && (length > 0))
	{
		for(index = 0; index < length; index++)
		{
			if((index % 8) == 0)
			{
				printf("\n");
				printf("0x%02x...0x%02x:  ", index, (index + 8));
			}
			printf("0x%02x ", pBuf[index]);
		}
		printf("\n");
	}
}


void TestSpiFlash(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32f10x_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f10x.c file
	*/     

	/* Initialize Leds mounted on STM3210X-EVAL board */
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);

	/* Initialize the SPI FLASH driver */
	sFLASH_Init();

	printf("Hello Spi Flash\n");
	/* Get SPI Flash ID */
	FlashID = sFLASH_ReadID();

	printf("FlashID = 0x%x\n", FlashID);
	printf("FlashID = 0x%x\n", sFLASH_ID);

	/* Check the SPI Flash ID */
	if (FlashID == sFLASH_ID)
	{
		/* OK: Turn on LD1 */
		STM_EVAL_LEDOn(LED1);

		/* Perform a write in the Flash followed by a read of the written data */
		/* Erase SPI FLASH Sector to write on */
		sFLASH_EraseSector(FLASH_SectorToErase);

		/* Write Tx_Buffer data to SPI FLASH memory */
		sFLASH_WriteBuffer(Tx_Buffer, FLASH_WriteAddress, BufferSize);

		/* Read data from SPI FLASH memory */
		sFLASH_ReadBuffer(Rx_Buffer, FLASH_ReadAddress, BufferSize);

		printf("Dump Rx_Buffer:\n");

		DumpBuf(Rx_Buffer, BufferSize);

		/* Check the correctness of written dada */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		/* TransferStatus1 = PASSED, if the transmitted and received data by SPI1
		are the same */
		/* TransferStatus1 = FAILED, if the transmitted and received data by SPI1
		are different */

		printf("TransferStatus1 = 0x%x\n", TransferStatus1);

		/* Perform an erase in the Flash followed by a read of the written data */
		/* Erase SPI FLASH Sector to write on */
		sFLASH_EraseSector(FLASH_SectorToErase);

		/* Read data from SPI FLASH memory */
		sFLASH_ReadBuffer(Rx_Buffer, FLASH_ReadAddress, BufferSize);

		/* Check the correctness of erasing operation dada */
		for (Index = 0; Index < BufferSize; Index++)
		{
			if (Rx_Buffer[Index] != 0xFF)
			{
				TransferStatus2 = FAILED;
			}
		}
		/* TransferStatus2 = PASSED, if the specified sector part is erased */
		/* TransferStatus2 = FAILED, if the specified sector part is not well erased */
	}
	else
	{
		/* Error: Turn on LD2 */
		STM_EVAL_LEDOn(LED2);
	}

	while (1)
	{}
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}
