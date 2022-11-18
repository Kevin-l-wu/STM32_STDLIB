#ifndef BSP_SYSTICK_H_
#define BSP_SYSTICK_H_

void SystickInit(void);

void Delay(volatile uint32_t nTime);

void TimingDelay_Decrement(void);


#endif
