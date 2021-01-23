#include "uart_nano.h"
#include <stdio.h>

#include "string.h"


uint8_t Uart2_DMA_Buffer[ Uart2_NANO_SIZE ] = {0};//dma buff addr

/**
  * @��  ��  ���ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
void UART_NANO_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//**USART����******
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //�򿪴���GPIO��ʱ��

	//��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //�򿪴��������ʱ��

	//����USART����
	USART_InitStructure.USART_BaudRate = baud;		//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	//����USARTΪ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //�������ȼ�	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);//��ʼ������NVIC

	//ʹ�ܴ���idle�ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	//DMA���������ж�
	USART_DMACmd( USART2, USART_DMAReq_Rx, ENABLE );
//	USART_DMACmd( USART2, USART_DMAReq_Tx, ENABLE );
	
	//DMA configure
	UART_DMA_Init();
	
	//ʹ�� USART�� �������
	USART_Cmd(USART2, ENABLE);
}
void UART_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );                            /* ʹ��DMA���� */
	DMA_DeInit( DMA1_Channel6 );                                                          /* ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ */
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (uint32_t)&(USART2->DR);                         /* DMA����ADC����ַ */
	DMA_InitStructure.DMA_MemoryBaseAddr		= (uint32_t)Uart2_DMA_Buffer;                         /* DMA�ڴ����ַ */
	DMA_InitStructure.DMA_DIR			= DMA_DIR_PeripheralSRC;        /* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
	DMA_InitStructure.DMA_BufferSize		= Uart2_NANO_SIZE;                            /* DMAͨ����DMA����Ĵ�С */
	DMA_InitStructure.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;    /* �����ַ�Ĵ������� */
	DMA_InitStructure.DMA_MemoryInc			= DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ĵ������� */
	DMA_InitStructure.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;  /* ���ݿ��Ϊ8λ */
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;      /* ���ݿ��Ϊ8λ */
	DMA_InitStructure.DMA_Mode			= DMA_Mode_Circular;              /*  */
	DMA_InitStructure.DMA_Priority			= DMA_Priority_VeryHigh;          /* DMAͨ�� xӵ�������ȼ� */
	DMA_InitStructure.DMA_M2M			= DMA_M2M_Disable;              /* DMAͨ��xû������Ϊ�ڴ浽�ڴ洫�� */
	
	DMA_Init( DMA1_Channel6, &DMA_InitStructure );                                        /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Rx_DMA_Channel����ʶ�ļĴ��� */
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

uint8_t RecFlag;
uint16_t RecCnt;
float chassis_speed_target[3] = {0};
void USART2_IRQHandler(void)            
{
	uint16_t uartRecCnt=0;

	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART2);
		uartRecCnt = sizeof(Uart2_DMA_Buffer) - DMA_GetCurrDataCounter(DMA1_Channel6); 
		//**********************//
		RecFlag=1;
		RecCnt=uartRecCnt;
		//printf("recv data0 = %d, data1 = %d, data2 = %d, data3 = %d\r\n", Uart2_DMA_Buffer[0], Uart2_DMA_Buffer[1], Uart2_DMA_Buffer[2], Uart2_DMA_Buffer[3]);
		chassis_speed_target[0] = (int16_t)((Uart2_DMA_Buffer[0]<<8) | Uart2_DMA_Buffer[1]);
		chassis_speed_target[1] = (int16_t)((Uart2_DMA_Buffer[2]<<8) | Uart2_DMA_Buffer[3]);
		chassis_speed_target[2] = (int16_t)((Uart2_DMA_Buffer[4]<<8) | Uart2_DMA_Buffer[5]);

//		printf("speed_target x:%f, y:%f, z:%f \r\n", chassis_speed_target[0], chassis_speed_target[1], chassis_speed_target[2]);
		//clean
		DMA_Cmd(DMA1_Channel6, DISABLE ); 
		DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(Uart2_DMA_Buffer));
		DMA_Cmd(DMA1_Channel6, ENABLE);     
		
//		memset(chassis_speed_target, 0, 100);
		
		USART_ClearITPendingBit(USART2,USART_IT_IDLE);    
	}
}

static uint8_t uart2_tx_buf[60] = {0};  //���ͻ���
void UART_NANO_SendPacket(uint8_t *pbuf, uint8_t len)
{
	uint8_t i;	

	for(i=0; i<len; i++)
	{
		uart2_tx_buf[i] = *(pbuf+i);
		USART_SendData(USART2, uart2_tx_buf[i]);
		printf("send data[%d]: %d  \r\n", i, uart2_tx_buf[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
	}
}
