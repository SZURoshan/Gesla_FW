#ifndef __UART_DB_H
#define __UART_DB_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


void UART_DB_Init(uint32_t baud);  //UART ���Դ��ڳ�ʼ��
uint8_t UART_DB_GetData(uint8_t *pbuf);  //UART ��ȡ���յ�����

#endif 
