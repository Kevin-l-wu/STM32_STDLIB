
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"
#include "stm32_eval_sdio_sd.h"

#include "LogDebug.h"
#include <stdio.h>
#include <string.h>

#include "BspUtility.h"
#include "BspSdio.h"




typedef enum{
	SD_INIT_SUCCESS = 0,
	SD_NOINIT = 1,
} SD_STATUS;

extern void MemDump(void* ptr, unsigned int length, unsigned int width);

/* Private define ------------------------------------------------------------*/
#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      32  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)

#define SD_OPERATION_ERASE          0
#define SD_OPERATION_BLOCK          1
#define SD_OPERATION_MULTI_BLOCK    2 
#define SD_OPERATION_END            3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __attribute__ ((aligned (4))) uint8_t Buffer_Block_Tx[BLOCK_SIZE] = {0}, Buffer_Block_Rx[BLOCK_SIZE] = {0};
static __attribute__ ((aligned (4))) uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE] = {0}, Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE] = {0};
static volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
static SD_Error Status = SD_OK;
static __IO uint32_t SDCardOperation = SD_OPERATION_ERASE;

static volatile SD_STATUS sdStatus = SD_NOINIT;

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Configuration(void);
static void SD_EraseTest(void);
static void SD_SingleBlockTest(void);
static void SD_MultiBlockTest(void);
static void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
static TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/
/*
int SD_disk_status()
{
	printf("SD_disk_status(): sdStatus = 0x%x\n", sdStatus);
	return 0;
}


int SD_disk_initialize()
{
	printf("SD_disk_initialize()1: sdStatus = 0x%x\n", sdStatus);
	if((Status = SD_Init()) != SD_OK)
	{
		printf("SD Initialize Failed\n");
		sdStatus = SD_NOINIT;
	}
	else
	{
		printf("SD Initialize Success\n");
		sdStatus = SD_INIT_SUCCESS;
	}
	
	printf("SD_disk_initialize()2: sdStatus = 0x%x\n", sdStatus);
	return sdStatus;
}


int SD_disk_read(BYTE* buff, DWORD sector, UINT count)
{
	printf("SD_disk_read(): sdStatus = 0x%x\n", sdStatus);
	return SD_ReadBlock(buff, sector, count);;
}

int SD_disk_write(const BYTE* buff, DWORD sector, UINT count)
{
	printf("SD_disk_write(): sdStatus = 0x%x\n", sdStatus);
	return SD_WriteBlock((unsigned char*)buff, sector, count);;
}

*/


void ReadDumpTest()
{
	
	/* Read block of 512 bytes from address 0 */
	Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
	/* Check if the Transfer is finished */
	Status = SD_WaitReadOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);

	MemDump(Buffer_Block_Rx, (512), 8);
	
	memset(Buffer_Block_Rx, 0, (512));
	
	/* Read block of 512 bytes from address 0 */
	Status = SD_ReadBlock(Buffer_Block_Rx, 0x400000, BLOCK_SIZE);
	/* Check if the Transfer is finished */
	Status = SD_WaitReadOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);
	
	MemDump(Buffer_Block_Rx, (512 * 2), 8);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void TestSdio(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32f10x_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f10x.c file
	*/

	/* Interrupt Config */
	NVIC_Configuration();

	/*------------------------------ SD Init ---------------------------------- */
	printf("SD Initialize Start\n");
	if((Status = SD_Init()) != SD_OK)
	{
		printf("SD Initialize Failed\n");
	}
	else
	{
		printf("SD Initialize Success\n");
	}
	
	printf("SD Initialize end, Status = 0x%x\n", Status);
	
	ReadDumpTest();
	
	

//	while((Status == SD_OK) && (SDCardOperation != SD_OPERATION_END) && (SD_Detect()== SD_PRESENT))
	while((Status == SD_OK) && (SDCardOperation != SD_OPERATION_END))
	{
		switch(SDCardOperation)
		{
			/*-------------------------- SD Erase Test ---------------------------- */
			case (SD_OPERATION_ERASE):
			{
				SD_EraseTest();
				SDCardOperation = SD_OPERATION_BLOCK;
				break;
			}
			/*-------------------------- SD Single Block Test --------------------- */
			case (SD_OPERATION_BLOCK):
			{
				SD_SingleBlockTest();
				SDCardOperation = SD_OPERATION_MULTI_BLOCK;
				break;
			}       
			/*-------------------------- SD Multi Blocks Test --------------------- */
			case (SD_OPERATION_MULTI_BLOCK):
			{
				SD_MultiBlockTest();
				SDCardOperation = SD_OPERATION_END;
				break;
			}              
		}
	}
}

/**
  * @brief  Configures SDIO IRQ channel.
  * @param  None
  * @retval None
  */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Tests the SD card erase operation.
  * @param  None
  * @retval None
  */
static void SD_EraseTest(void)
{
	/*------------------- Block Erase ------------------------------------------*/
	if (Status == SD_OK)
	{
		/* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
		Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));
	}

	printf("SD_EraseTest(): SD_Erase Success, Status = 0x%x\n", Status);

	if (Status == SD_OK)
	{
		Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);

//		printf("SD_EraseTest(): Log1, Status = 0x%x\n", Status);

		/* Check if the Transfer is finished */
		Status = SD_WaitReadOperation();

//		printf("SD_EraseTest(): Log2, Status = 0x%x\n", Status);

		/* Wait until end of DMA transfer */
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}

	/* Check the correctness of erased blocks */
	if (Status == SD_OK)
	{
		EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
	}

	if(EraseStatus == PASSED)
	{
		printf("SD_EraseTest Passed\n");
	}
	else
	{
		printf("SD_EraseTest Failed\n");
	}
}

/**
  * @brief  Tests the SD card Single Blocks operations.
  * @param  None
  * @retval None
  */
static void SD_SingleBlockTest(void)
{
	/*------------------- Block Read/Write --------------------------*/
	/* Fill the buffer to send */
	Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);

	printf("Buffer_Block_Tx:\n");
	MemDump(Buffer_Block_Tx, (512), 8);

	if (Status == SD_OK)
	{
		/* Write block of 512 bytes on address 0 */
		Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
		/* Check if the Transfer is finished */
		Status = SD_WaitWriteOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	
	if (Status == SD_OK)
	{
		/* Read block of 512 bytes from address 0 */
		Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);
		/* Check if the Transfer is finished */
		Status = SD_WaitReadOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}

	
	/* Check the correctness of written data */
	if (Status == SD_OK)
	{
		TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE);
	}
	
	if(TransferStatus1 == PASSED)
	{
		printf("SD_SingleBlockTest Passed\n");
	}
	else
	{
		printf("SD_SingleBlockTest Failed\n");
	}
}

/**
  * @brief  Tests the SD card Multiple Blocks operations.
  * @param  None
  * @retval None
  */
static void SD_MultiBlockTest(void)
{
	/*--------------- Multiple Block Read/Write ---------------------*/
	/* Fill the buffer to send */
	Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

	if (Status == SD_OK)
	{
		/* Write multiple block of many bytes on address 0 */
		Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
		/* Check if the Transfer is finished */
		Status = SD_WaitWriteOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}

	if (Status == SD_OK)
	{
		/* Read block of many bytes from address 0 */
		Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
		/* Check if the Transfer is finished */
		Status = SD_WaitReadOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}

	/* Check the correctness of written data */
	if (Status == SD_OK)
	{
		TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
	}

	if(TransferStatus2 == PASSED)
	{
		printf("SD_MultiBlockTest Passed\n");
	}
	else
	{
		printf("SD_MultiBlockTest Failed\n");
	}
}


/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
static void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
	uint16_t index = 0;

	/* Put in global buffer same values */
	for (index = 0; index < BufferLength; index++)
	{
		//pBuffer[index] = index + Offset;
		pBuffer[index] = (index % 256);
	}
}

/**
  * @brief  Checks if a buffer has all its values are equal to zero.
  * @param  pBuffer: buffer to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer values are zero
  *         FAILED: At least one value from pBuffer buffer is different from zero.
  */
static TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
	while (BufferLength--)
	{
		/* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
		if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))
		{
			return FAILED;
		}

		pBuffer++;
	}

	return PASSED;
}
