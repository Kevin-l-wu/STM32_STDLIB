#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"


#include "LogDebug.h"

/*

USART1:

USARTX_CK: 	PA8
USARTX_TX:	PA9
USARTX_RX: 	PA10
USARTX_CTS:	PA11
USARTX_RTS:	PA12


USART2:

USART2_CTS:	PA0
USART2_RTS:	PA1
USART2_TX:	PA2
USART2_RX: 	PA3
USART2_CK: 	PA4


USART3:
USART3_TX:	PB10
USART3_RX:	PB11

*/

typedef struct
{
	USART_TypeDef* 	pUsart;
	GPIO_TypeDef* 	pGpio;
	uint16_t		pinTx;
	uint16_t		pinRx;
	uint8_t			usartPos;	//APB1 or APB2
	uint8_t			gpioPos;	//APB1 or APB2
	uint32_t		rccUsartIndex;
	uint32_t		rccGPIOIndex;
} USARTx_STRUCT;


USARTx_STRUCT usartTable[] = {
	{USART1, GPIOA, GPIO_Pin_9, GPIO_Pin_10,  2, 2, RCC_APB2Periph_USART1, RCC_APB2Periph_GPIOA},
	{USART2, GPIOA, GPIO_Pin_2, GPIO_Pin_3,   1, 2, RCC_APB1Periph_USART2, RCC_APB2Periph_GPIOA},
	{USART3, GPIOB, GPIO_Pin_10, GPIO_Pin_11, 1, 2, RCC_APB1Periph_USART3, RCC_APB2Periph_GPIOB},
};

USARTx_STRUCT* pCurrentUsart = &usartTable[0];





#if 0
#pragma import(__use_no_semihosting)             
                
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;    
FILE __stdin;
   
_sys_exit(int x) 
{ 
	x = x; 
} 
#endif

int fputc(int ch, FILE *f)
{      
	while((pCurrentUsart->pUsart->SR & 0X40) == 0);//循环发送,直到发送完毕   
    
	pCurrentUsart->pUsart->DR = (u8) ch;      
	
	return ch;
}


int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(pCurrentUsart->pUsart, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(pCurrentUsart->pUsart);
}




void LogDebugInit(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
//	NVIC_InitTypeDef NVIC_InitStrue;

	USART_DeInit(pCurrentUsart->pUsart);
	
	GPIO_DeInit(pCurrentUsart->pGpio);
	
	//1. Enable GPIO and UART1 clock
	
	if(pCurrentUsart->usartPos == 1)	//USARTx is on APB1 Bus
	{
		RCC_APB1PeriphClockCmd(pCurrentUsart->rccUsartIndex, ENABLE);
	}
	else if( pCurrentUsart->usartPos == 2)//USARTx is on APB2 Bus
	{
		RCC_APB2PeriphClockCmd(pCurrentUsart->rccUsartIndex, ENABLE);
	}
	
	if(pCurrentUsart->gpioPos == 1)	//GPIOx is on APB1 Bus
	{
		RCC_APB1PeriphClockCmd(pCurrentUsart->rccGPIOIndex, ENABLE);
	}
	else if( pCurrentUsart->gpioPos == 2)//GPIOx is on APB2 Bus
	{
		RCC_APB2PeriphClockCmd(pCurrentUsart->rccGPIOIndex, ENABLE);
	}
	
	/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	*/
	
	//2. Set GPIOA9 AF function(UART1 txd)
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin = pCurrentUsart->pinTx;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(pCurrentUsart->pGpio, &GPIO_InitStrue);
	
	//3. Set GPIOA10 AF function(UART1 rxd)
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin = pCurrentUsart->pinRx;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
  
	GPIO_Init(pCurrentUsart->pGpio, &GPIO_InitStrue);//②
	
	//4. Set Uart1' parameters, including baudrate etc
	USART_InitStrue.USART_BaudRate = 115200;
	USART_InitStrue.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStrue.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStrue.USART_Parity = USART_Parity_No;
	USART_InitStrue.USART_StopBits = USART_StopBits_1;
	USART_InitStrue.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(pCurrentUsart->pUsart, &USART_InitStrue);//③
	
	USART_Cmd(pCurrentUsart->pUsart, ENABLE);//使能串口1
}

