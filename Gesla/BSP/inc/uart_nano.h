#ifndef __UART_NANO_H
#define __UART_NANO_H

#define    Uart2_NANO_SIZE           5   //ÉÔÎ¢¸ø´óµã

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"


void UART_NANO_Init(uint32_t baud);
void UART_DMA_Init(void);

extern uint8_t Uart2_DMA_Buffer[ Uart2_NANO_SIZE ];

#endif 
