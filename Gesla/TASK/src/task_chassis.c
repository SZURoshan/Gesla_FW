/*
*	l_front:	motor_A	input(+)back	input(-)front	O|----LIDAR(HEAD)----|O		r_front:	motor_B	input(+)back	input(-)front
*													 	 |---------^---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*													 	 |---------|---------|
*	l_back:		motor_C	input(+)front	input(-)back 	O|------ANTENNA------|O		r_back:		motor_D	input(+)front	input(-)back
*	
*/
#include "task_chassis.h"
#include <stdio.h>

#include "uart_nano.h"
#include "motor.h"

float constrain_float(float amt, float low, float high)
{
	if (amt < low)
			return low;
	else if (amt > high)
			return high;
	else
			return amt;
}

static float wheel_velocity[4] = {0};
float x_speed = 0;
float y_speed = 0;
float z_speed = 0;
//500Hz
void Task_Chassis(void *pvParameters)
{
	portTickType currentTime;	
	
	for(;;)
	{
		currentTime = xTaskGetTickCount();
		
		/* CODE */
		printf("Task_Chassis running ... \r\n");
//		Motor_D_SetSpeed(-300);
		
		//test
//		wheel_velocity[0] = -y_speed + x_speed - (wheel_track_cali)*yaw_speed;
//		wheel_velocity[1] = y_speed + x_speed + (wheel_track_cali)*yaw_speed;
//		wheel_velocity[2] = y_speed + x_speed - (wheel_track_cali)*yaw_speed;
//		wheel_velocity[3] = -y_speed + x_speed + (wheel_track_cali)*yaw_speed;
		
		wheel_velocity[0] = - chassis_speed_target[1] + chassis_speed_target[0] - chassis_speed_target[2];
		wheel_velocity[1] =   chassis_speed_target[1] + chassis_speed_target[0] + chassis_speed_target[2];
		wheel_velocity[2] =   chassis_speed_target[1] + chassis_speed_target[0] - chassis_speed_target[2];
		wheel_velocity[3] = - chassis_speed_target[1] + chassis_speed_target[0] + chassis_speed_target[2];
		constrain_float(wheel_velocity[0], -1000, 1000);
		constrain_float(wheel_velocity[1], -1000, 1000);
		constrain_float(wheel_velocity[2], -1000, 1000);
		constrain_float(wheel_velocity[3], -1000, 1000);
		printf("wheel_velocity	x:%d y:%d z:%d \r\n", (int16_t)wheel_velocity[0], (int16_t)wheel_velocity[1], (int16_t)wheel_velocity[2]);
		Motor_A_SetSpeed((int16_t)wheel_velocity[0]);
		Motor_B_SetSpeed((int16_t)wheel_velocity[1]);
		Motor_C_SetSpeed((int16_t)wheel_velocity[2]);
		Motor_D_SetSpeed((int16_t)wheel_velocity[3]);
		/*  */
		
		vTaskDelayUntil(&currentTime, 20);
	}
}
