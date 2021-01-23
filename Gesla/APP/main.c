#include "stm32f10x.h"
#include <stdio.h>

/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"

/* APP */
#include "sysinit.h"

/* TASK */
#include "Task_Start.h"

int main(void)
{	
	System_Init();

	App_Task_Create();
	
	vTaskStartScheduler();
	
	while(1){};
}
