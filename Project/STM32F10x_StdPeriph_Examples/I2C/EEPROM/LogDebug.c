#include <stdio.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"


#include "LogDebug.h"





#pragma import(__use_no_semihosting)             
                
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
   
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕   
    
	USART1->DR = (u8) ch;      
	
	return ch;
}





void LogDebugInit(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
//	NVIC_InitTypeDef NVIC_InitStrue;
	
	//1. Enable GPIO and UART1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//2. Set GPIOA9 AF function(UART1 txd)
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStrue);
	
	//3. Set GPIOA10 AF function(UART1 txd)
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_10MHz;
  
	GPIO_Init(GPIOA, &GPIO_InitStrue);//②
	
	//4. Set Uart1' parameters, including baudrate etc
	USART_InitStrue.USART_BaudRate = 115200;
	USART_InitStrue.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStrue.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStrue.USART_Parity = USART_Parity_No;
	USART_InitStrue.USART_StopBits = USART_StopBits_1;
	USART_InitStrue.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1, &USART_InitStrue);//③
	
	USART_Cmd(USART1, ENABLE);//使能串口1
/*	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
	
	NVIC_InitStrue.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStrue);
*/
}

