
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include <stdio.h>

#include "BspIwdg.h"




/** @addtogroup IWDG_Reset
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment/Comment depending on your STM32 device. 
   The LSI is internally connected to TIM5 IC4 only on STM32F10x Connectivity 
   line, High-Density Value line, High-Density and XL-Density Devices */
   
#define LSI_TIM_MEASURE

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__IO uint32_t TimingDelay = 0;
__IO uint32_t LsiFreq = 40000;
extern __IO uint16_t CaptureNumber;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
void TIM5_ConfigForLSI(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void IWDGInit(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32f10x_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f10x.c file
	*/     

	/* Initialize LED1 and Key Button mounted on STM3210X-EVAL board */       


	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
		/* Capture error */ 
		while (1);
	}


	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("IWDGRST flag set\n");

		/* Clear reset flags */
		RCC_ClearFlag();
	}
	else
	{
		/* IWDGRST flag is not set */
		/* Turn off LED1 */
		printf("IWDGRST flag not set\n");
	}

#ifdef LSI_TIM_MEASURE
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}

	/* TIM Configuration -------------------------------------------------------*/
	TIM5_ConfigForLSI();

	/* Wait until the TIM5 get 2 LSI edges */
	while(CaptureNumber != 2)
	{
	}
	
	/* Disable TIM5 CC4 Interrupt Request */
	TIM_ITConfig(TIM5, TIM_IT_CC4, DISABLE);
#endif

	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
	dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: LSI/32 */
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	Counter Reload Value = 250ms/IWDG counter clock period
						  = 250ms / (LSI/32)
						  = 0.25s / (LsiFreq/32)
						  = LsiFreq/(32 * 4)
						  = LsiFreq/128
	*/
	
	printf("LsiFreq = %d\n", LsiFreq);
	
	IWDG_SetReload(LsiFreq / 128);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}

#ifdef LSI_TIM_MEASURE
/**
  * @brief  Configures TIM5 to measure the LSI oscillator frequency.
  * @param  None
  * @retval None
  */
void TIM5_ConfigForLSI(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;

	/* Enable TIM5 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Enable the TIM5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure TIM5 prescaler */
	TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);

	/* Connect internally the TM5_CH4 Input Capture to the LSI clock output */
	GPIO_PinRemapConfig(GPIO_Remap_TIM5CH4_LSI, ENABLE);

	/* TIM5 configuration: Input Capture mode ---------------------
	 The LSI oscillator is connected to TIM5 CH4
	 The Rising edge is used as active edge,
	 The TIM5 CCR4 is used to compute the frequency value 
	------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	/* TIM10 Counter Enable */
	TIM_Cmd(TIM5, ENABLE);

	/* Reset the flags */
	TIM5->SR = 0;

	/* Enable the CC4 Interrupt Request */  
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);  
}
#endif
