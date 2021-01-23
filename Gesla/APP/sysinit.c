#include "sysinit.h"
#include <stdio.h>

/* BSP */
#include "uart_db.h"
#include "uart_nano.h"
#include "led.h"
#include "vin.h"

/* Driver */
#include "motor.h"

void System_Init(void)
{
	UART_DB_Init(115200);
	UART_NANO_Init(115200);
	
	Vin_Init();
	LED_Init();
	
	Motor_Init(20);
	printf("Motor init... \r\n");
}
