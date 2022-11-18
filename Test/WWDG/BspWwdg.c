
#include "stm32_eval.h"
#include "stm32_eval_spi_flash.h"

#include "LogDebug.h"
#include <stdio.h>

#include "BspWwdg.h"

void BspWWDGInit(void)
{
	/*!< At this stage the microcontroller clock setting is already configured, 
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f10x_xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f10x.c file
	 */     



	/* Check if the system has resumed from WWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{ 
		/* WWDGRST flag set */
		printf("WWDGRST flag set\n");

		/* Clear reset flags */
		RCC_ClearFlag();
	}
	else
	{
		/* WWDGRST flag is not set */
		/* Turn off LED1 */
		printf("WWDGRST flag is not set\n");
	}

	/* WWDG configuration */
	/* Enable WWDG clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	/* On Value line devices, WWDG clock counter = (PCLK1 (24MHz)/4096)/8 = 732 Hz (~1366 us)  */
	/* On other devices, WWDG clock counter = (PCLK1(36MHz)/4096)/8 = 1099 Hz (~910 us)  */
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	/* Set Window value to 80; WWDG counter should be refreshed only when the counter
	is below 80 (and greater than 64) otherwise a reset will be generated */
	WWDG_SetWindowValue(120);

	/* - On Value line devices,
	Enable WWDG and set counter value to 127, WWDG timeout = ~1366 us * 64 = 87.42 ms 
	In this case the refresh window is: ~1366us * (127-80) = 64.20 ms < refresh window < ~1366us * 64 = 87.42ms
	 - On other devices
	Enable WWDG and set counter value to 127, WWDG timeout = ~910 us * 64 = 58.25 ms 
	In this case the refresh window is: ~910 us * (127-80) = 42.77 ms < refresh window < ~910 us * 64 = 58.25ms     
	*/
	WWDG_Enable(127);
}
