#include "stm32l1xx.h"
#include "math.h"
//#include "LCDmainHeader.h"   ////**************Uncomment to use the LCD *****************LCD
#include "distanceSensors.h"
#include "motor_header.h"
#include "system_clock.h"    


int main(void)
{  uint16_t result2,result, result1,counter=0;  //variables needed for conversions
	 uint16_t front_sensor =0,side_sensor=0;      // seperating front and side sensor
	 uint16_t i=0,j=0;
	 uint16_t motor_delay=5;   // motor delay defines the speed of the robot 
	
  Motor motor_right; //RIGHT MOTOR
	Motor motor_left; //LEFT MOTOR

//	LCD_Clock_Init();    //*****************Uncomment to use the LCD**********************LCD
	if (SysTick_Config(SystemCoreClock / 1000)) { // SysTick 1 msec interrupts  
   while (1);                                  // Capture error              
 }

  //LCD_PIN_Init();    //****************Uncoment To use the LCD**************************LCD
  //LCD_Configure();   //****************UNComment To use the LCD*************************LCD
  distance();
 	Motor_Init();  // initilozing the PB AND PC PINS FOR MOTOR
	motor_right.step = 0;        // right motor
	motor_right.totalSteps = 0;  //right motor
	
	motor_left.step = 0;        // left motor
	motor_left.totalSteps = 0;  //left motor
 
 while (1)
	 {
		 ADC1->CR2 |= ADC_CR2_SWSTART;  //ADC Conversion
		 
		if(!(ADC1->SR & ADC_SR_EOC))
		{
			result1 = ADC1->DR;
			//LCD_Display_2(result1);   //***************uncomment to view LCD********************LCD
		  front_sensor=	sensor_front(result1);
		}
		//	ADC1->CR2 |= ADC_CR2_SWSTART;
	 
		if((ADC1->SR & ADC_SR_EOC))
		{	
			result2 = ADC1->DR;
			//LCD_Display(result2);    //***************uncomment to view LCD*********************LCD
		  side_sensor= sensor_side(result2);
		}

	// below are the conditions in which the robot will face moving throught the maze
		if( side_sensor >= 10  && side_sensor <= 20 )   // if is between 10 and 20cm away then keep moving foward
		{	 
				 for(i=0; i<4; i++)
				 {
				 StepMotor_half_right(&motor_right); // half step stepper motor right
				 Delay (motor_delay);
				 StepMotor_half_left(&motor_left); // half step stepper motor left
				 Delay (motor_delay); 
				 } 
		}	
		else if ( side_sensor < 10)   // if it gets less than 10cm then shift a bit to the left
		{
		  	for(i=0; i<3; i++)
				 {
				 StepMotor_half_right(&motor_right); // half step stepper motor right
				 Delay (motor_delay);
				 StepMotor_half_left_reverse(&motor_left); // half step stepper motor left
				 Delay (motor_delay); 
				 } 		 
		}
	else if ( side_sensor > 20 && side_sensor < 30)   // if is greater than 20 cm and less than 30cm adjust yourself to move right
		{
		  	for(i=0; i<30; i++)
				 {
				 StepMotor_half_right_reverse(&motor_right); // half step stepper motor right reverse
				 Delay (motor_delay);
				 StepMotor_half_left(&motor_left); // half step stepper motor left
				 Delay (motor_delay); 
				 } 				    				 
		}
	else if ( side_sensor >= 30)   // if it become greater than 30 then perform the turn right sequence 
		{
		  	for(i=0; i< 400; i++)
				 {   
				 StepMotor_half_right(&motor_right); // half step stepper motor right
				 Delay (motor_delay);
				 StepMotor_half_left(&motor_left); // half step stepper motor left
				 Delay (motor_delay); 
				 } 
				 for(i=0; i<150; i++)
				 {
				 StepMotor_half_right_reverse(&motor_right); // half step stepper motor right reverse
				 Delay (motor_delay);
				 StepMotor_half_left(&motor_left); // half step stepper motor left
				 Delay (motor_delay); 
				 } 		 
		 }
	 }// end of while loop
} //end main

