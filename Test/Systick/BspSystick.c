#include "stm32_eval.h"
#include "BspSystick.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SysTick_TimeBase
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
void Delay(volatile uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

void DumpSystickRegs(void)
{
	
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void SystickInit(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32f10x_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f10x.c file
	*/     

	/* Setup SysTick Timer for 1 msec interrupts.
	------------------------------------------
	1. The SysTick_Config() function is a CMSIS function which configure:
	- The SysTick Reload register with value passed as function parameter.
	- Configure the SysTick IRQ priority to the lowest value (0x0F).
	- Reset the SysTick Counter register.
	- Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
	- Enable the SysTick Interrupt.
	- Start the SysTick Counter.

	2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
	SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
	inside the misc.c file.

	3. You can change the SysTick IRQ priority by calling the
	NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
	call. The NVIC_SetPriority() is defined inside the core_cm3.h file.

	4. To adjust the SysTick time base, use the following formula:
						
	 Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)

	- Reload Value is the parameter to be passed for SysTick_Config() function
	- Reload Value should not exceed 0xFFFFFF
	*/
	if (SysTick_Config((SystemCoreClock / 1000) / 8))
	{ 
		/* Capture error */ 
		while (1);
	}
	
	  /* Select AHB clock(HCLK) divided by 8 as SysTick clock source */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(volatile uint32_t nTime)
{ 
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
