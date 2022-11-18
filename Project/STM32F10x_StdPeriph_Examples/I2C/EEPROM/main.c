/**
  ******************************************************************************
  * @file    I2C/EEPROM/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "stm32_eval_i2c_ee.h"
#include "LogDebug.h"
#include <stdio.h>



/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_EEPROM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

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
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1_Buffer[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
};
uint8_t Tx2_Buffer[] = "/* STM32F10xx I2C Firmware Library EEPROM driver example: \
                        buffer 2 transfer into address sEE_WRITE_ADDRESS2 */";

uint8_t Rx1_Buffer[BUFFER_SIZE1] = {0}, Rx2_Buffer[BUFFER_SIZE2] = {0};
uint8_t Rx_Buffer[256] = {0};
uint8_t Tx_Buffer[256] = {0};
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint16_t NumDataRead = 0;
volatile uint8_t NumDataWrite = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);



void MainDelay(unsigned int n)
{
	volatile unsigned int index = n;
	
	while(n--)
	{
		index = n;
		while(index--);
	}
}


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


void DumpGpioRegs(GPIO_TypeDef* pGpio)
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
int main(void)
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
#if 0
  NumDataWrite = 0x10;
  
  sEE_WritePage(Tx_Buffer, 0x20, &NumDataWrite);
  
   while (NumDataWrite > 0)
  {    
    /* Starting from this point, if the requested number of data is higher than 1, 
       then only the DMA is managing the data transfer. Meanwhile, CPU is free to 
       perform other tasks:
       
      // Add your code here: 
      //...
      //...
	
       For simplicity reasons, this example is just waiting till the end of the 
       transfer. */    
  }
#endif  

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
#if 0
/*----------------------------------
  
                                    ------------------------------------------*/
  
  /* Second write in the memory followed by a read of the written data -------*/
  /* Write on I2C EEPROM from sEE_WRITE_ADDRESS2 */
  sEE_WriteBuffer(Tx2_Buffer, sEE_WRITE_ADDRESS2, BUFFER_SIZE2);

  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE2;  
  
  /* Read from I2C EEPROM from sEE_READ_ADDRESS2 */
  sEE_ReadBuffer(Rx2_Buffer, sEE_READ_ADDRESS2, (uint16_t *)(&NumDataRead));
 
  
  /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
    resets the variable holding the number of data to be read) */
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
  }
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus2 = Buffercmp(Tx2_Buffer, Rx2_Buffer, BUFFER_SIZE2);
  if(TransferStatus2 == PASSED)
  {
	  printf("I2c Test2 Passed\n");
  }
  /* TransferStatus2 = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
  /* TransferStatus2 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */

#endif
  
  /* Free all used resources */
  sEE_DeInit();

  while (1)
  {
	  printf("Main While...\n");
	  MainDelay(5000);	
  }
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
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
