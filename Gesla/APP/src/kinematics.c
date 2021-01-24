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
*
*   fb:y      lr:x
*/
#include "kinematics.h"
#include <stdio.h>


#define PI 						3.1415926

#define PID_RATE    			500
#define WHEEL_TRACK 			0.2194
#define WHEEL_DIAMETER          0.1292
#define ENCODER_RESOLUTION      1440.0

#define CHASSIS_LINEAR_SPEED_LIMIT	5000   //m/s*1000
#define CHASSIS_ANGULAR_SPEED_LIMIT	5000  //rad/s*1000

/**
  *according to the number of degrees the encoder has rotated that calculates how many meters the robot has moved forward
  *Replace motor encoder with xyz
  *input:  encoder accumulator
  *output: xyz speed
  */
void Kinematics_Analysis(int16_t* input, int16_t* output)
{

}

/**
  *According to Jetson NANO's speed target(m/s) that calculates how much speed we need to set(motor rotate speed)
  *Replace NANO chassis xyz speed with wheel speed
  *input:  speedx\y\z   m/s*1000(NANO ctl)
  *output: motor speed target
  */
double ticks_per_meter = 0;
void Inverse_Kinematics_Analysis(int16_t* input, int16_t* output)
{
	float x_speed = ((float)input[0])/7;
	float y_speed = ((float)input[1])/7;
	float z_speed = ((float)input[2])/7;
	static float wheel_velocity[4] = {0};
	
	ticks_per_meter = (float)ENCODER_RESOLUTION/(WHEEL_DIAMETER*PI);
	
	//Chassis Omni Move Calculate
	wheel_velocity[0] = -y_speed - x_speed + (WHEEL_TRACK * z_speed);
	wheel_velocity[1] = -y_speed + x_speed - (WHEEL_TRACK * z_speed);
	wheel_velocity[2] =  y_speed + x_speed - (WHEEL_TRACK * z_speed);
	wheel_velocity[3] =  y_speed - x_speed + (WHEEL_TRACK * z_speed);

	output[0] = (int16_t)(wheel_velocity[0] * ticks_per_meter/PID_RATE);
	output[1] = (int16_t)(wheel_velocity[1] * ticks_per_meter/PID_RATE);
	output[2] = (int16_t)(wheel_velocity[2] * ticks_per_meter/PID_RATE);
	output[3] = (int16_t)(wheel_velocity[3] * ticks_per_meter/PID_RATE);
	
//	printf("output target 0: %d \r\n", output[0]);
//	printf("output target 1: %d \r\n", output[1]);
//	printf("output target 2: %d \r\n", output[2]);
//	printf("output target 3: %d \r\n", output[3]);
}
