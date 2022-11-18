/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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
#include "stm32f10x_it.h"
#include "stm32_eval.h"
#include "stm32_eval_sdio_sd.h"
#include <stdio.h>

#include "BspTimerCapture.h"

extern __IO uint32_t TimingDelay;

__IO uint32_t step = 1;

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t capture = 0;
extern __IO uint16_t CCR1_Val;
extern __IO uint16_t CCR2_Val;
extern __IO uint16_t CCR3_Val;
extern __IO uint16_t CCR4_Val;

// captureFlag = 0: Capture stop
// captureFlag = 1: Capture start
// captureFlag = 2: Captured Rising
// captureFlag = 3: Captured Falling
// captureFlag = 4: Captured Second Rising(Success)

extern volatile unsigned char captureFlag;
extern volatile unsigned int captureCount1;
extern volatile unsigned int captureCount2;
extern volatile unsigned short captureValue1;
extern volatile unsigned short captureValue2;
extern volatile unsigned short captureValue3;


__IO uint16_t IC1ReadValue1 = 0, IC1ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0;
__IO uint32_t Capture = 0;
extern uint32_t LsiFreq;


uint8_t RxCounter = 0;
extern uint8_t RxBuffer2[];
extern uint8_t NbrOfDataToRead;

//extern uint16_t* gAdcConvertResult[CHANNEL_NUM];



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
   printf("HardFault_Handler, hang...\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
   printf("MemManage_Handler, hang...\n");
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  printf("BusFault_Handler, hang...\n");
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  printf("UsageFault_Handler, hang...\n");
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//	printf("SysTick_Handler\n");
	TimingDelay--;
//	TIM_GenerateEvent(TIM1, TIM_EventSource_COM);
}

void EXTI0_IRQHandler(void)
{
	printf("EXTI0_IRQHandler\n");
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		printf("Key wakup Pressed\n");
		
		/* Clear the  EXTI line 8 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI9_5_IRQHandler(void)
{
	printf("EXTI9_5_IRQHandler\n");
	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		printf("Key0 Pressed\n");
		
		/* Clear the  EXTI line 8 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line5);
		
		*((volatile unsigned int*)0xff) = 0xff;
	}
}

void EXTI15_10_IRQHandler(void)
{
	printf("EXTI15_10_IRQHandler\n");
	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{
		printf("Key1 Pressed\n");
		
		/* Clear the  EXTI line 8 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

void TIM2_IRQHandler(void)
{
//	printf("TIM2_IRQHandler\n");
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		printf("TIM_IT_Update\n");
	}
	
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		/* Pin PC.06 toggling with frequency = 73.24 Hz */
		GPIO_WriteBit(GPIOC, GPIO_Pin_6, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_6)));
		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + CCR1_Val);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

		/* Pin PC.07 toggling with frequency = 109.8 Hz */
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + CCR2_Val);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

		/* Pin PC.08 toggling with frequency = 219.7 Hz */
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8)));
		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + CCR3_Val);
	}
	else
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

		/* Pin PC.09 toggling with frequency = 439.4 Hz */
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9)));
		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + CCR4_Val);
	}
}



void TIM3_IRQHandler(void)
{
	printf("TIM3_IRQHandler()\n");
	
	/* TIM3_CH1 toggling with frequency = 183.1 Hz */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{
		printf("TIM3_IRQHandler(): TIM_IT_CC1\n");
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 );
		capture = TIM_GetCapture1(TIM3);
		TIM_SetCompare1(TIM3, capture + CCR1_Val );
	}

	/* TIM3_CH2 toggling with frequency = 366.2 Hz */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		capture = TIM_GetCapture2(TIM3);
		TIM_SetCompare2(TIM3, capture + CCR2_Val);
	}

	/* TIM3_CH3 toggling with frequency = 732.4 Hz */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		capture = TIM_GetCapture3(TIM3);
		TIM_SetCompare3(TIM3, capture + CCR3_Val);
	}

	/* TIM3_CH4 toggling with frequency = 1464.8 Hz */
	if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
		capture = TIM_GetCapture4(TIM3);
		TIM_SetCompare4(TIM3, capture + CCR4_Val);
	}
}



void TIM4_IRQHandler(void)
{
	
//	printf("TIM4_IRQHandler()\n");
//	printf("TIM4->SR = 0x%x\n", TIM4->SR);
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
//		printf("TIM4_IRQHandler(): TIM_IT_Update\n");
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(captureFlag == 2)
		{
			captureCount1++;
		}
		else if(captureFlag == 3)
		{
			captureCount2++;
		}
		
		//printf("TIM_IT_Update\n");
	}
	
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
		
		if((captureFlag == 1))				//1. Capture First Rising
		{
			captureFlag = 2;
			
			captureValue1 = TIM_GetCapture1(TIM4);
			
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Falling); 
		}
		else if(captureFlag == 2)			//2. Captured falling
		{
			captureFlag = 3;
			
			captureValue2 = TIM_GetCapture1(TIM4);
			
			TIM_OC1PolarityConfig(TIM4, TIM_ICPolarity_Rising); 
		}
		else if(captureFlag == 3)			//3. Captured Second Rising, Stop Capturing
		{
			captureFlag = 4;
			
			captureValue3 = TIM_GetCapture1(TIM4);
			
			TimerCaptureStop();
		}
	}
	else if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);

	}
	else if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	}
	else
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
	}
}


void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET)
	{    
		if(CaptureNumber == 0)
		{
			/* Get the Input Capture value */
			IC1ReadValue1 = TIM_GetCapture4(TIM5);
		}
		else if(CaptureNumber == 1)
		{
			/* Get the Input Capture value */
			IC1ReadValue2 = TIM_GetCapture4(TIM5); 

			/* Capture computation */
			if (IC1ReadValue2 > IC1ReadValue1)
			{
				Capture = (IC1ReadValue2 - IC1ReadValue1); 
			}
			else
			{
				Capture = ((0xFFFF - IC1ReadValue1) + IC1ReadValue2); 
			}
			/* Frequency computation */ 
			LsiFreq = (uint32_t) SystemCoreClock / Capture;
			LsiFreq *= 8;
		}

		CaptureNumber++;

		/* Clear TIM5 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);
	}
}


void TIM1_TRG_COM_IRQHandler(void)
{
	printf("TIM1_TRG_COM_IRQHandler\n");
	
	/* Clear TIM1 COM pending bit */
	TIM_ClearITPendingBit(TIM1, TIM_IT_COM);

	if (step == 1)
	{
		/* Next step: Step 2 Configuration ---------------------------- */
		/*  Channel3 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

		/*  Channel1 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

		/*  Channel2 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
		
		step++;
	}
	else if (step == 2)
	{
		/* Next step: Step 3 Configuration ---------------------------- */
		/*  Channel2 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);

		/*  Channel3 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

		/*  Channel1 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
		
		step++;
	}
	else if (step == 3)
	{
		/* Next step: Step 4 Configuration ---------------------------- */
		/*  Channel3 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

		/*  Channel2 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

		/*  Channel1 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
		
		step++;
	}
	else if (step == 4)
	{
		/* Next step: Step 5 Configuration ---------------------------- */
		/*  Channel3 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);

		/*  Channel1 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);

		/*  Channel2 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
		
		step++;
	}
	else if (step == 5)
	{
		/* Next step: Step 6 Configuration ---------------------------- */
		/*  Channel3 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

		/*  Channel1 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);

		/*  Channel2 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
		
		step++;
	}
	else
	{
		/* Next step: Step 1 Configuration ---------------------------- */
		/*  Channel1 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);

		/*  Channel3 configuration */
		TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
		TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);

		/*  Channel2 configuration */
		TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
		
		step = 1;
	}
}


/**
  * @brief  This function handles USART3 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		RxBuffer2[RxCounter++] = USART_ReceiveData(USART3);

		if(RxCounter == NbrOfDataToRead)
		{
			/* Disable the USART3 Receive interrupt */
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
		}
	}
}






void ADC1_2_IRQHandler(void)
{
	
}



void SDIO_IRQHandler(void) 
{
	SD_ProcessIRQSrc();
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
