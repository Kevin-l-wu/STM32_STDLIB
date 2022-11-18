#ifndef BSP_ADC_DISC_H_
#define BSP_ADC_DISC_H_

void BspAdcDisc(void);


u16 BspGetAdcVal(u8 ch);

u16 BspGetAdcAverageVal(u8 ch, u8 times);

#endif
