
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include <stdio.h>

#include "BspTimerCapture.h"





/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_ICInitTypeDef  TIM_ICInitStructure;
static __IO uint16_t CCR1_Val = 40961;
static __IO uint16_t CCR2_Val = 27309;
static __IO uint16_t CCR3_Val = 13654;
static __IO uint16_t CCR4_Val = 6826;
static uint16_t PrescalerValue = 0;


volatile unsigned char captureFlag = 0;

volatile unsigned int captureCount1 = 0;
volatile unsigned int captureCount2 = 0;

volatile unsigned int captureValue1 = 0;
volatile unsigned int captureValue2 = 0;
volatile unsigned int captureValue3 = 0;


/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void TimerCaptureInit(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     
	   
	/* System Clocks Configuration */
	RCC_Configuration();

	/* NVIC Configuration */
	NVIC_Configuration();

	/* GPIO Configuration */
	GPIO_Configuration();

	/* ---------------------------------------------------------------
	TIM4 Configuration: Output Compare Timing Mode:
	TIM4 counter clock at 6 MHz
	CC1 update rate = TIM4 counter clock / CCR1_Val = 146.48 Hz
	CC2 update rate = TIM4 counter clock / CCR2_Val = 219.7 Hz
	CC3 update rate = TIM4 counter clock / CCR3_Val = 439.4 Hz
	CC4 update rate = TIM4 counter clock / CCR4_Val = 878.9 Hz
	--------------------------------------------------------------- */

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);

	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; 					// Channel Index
  	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			// Rising Capture
  	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 	// Directly mapping
  	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 			//
  	TIM_ICInitStructure.TIM_ICFilter = 0x03;							//IC1F = 0000
  	
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	
//	TIM_CtrlPWMOutputs(TIM4, ENABLE);

	/* TIM IT enable */
//	TIM_ITConfig(TIM4, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

//	TIM_ARRPreloadConfig(TIM4, ENABLE);


//	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1, ENABLE)
	
	/* TIM4 enable counter */
//	TIM_Cmd(TIM4, ENABLE);

}

void TimerCaptureStart(void)
{
//	printf("TimerCaptureStart()\n");
//	printf("captureFlag = %d\n", captureFlag);
	
	if((captureFlag == 0) || (captureFlag == 4))
	{
		captureFlag = 1;		// Start flag

		captureCount1 = 0;
		captureCount2 = 0;

		captureValue1 = 0;
		captureValue2 = 0;
		captureValue3 = 0;
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update | TIM_IT_CC1);
		
		TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1, ENABLE);
		
		TIM_Cmd(TIM4, ENABLE);
	}
}

void TimerCaptureStop(void)
{
	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1, DISABLE);
	
	TIM_Cmd(TIM4, DISABLE);
}

void CaptureCaculate(unsigned int* pFrequency, unsigned int* pHighLevelUs, unsigned int* pLowLevelUs)
{
	printf("captureCount1 = %d\n", captureCount1);
	printf("captureCount2 = %d\n", captureCount2);
	
	printf("captureValue1 = %d\n", captureValue1);
	printf("captureValue2 = %d\n", captureValue2);
	printf("captureValue3 = %d\n", captureValue3);
	
	
	if(captureFlag == 4) //Capture success
	{
		*pHighLevelUs = (0xffff * captureCount1) - captureValue1 + captureValue2;
		
		*pLowLevelUs = (0xffff * captureCount2) - captureValue2 + captureValue3;
		
		*pFrequency = 1000000 / ((*pHighLevelUs) + (*pLowLevelUs));
	}
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
	/* PCLK1 = HCLK/4 */
//	RCC_PCLK1Config(RCC_HCLK_Div4);

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

/**
  * @brief  Configure the GPIO Pins.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOA Configuration:Pin 0, 1, 2 and 3 as input push-donw */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM4 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
