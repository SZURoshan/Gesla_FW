#include "uart_db.h"
#include <stdio.h>
#include <string.h>


/* BSP */
#include "bootload_jump.h"

//static uint8_t uart_db_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte
#define UART1_DMA_SIZE 20
uint8_t Uart1_DMA_Buffer[ UART1_DMA_SIZE ] = {0};//dma buff addr

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
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	USART_DMACmd( USART1, USART_DMAReq_Rx, ENABLE );
	
	//DMA configure
	UART1_DMA_Init();
	
	//ʹ�� USART�� �������
	USART_Cmd(USART1, ENABLE);
}

void UART1_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );                            /* ʹ��DMA���� */
	
	/* Reset DMAy Channelx remaining bytes register �����Ͳ�����������ֱ�Ӹ���*/
	DMA_DeInit( DMA1_Channel5 );                                                          /* ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ */
	
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (uint32_t)&(USART1->DR);                         /* DMA����ADC����ַ */
	DMA_InitStructure.DMA_MemoryBaseAddr		= (uint32_t)Uart1_DMA_Buffer;                         /* DMA�ڴ����ַ */
	DMA_InitStructure.DMA_DIR			        = DMA_DIR_PeripheralSRC;        /* ���ݴ��䷽�򣬴��ڴ��ȡ���͵����� */
	DMA_InitStructure.DMA_BufferSize		    = UART1_DMA_SIZE;                            /* DMAͨ����DMA����Ĵ�С */
	DMA_InitStructure.DMA_PeripheralInc		    = DMA_PeripheralInc_Disable;    /* �����ַ�Ĵ������� */
	DMA_InitStructure.DMA_MemoryInc			    = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ĵ������� */
	DMA_InitStructure.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;  /* ���ݿ��Ϊ8λ */
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;      /* ���ݿ��Ϊ8λ */
	DMA_InitStructure.DMA_Mode			        = DMA_Mode_Circular;              /*  */
	DMA_InitStructure.DMA_Priority			    = DMA_Priority_VeryHigh;          /* DMAͨ�� xӵ�������ȼ� */
	DMA_InitStructure.DMA_M2M			        = DMA_M2M_Disable;              /* DMAͨ��xû������Ϊ�ڴ浽�ڴ洫�� */
	
	DMA_Init( DMA1_Channel5, &DMA_InitStructure );                                        /* ����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Rx_DMA_Channel����ʶ�ļĴ��� */
	DMA_Cmd( DMA1_Channel5, ENABLE );
}

/**
  * @��  ��  DBUART �����жϷ�����
  * @��  ��  �� 
  * @����ֵ  ��
  */
char Return_BL_Cmd[] = {"sanger"};
void USART1_IRQHandler(void)
{
	uint16_t uartRecCnt=0;
	char cmp_temp[UART1_DMA_SIZE] = {0};
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //�����ж�
	{
		USART_ReceiveData(USART1);/* ������жϱ�־λ */
		
		DMA_Cmd(DMA1_Channel5, DISABLE ); 
		uartRecCnt = sizeof(Uart1_DMA_Buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(Uart1_DMA_Buffer));
		
		/* Reset DMAy Channelx remaining bytes register */
		UART1_DMA_Init(); //reinit
		
		DMA_Cmd(DMA1_Channel5, ENABLE); 
		
		memcpy(cmp_temp, Uart1_DMA_Buffer, uartRecCnt);
		
		if( strcmp(Return_BL_Cmd, cmp_temp) == 0)//�Ƚ�һ���Ƿ��ǽ�BL��ָ��
		{
			//printf("%s , uartRecCnt: %d \r\n", Uart1_DMA_Buffer, uartRecCnt);
			
			if(((*(vu32*)(STM32_FLASH_BASE+4))&0xFF000000)==0x08000000)//�ж�BL����������ʼ��ַ�Ƿ�Ϸ����Ƿ�Ϊ0X08XXXXXX.
			{
				printf("return to BL: 0x%08X...\r\n", STM32_FLASH_BASE);
				iap_load_app(STM32_FLASH_BASE);//����BL
			}
			else
			{
				printf("illegal BL ADDR: %08x \r\n", ((*(vu32*)(STM32_FLASH_BASE+4))&0xFF000000));
			}
		}
		else
		{
			printf("strcmp fail... \r\n");
		}
		
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
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
