#include "stm32f10x.h"
#include <stdio.h>

/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"
#include "uart_db.h"
#include "uart_nano.h"

static TaskHandle_t startTaskHandle;
static void startTask(void *param);

int main(void)
{	
	UART_DB_Init(115200);
	UART_NANO_Init(115200);
	
	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle);/*创建起始任务*/
	vTaskStartScheduler();/*开启任务调度*/
	
	while(1){};/* 任务调度后不会执行到这 */
}
uint8_t send_test[4] = {0};
void test_task(void* param)
{
	for(;;)
	{
		vTaskDelay(1000);
		printf("task running \r\n");
		
		//send test
		send_test[0] = send_test[3]+2;
		send_test[1] = send_test[0]+2;
		send_test[2] = send_test[1]+2;
		send_test[3] = send_test[2]+2;
		UART_NANO_SendPacket(send_test, 4);
	}
}

/*创建任务*/
void startTask(void *param)
{
	taskENTER_CRITICAL();	/*进入临界区*/

	xTaskCreate(test_task, "test_task", 100, NULL, 6, NULL);
	vTaskDelete(startTaskHandle);									/*删除开始任务*/
	
	taskEXIT_CRITICAL();	/*退出临界区*/
}

