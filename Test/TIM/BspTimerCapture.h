#ifndef BSP_TIMER_CAPTURE_H_
#define BSP_TIMER_CAPTURE_H_


void TimerCaptureInit(void);

void TimerCaptureStart(void);

void TimerCaptureStop(void);

void CaptureCaculate(unsigned int* pFrequency, unsigned int* pHighLevelUs, unsigned int* pLowLevelUs);

#endif
