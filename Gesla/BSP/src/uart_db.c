#include "uart_db.h"
#include <stdio.h>


static uint8_t uart_db_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte

/**
  * @��  ��  UART DB���Դ��ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
void UART_DB_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//**���Դ���USART����******
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //�򿪴���GPIO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //�򿪴��������ʱ��
	
	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//����USART����
	USART_InitStructure.USART_BaudRate = baud; //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	//����USARTΪ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //�������ȼ�	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);//��ʼ������NVIC
	
	//ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//ʹ�� USART�� �������
	USART_Cmd(USART1, ENABLE);
}

/**
  * @��  ��  DBUART �����жϷ�����
  * @��  ��  �� 
  * @����ֵ  ��
  */
void USART1_IRQHandler(void)
{
//	uint8_t Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
//		Res =USART_ReceiveData(USART1);	
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	} 
}

static uint8_t uart_db_flag_rx_ok = 0; //���ճɹ���־
/**
  * @��  ��  ��ȡ���յ�����
  * @��  ��  *pbuf����������ָ��,��1���ֽ�Ϊ֡���룬����Ϊ����
  * @����ֵ	 0-�����ݽ��գ�other-��Ҫ��ȡ�������ֽڸ���
  */
uint8_t UART_DB_GetData(uint8_t *pbuf)
{
	uint8_t cnt,i;
	
	if(uart_db_flag_rx_ok != 0)
	{
		cnt = uart_db_rx_buf[2]-4;
		
		for(i=0; i<cnt; i++)
		{
			*(pbuf+i) = uart_db_rx_buf[3+i];
		}
		
		uart_db_flag_rx_ok = 0;
		return cnt;
	}
	else
	{
		return 0;
	}	
}



/**************************���ڴ�ӡ��غ����ض���********************************/
/**
  * @��  ��  �ض���putc������USART1��	
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/**
  * @��  ��  �ض���getc������USART1��	
  */
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART1);
}
