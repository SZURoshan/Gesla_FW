#include "uart_db.h"
#include <stdio.h>
#include <string.h>


/* BSP */
#include "bootload_jump.h"

//static uint8_t uart_db_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
#define UART1_DMA_SIZE 20
uint8_t Uart1_DMA_Buffer[ UART1_DMA_SIZE ] = {0};//dma buff addr

/**
  * @简  述  UART DB调试串口初始化
  * @参  数  baud： 波特率设置
  * @返回值	 无
  */
void UART_DB_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//**调试串口USART配置******
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //打开串口外设的时钟
	
	//将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置USART参数
	USART_InitStructure.USART_BaudRate = baud; //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	//配置USART为中断源
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢断优先级	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断
	NVIC_Init(&NVIC_InitStructure);//初始化配置NVIC
	
	//使能串口接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	USART_DMACmd( USART1, USART_DMAReq_Rx, ENABLE );
	
	//DMA configure
	UART1_DMA_Init();
	
	//使能 USART， 配置完毕
	USART_Cmd(USART1, ENABLE);
}

void UART1_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );                            /* 使能DMA传输 */
	
	/* Reset DMAy Channelx remaining bytes register 这样就不会往后塞，直接覆盖*/
	DMA_DeInit( DMA1_Channel5 );                                                          /* 将DMA的通道1寄存器重设为缺省值 */
	
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (uint32_t)&(USART1->DR);                         /* DMA外设ADC基地址 */
	DMA_InitStructure.DMA_MemoryBaseAddr		= (uint32_t)Uart1_DMA_Buffer;                         /* DMA内存基地址 */
	DMA_InitStructure.DMA_DIR			        = DMA_DIR_PeripheralSRC;        /* 数据传输方向，从内存读取发送到外设 */
	DMA_InitStructure.DMA_BufferSize		    = UART1_DMA_SIZE;                            /* DMA通道的DMA缓存的大小 */
	DMA_InitStructure.DMA_PeripheralInc		    = DMA_PeripheralInc_Disable;    /* 外设地址寄存器不变 */
	DMA_InitStructure.DMA_MemoryInc			    = DMA_MemoryInc_Enable;         /* 内存地址寄存器递增 */
	DMA_InitStructure.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;  /* 数据宽度为8位 */
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;      /* 数据宽度为8位 */
	DMA_InitStructure.DMA_Mode			        = DMA_Mode_Circular;              /*  */
	DMA_InitStructure.DMA_Priority			    = DMA_Priority_VeryHigh;          /* DMA通道 x拥有中优先级 */
	DMA_InitStructure.DMA_M2M			        = DMA_M2M_Disable;              /* DMA通道x没有设置为内存到内存传输 */
	
	DMA_Init( DMA1_Channel5, &DMA_InitStructure );                                        /* 根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Rx_DMA_Channel所标识的寄存器 */
	DMA_Cmd( DMA1_Channel5, ENABLE );
}

/**
  * @简  述  DBUART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
char Return_BL_Cmd[] = {"sanger"};
void USART1_IRQHandler(void)
{
	uint16_t uartRecCnt=0;
	char cmp_temp[UART1_DMA_SIZE] = {0};
	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //接收中断
	{
		USART_ReceiveData(USART1);/* 清空闲中断标志位 */
		
		DMA_Cmd(DMA1_Channel5, DISABLE ); 
		uartRecCnt = sizeof(Uart1_DMA_Buffer) - DMA_GetCurrDataCounter(DMA1_Channel5);
		DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(Uart1_DMA_Buffer));
		
		/* Reset DMAy Channelx remaining bytes register */
		UART1_DMA_Init(); //reinit
		
		DMA_Cmd(DMA1_Channel5, ENABLE); 
		
		memcpy(cmp_temp, Uart1_DMA_Buffer, uartRecCnt);
		
		if( strcmp(Return_BL_Cmd, cmp_temp) == 0)//比较一下是否是进BL的指令
		{
			//printf("%s , uartRecCnt: %d \r\n", Uart1_DMA_Buffer, uartRecCnt);
			
			if(((*(vu32*)(STM32_FLASH_BASE+4))&0xFF000000)==0x08000000)//判断BL里面主程序开始地址是否合法，是否为0X08XXXXXX.
			{
				printf("return to BL: 0x%08X...\r\n", STM32_FLASH_BASE);
				iap_load_app(STM32_FLASH_BASE);//跳回BL
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


/**************************串口打印相关函数重定义********************************/
/**
  * @简  述  重定义putc函数（USART1）	
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
  * @简  述  重定义getc函数（USART1）	
  */
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART1);
}
