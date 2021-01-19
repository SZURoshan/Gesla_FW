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
	
	xTaskCreate(startTask, "START_TASK", 100, NULL, 2, &startTaskHandle);/*������ʼ����*/
	vTaskStartScheduler();/*�����������*/
	
	while(1){};/* ������Ⱥ󲻻�ִ�е��� */
}

void test_task(void* param)
{
	for(;;)
	{
		vTaskDelay(100);
		printf("task running \r\n");
	}
}

/*��������*/
void startTask(void *param)
{
	taskENTER_CRITICAL();	/*�����ٽ���*/

	xTaskCreate(test_task, "test_task", 100, NULL, 6, NULL);
	vTaskDelete(startTaskHandle);									/*ɾ����ʼ����*/
	
	taskEXIT_CRITICAL();	/*�˳��ٽ���*/
}

