#ifndef BSP_ADC_SINGLE_H_
#define BSP_ADC_SINGLE_H_

void BspAdcSingle(void);


u16 BspGetAdcVal(u8 ch);

u16 BspGetAdcAverageVal(u8 ch, u8 times);

#endif
