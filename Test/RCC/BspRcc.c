
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include <stdio.h>

#include "BspRcc.h"


RCC_ClocksTypeDef rccClk;

void RccTest(void)
{
	RCC_GetClocksFreq(&rccClk);
	printf("----------------------RCC---------------------\n");
	printf("rccClk.SYSCLK_Frequency = %d Hz\n", rccClk.SYSCLK_Frequency);
	printf("rccClk.HCLK_Frequency = %d Hz\n", rccClk.HCLK_Frequency);
	printf("rccClk.PCLK1_Frequency = %d Hz\n", rccClk.PCLK1_Frequency);
	printf("rccClk.PCLK2_Frequency = %d Hz\n", rccClk.PCLK2_Frequency);
	printf("rccClk.ADCCLK_Frequency = %d Hz\n", rccClk.ADCCLK_Frequency);
	printf("----------------------RCC---------------------\n");
}
