#ifndef __UART_NANO_H
#define __UART_NANO_H

#define    Uart2_NANO_SIZE           20   //��΢�����

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

extern uint8_t Uart2_DMA_Buffer[ Uart2_NANO_SIZE ];
extern float chassis_speed_target[3];

void UART_NANO_Init(uint32_t baud);
void UART_DMA_Init(void);
void UART_NANO_SendPacket(uint8_t *pbuf, uint8_t len);

#endif 
