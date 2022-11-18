
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "stm32_eval_i2c_ee.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>

#include "BspUtility.h"

#include "BspI2cEeprom.h"



extern void MainDelay(unsigned int n);






/* Private typedef -----------------------------------------------------------*/
//typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Uncomment the following line to enable using LCD screen for messages display */
#define ENABLE_LCD_MSG_DISPLAY

#define sEE_WRITE_ADDRESS1        0x50
#define sEE_READ_ADDRESS1         0x50
#define BUFFER_SIZE1             (countof(Tx1_Buffer)-1)
#define BUFFER_SIZE2             (countof(Tx2_Buffer)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)

/* Private macro -------------------------------------------------------------*/
//#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1_Buffer[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
};
uint8_t Tx2_Buffer[] = "/* STM32F10xx I2C Firmware Library EEPROM driver example: \
                        buffer 2 transfer into address sEE_WRITE_ADDRESS2 */";

uint8_t Rx1_Buffer[BUFFER_SIZE1] = {0}, Rx2_Buffer[BUFFER_SIZE2] = {0};
static uint8_t Rx_Buffer[256] = {0};
//static uint8_t Tx_Buffer[256] = {0};
static volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint16_t NumDataRead = 0;
volatile uint8_t NumDataWrite = 0;

/* Private functions ---------------------------------------------------------*/
//TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


void DumpI2CRegs()
{
	printf("I2C1->CR1 = 0x%x\n", I2C1->CR1);
	printf("I2C1->CR2 = 0x%x\n", I2C1->CR2);
	printf("I2C1->OAR1 = 0x%x\n", I2C1->OAR1);
	printf("I2C1->OAR2 = 0x%x\n", I2C1->OAR2);
	printf("I2C1->DR = 0x%x\n", I2C1->DR);
	printf("I2C1->SR1 = 0x%x\n", I2C1->SR1);
	printf("I2C1->SR2 = 0x%x\n", I2C1->SR2);
	printf("I2C1->CCR = 0x%x\n", I2C1->CCR);
	printf("I2C1->TRISE = 0x%x\n", I2C1->TRISE);
}


static void DumpGpioRegs(GPIO_TypeDef* pGpio)
{
	printf("pGpio->CRL = 0x%x\n", pGpio->CRL);
	printf("pGpio->CRH = 0x%x\n", pGpio->CRH);
	printf("pGpio->IDR = 0x%x\n", pGpio->IDR);
	printf("pGpio->ODR = 0x%x\n", pGpio->ODR);
	printf("pGpio->BSRR = 0x%x\n", pGpio->BSRR);
	printf("pGpio->BRR = 0x%x\n", pGpio->BRR);
	printf("pGpio->LCKR = 0x%x\n", pGpio->LCKR);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void BspI2cEepromTest(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32f10x_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f10x.c file
	*/     

	unsigned int index = 10;

	unsigned int readRet = 0;

	LogDebugInit();

	while(index--)
	{
		MainDelay(3000);
		printf("I2c Test Starting...\n");
	}
	
	/* Initialize the I2C EEPROM driver ----------------------------------------*/
	sEE_Init();  

	printf("Dump I2c Regs:\n");
	
	DumpI2CRegs();
	
	DumpGpioRegs(GPIOB);
	
	/* First write in the memory followed by a read of the written data --------*/
	/* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
	sEE_WriteBuffer(Tx1_Buffer, 0x40, 16);

	//  MainDelay(1000);	

	sEE_WriteBuffer(Tx1_Buffer, 0x00, 16);

	sEE_WriteBuffer(Tx1_Buffer, 0x80, 16);


	/* Set the Number of data to be read */
	//NumDataRead = BUFFER_SIZE1;
	NumDataRead = 256;

	printf("NumDataRead = 0x%x\n", NumDataRead);

	/* Read from I2C EEPROM from sEE_READ_ADDRESS1 */

	/*  
	for(index = 0; index < 48; index++)
	{
	NumDataRead = 1;
	sEE_ReadBuffer((Rx_Buffer + index), (sEE_READ_ADDRESS1 + index), (uint16_t *)(&NumDataRead)); 

	while (NumDataRead > 0);
	}
	*/ 

	readRet = sEE_ReadBuffer(Rx_Buffer, 0, (uint16_t *)(&NumDataRead)); 

	printf("readRet = 0x%x\n", readRet);

	/* Wait till DMA transfer is complete (Transfer complete interrupt handler 
	resets the variable holding the number of data to be read) */

	//MainDelay(5000);
	while (NumDataRead > 0)
	{    
		/* Starting from this point, if the requested number of data is higher than 1, 
		then only the DMA is managing the data transfer. Meanwhile, CPU is free to 
		perform other tasks:

		// Add your code here: 
		//...
		//...

		For simplicity reasons, this example is just waiting till the end of the 
		transfer. */    

		printf("Waiting for reading complete\n");
		printf("NumDataRead = 0x%x\n", NumDataRead);


		MainDelay(3000);	
	}

	// printf("Rx1_Buffer = %s\n", Rx1_Buffer);
	printf("Dump Rx_Buffer\n");

	for(index = 0; index < 256; index++)
	{
		if((index % 8) == 0)
		{
			printf("\n");
			printf("0x%02x...0x%02x:  ", index, (index + 8));
		}
		printf("0x%02x ", Rx_Buffer[index]);
	}
	printf("\n");


	/* Check if the data written to the memory is read correctly */
	TransferStatus1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BUFFER_SIZE1);

	if(TransferStatus1 == PASSED)
	{
		printf("I2c Test2 Passed\n");
	}
	/* TransferStatus1 = PASSED, if the transmitted and received data 
	to/from the EEPROM are the same */
	/* TransferStatus1 = FAILED, if the transmitted and received data 
	to/from the EEPROM are different */


	/* Free all used resources */
	sEE_DeInit();

}

#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Example of timeout situation management.
  * @param  None.
  * @retval None.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
	/* Use application may try to recover the communication by resetting I2C
	peripheral (calling the function I2C_SoftwareResetCmd()) then re-start
	the transmission/reception from a previously stored recover point.
	For simplicity reasons, this example only shows a basic way for errors 
	managements which consists of stopping all the process and requiring system
	reset. */


	/* Block communication and all processes */
	printf("Timeout hanging...\n");
	
	DumpI2CRegs();
	
	while (1)
	{   
	}  
}

#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */

