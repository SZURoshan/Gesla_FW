#include "sysinit.h"
#include <stdio.h>

/* BSP */
#include "uart_db.h"
#include "uart_nano.h"
#include "led.h"
#include "vin.h"

/* Driver */
#include "motor.h"
#include "motor_encoder.h"

void System_Init(void)
{
	UART_DB_Init(115200);
	UART_NANO_Init(115200);
	
	Motor_Init(10);
	

	//motor encoder initialize
	Moter_Encoder_AB_Init(MOTOR_ENCODER_MID_VALUE*2);  
	Moter_Encoder_CD_Init(MOTOR_ENCODER_MID_VALUE*2);  
	Moter_Encoder_EF_Init(MOTOR_ENCODER_MID_VALUE*2);  
	Moter_Encoder_GH_Init(MOTOR_ENCODER_MID_VALUE*2);  
	//Sets the encoder median
	Moter_Encoder_AB_SetCounter(MOTOR_ENCODER_MID_VALUE); 
	Moter_Encoder_CD_SetCounter(MOTOR_ENCODER_MID_VALUE); 
	Moter_Encoder_EF_SetCounter(MOTOR_ENCODER_MID_VALUE); 
	Moter_Encoder_GH_SetCounter(MOTOR_ENCODER_MID_VALUE);
	
	Vin_Init();
	LED_Init();
		
	printf("System init... \r\n");
}

int16_t constrain_int16_t(int16_t amt, int16_t low, int16_t high)
{
	if (amt < low)
			return low;
	else if (amt > high)
			return high;
	else
			return amt;
}

float constrain_float(float amt, float low, float high)
{
	if (amt < low)
			return low;
	else if (amt > high)
			return high;
	else
			return amt;
}

int32_t constrain_int32_t(int32_t amt, int32_t low, int32_t high)
{
	if (amt < low)
			return low;
	else if (amt > high)
			return high;
	else
			return amt;
}
