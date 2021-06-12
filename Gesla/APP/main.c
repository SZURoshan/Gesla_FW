/*
** A single spark can start a prairie fire.
*/

#include "stm32f10x.h"
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* APP */
#include "sysinit.h"

/* TASK */
#include "Task_Start.h"

int main(void)
{	
	SCB->VTOR = FLASH_BASE | 0x4000;
	
	System_Init();

	App_Task_Create();
	
	vTaskStartScheduler();
	
	while(1){};
}
