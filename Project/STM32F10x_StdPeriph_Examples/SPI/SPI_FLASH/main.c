/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/main.c 
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
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include "platform_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ff.h"

#include "TestSpiFlash.h"
#include "BspSystick.h"
#include "BspExti.h"
#include "BspRcc.h"
#include "BspTimer.h"
#include "BspTimerBase.h"
#include "BspTimerPwm.h"

#include "BspTimerCapture.h"

#include "BspTimer7Pwm.h"

#include "BspTimer6Steps.h"

#include "BspTimerOcToggle.h"

#include "BspIwdg.h"
#include "BspWwdg.h"

#include "BspUsartPolling.h"

#include "BspUsartDmaPolling.h"

#include "BspUsartDmaInterrupt.h"

#include "BspI2cEeprom.h"


#include "BspDmaMtom.h"

#include "BspAdcSingle.h"

#include "BspAdcDma.h"

#include "BspAdcCont.h"

#include "BspSdio.h"

#include "BspKey.h"



unsigned int frequency = 0, highLevelUs = 0, lowLevelUs = 0;

static volatile uint16_t val = 0;

uint16_t adcResult[16] = {0};

uint16_t* pAdcResult = NULL;


const unsigned char writeBuf[16] = "Hello wolrd\n";

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

char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEF";
    unsigned unum;
    int i=0,j,k;
    
    if(radix==10&&num<0)
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;
    /*转换*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
       }while(unum);
    str[i]='\0';
    /*逆序*/
    if(str[0]=='-')
        k=1;
    else
        k=0;
     
    for(j=k;j<=(i-1)/2;j++)
    {       char temp;
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}


//extern int SD_disk_initialize();


FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_sd;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024] = {0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
"Hello World! This is a fatfs test file \r\n";
const char* fileNamePre = "0:TestFileTestLongFileTest";
const char* fileNameTail = ".txt";
char fileNameIndex[128] = {0}; 
char fileName[128] = {0};

void TestFatfs(void)
{	
	unsigned int index = 0;
	unsigned int index1 = 0;
	
	printf("\r\n******SD FATFS TEST******\r\n");
	NVIC_Configuration();
	
	res_sd = f_mount(&fs, "0:", 1);				// Mount file system, this will initlize the store device
	
/*----------------------- Format Test ---------------------------*/  
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("SD Card has no file system, will be format\r\n");

		res_sd = f_mkfs("0:", 0, 0);		//Format					
		
		if(res_sd == FR_OK)
		{
			printf("SD Card Format success\r\n");
			
			// Remount after format
			res_sd = f_mount(NULL, "0:", 1);			
 			
			res_sd = f_mount(&fs, "0:", 1);
		}
		else
		{
			printf("Format failed\r\n");
			while(1);
		}
	}
	else if(res_sd != FR_OK)
	{
		printf("SD Card Mount File System Failed, (%d)\r\n",res_sd);
		printf("The Card May not initileze\r\n");
		while(1);
	}
	else
	{
		printf("File system mount success, could do Read and write testing\r\n");
	}
 

/*----------------------- File System: Write Test -----------------------------*/

	printf("\r\n****** File System: Write Test ******\r\n");
	
	
	
	for(index = 0; index < 2; index++)
	{
		memset(fileName, 0, 128);
		
		strcpy(fileName, fileNamePre);
		itoa(index, fileNameIndex, 10);
		
		printf("fileNameIndex = %s\n", fileNameIndex);
		strcat(fileName, fileNameIndex);
		strcat(fileName, fileNameTail);
		printf("fileName = %s\n", fileName);
		
		
		
		
		//res_sd = f_open(&fnew, "0:T1.txt", FA_CREATE_ALWAYS | FA_WRITE );
		res_sd = f_open(&fnew, fileName, FA_CREATE_ALWAYS | FA_WRITE );
		if ( res_sd == FR_OK )
		{
			printf("Open File Success\r\n");
			for(index1 = 0; index1 < 200; index1++)
			{
				res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
				if(res_sd == FR_OK)
				{
					printf("Write Data Success, Write Data Num: %d\n", fnum);
				}
				else
				{
					printf("File Write Failed(%d)\n", res_sd);
				}
			}
			f_close(&fnew);
		}
		else
		{	
			printf("Create/Open File Failed\r\n");
			
			printf("res_sd = 0x%x\n", res_sd);
		}
	

/*------------------- File System: Reading Test------------------------------------*/
		printf("****** File System: Reading Test ******\r\n");
		
		res_sd = f_open(&fnew, fileName, FA_OPEN_EXISTING | FA_READ); 	 
		if(res_sd == FR_OK)
		{
			printf("Open File Success\r\n");
			res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
			if(res_sd==FR_OK)
			{
				printf("File System Read Sucdess, Read Nub: %d\r\n", fnum);
				printf("Read Data: \r\n%s \r\n", ReadBuffer);	
			}
			else
			{
				printf("File Read Failed: (%d)\n", res_sd);
			}		
		}
		else
		{
			printf("Open File Failed\r\n");
			printf("res_sd = 0x%x\n", res_sd);
		}

		f_close(&fnew);			
	}
	
	f_mount(NULL, "0:", 1); //Unmount
  

}



void DumpSDIORegs(void)
{
	printf("SDIO Regs Dump\n");
	
	printf("SDIO->POWER = 0x%08x\n", SDIO->POWER);
	printf("SDIO->CLKCR = 0x%08x\n", SDIO->CLKCR);
	printf("SDIO->ARG = 0x%08x\n", SDIO->ARG);
	printf("SDIO->CMD = 0x%08x\n", SDIO->CMD);
	
	printf("SDIO->RESPCMD = 0x%08x\n", SDIO->RESPCMD);
	printf("SDIO->RESP1 = 0x%08x\n", SDIO->RESP1);
	printf("SDIO->RESP2 = 0x%08x\n", SDIO->RESP2);
	printf("SDIO->RESP3 = 0x%08x\n", SDIO->RESP3);
	
	printf("SDIO->RESP4 = 0x%08x\n", SDIO->RESP4);
	printf("SDIO->DTIMER = 0x%08x\n", SDIO->DTIMER);
	printf("SDIO->DLEN = 0x%08x\n", SDIO->DLEN);
	printf("SDIO->DCTRL = 0x%08x\n", SDIO->DCTRL);
	
	printf("SDIO->DCOUNT = 0x%08x\n", SDIO->DCOUNT);
	printf("SDIO->STA = 0x%08x\n", SDIO->STA);
	printf("SDIO->ICR = 0x%08x\n", SDIO->ICR);
	printf("SDIO->MASK = 0x%08x\n", SDIO->MASK);
	
	
	printf("SDIO->FIFOCNT = 0x%08x\n", SDIO->FIFOCNT);
	printf("SDIO->FIFO = 0x%08x\n", SDIO->FIFO);
}

 
void MainDelay(unsigned int n)
{
	volatile unsigned int index = n;
	while(n--)
	{
		index = n;
		while(index--);
	}
}

//Width can be 8 / 16 / 32
void MemDump(void* ptr, unsigned int length, unsigned int width)
{
	unsigned int index = 0;
	unsigned char* ptrChar = NULL;
	unsigned short* ptrShort = NULL;
	unsigned int* ptrInt = NULL;
	
	if(ptr != NULL)
	{
		switch (width)
		{
			case 8:
				ptrChar = (unsigned char*)ptr;
				
				for(index = 0; index < length; index++)
				{
					if((index % 16) == 0)
					{
						printf("\n");
						printf("0x%08x-0x%08x: ", (index), (index + 15));
						
					}
					printf("0x%02x ", *(ptrChar + index));
				}
				
				printf("\n");
				
				break;
			
			case 16:
				ptrShort = (unsigned short*)ptr;
				
				for(index = 0; index < length; index++)
				{
					printf("%d ", *(ptrShort + index));
				}
				
				printf("\n");
				
				break;
				
			case 32:
				ptrInt = (unsigned int*)ptr;
				
				for(index = 0; index < length; index++)
				{
					printf("%d ", *(ptrInt + index));
				}
				
				printf("\n");
				
				break;
				
			default:
				printf("Width Error\n");
		}
	}
}

void MallocTest()
{
	char* pTestArray[10] = {NULL};
	int index = 0;
//	int* pHead = (int*)(0x20000220 - 4);
	
	for(index = 0; index < 10; index++)
	{
		pTestArray[index] = malloc(16);
		
		printf("index = %d, *((int*)(pTestArray[index] - 4)) = 0x%x, *((int*)(pTestArray[index] - 8)) = 0x%x, pTestArray[index] = 0x%x\n", index, 
				*((int*)(pTestArray[index] - 4)), *((int*)(pTestArray[index] - 8)), pTestArray[index]);
	}

	for(index = 0; index < 10; index++)
	{
		if(pTestArray[index] != NULL )
		{
			free(pTestArray[index]);
			pTestArray[index] = NULL;
		}
	}
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
//	UsartDmaPollingTest();
	int testInt = 0;
	char commandStr[16] = {0};

	
	LogDebugInit();
	BspKeyConfig();
//	TestSpiFlash();
	
//	SystickInit();
	
	printf("Test Start\n");
	
	
	MallocTest();
	
//	scanf("%d", &testInt);

	scanf("%s", commandStr);
	
	
	printf("testInt = 0x%x\n", testInt);
	
	
	printf("commandStr = %s\n", commandStr);
	
//	RccTest();
	
//	TimerTest();

//	TimerBaseTest();
//	TimerPwmTest();
/*	
	EXTI0_Config();
	
	EXTI15_10_Config();
*/	
//	EXTI9_5_Config();
	

//	TimerCaptureInit();
	
//	Timer7PwmInit();
	
//	Timer6StepsInit();
	
//	TimerOcToggleInit();

//	IWDGInit();
//	BspWWDGInit();
	
//	UsartPollingTest();
//	UsartDmaPollingTest();
	
//	UsartDmaInterruptTest();
	
//	BspI2cEepromTest();
//	BspDmaMtomTest();
	
//	BspAdcSingle();
//	BspAdcDma();
	
//	BspAdcCont();
//	NVIC_Configuration();
//	TestSdio();
	
//	TestFatfs();
	
//	RccTest();

//	printf("KEY1: TestSdio()\n");
//	printf("KEY2: TestFatfs()\n");
	while (1)
	{
//		Delay(1000);
		MainDelay(5000);
		printf("Hello world\n");
/*		
		if(BspKeyScan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
		{
			TestSdio();
		}
		else if(BspKeyScan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
		{
			TestFatfs();
		}
		
*/		
		
//		
		
		
//		BspGetAdcValCont(adcResult);
//		pAdcResult = BspGetAdcValDma();
		
//		MemDump(pAdcResult, 6, 16);
		
		
		
//		val = BspGetAdcAverageVal(ADC_Channel_16, 10);

//		val = BspGetAdcValDma(ADC_Channel_4);
		
//		printf("val = %d\n", val);
		
//		EXTI_GenerateSWInterrupt(EXTI_Line0);
		
//		TimerCaptureStart();
		
//		Delay(3000);
		
//		CaptureCaculate(&frequency, &highLevelUs, &lowLevelUs);
		
//		printf("frequency = %d\n", frequency);
//		printf("highLevelUs = %d\n", highLevelUs);
//		printf("lowLevelUs = %d\n", lowLevelUs);
		
//		IWDG_ReloadCounter();
 
//		WWDG_SetCounter(120);
	}
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
	{}
}

#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
