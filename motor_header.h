// *****************************HEADER FILE FOR MOTOR
// contains constants and Functions for Full step and half step for both motors
// HALF and Full step Right motor 
// HALF and Full step LEFT motor 
//*********************************************************************
//----------------------------------------------------------------
// Define Constants
#include "stm32l1xx.h"
#include "math.h"
//#include "system_clock.h"


#define F 1
#define R 2
#define O 0

//----------------------------------------------------------------

typedef struct _Motor
{
	unsigned long totalSteps;
	unsigned short step;
} Motor;
//----------------------------------------------------------------
// Prototypes
void coil_m1 (uint8_t dir, uint8_t p1, uint8_t p2);

//void StepMotor_1(Motor *); //half step DEFAULT
//void StepMotor_2(Motor *);  //full step DEFAULT

void StepMotor_half_left(Motor *);  //HALF Step with LEFT   motor PB15,14,13,12
void StepMotor_full_left(Motor *);  //FULL Step with LEFT   motor PB15,14,13,12
void StepMotor_full_left_reverse (Motor *);

void StepMotor_half_right(Motor *); //HALF Step with RIGHT motor PC6,7,8,9
void StepMotor_full_right(Motor *); //FULL Step with RIGHT motor PC6,7,8,9
void StepMotor_full_right_reverse (Motor *);
/*----------------------------------------------------------------------------
  initialize LED Pins
 *----------------------------------------------------------------------------*/
void Motor_Init (void) {

	
	//this initializes the buttons and gpio pins outputs to pb4 to pb11 , buttons inputs pb12 and pb13
//  RCC->AHBENR |=  (1UL <<  1);                  /* Enable GPIOA,GPIOB,GPIOC, clock         */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;  
	
  GPIOB->MODER   &=  ~(0xFF000000); // PB15-12 for Stepper Motor LEFT     //GOOD
 	GPIOB->MODER   |=   (0x55000000); // PB15-12 for Stepper Motor LEFT    //GOOD
	
	GPIOB->OTYPER  &=  ~(0xFF000000);   //clear
  GPIOB->OSPEEDR &=  ~(0xFF000000);   //clear
	
	GPIOC->MODER   &=  ~(0x000FF000); // PC 6-9 for Stepper Motor RIGHT    // GOOD 
 	GPIOC->MODER   |=   (0x00055000); // PC6-9 for Stepper Motor RIGHT   // GOOD
	
	GPIOC->OTYPER  &=  ~(0x000FF000);  //clear
  GPIOC->OSPEEDR &=  ~(0x000FF000);  //clear
	
	
	//GPIOB->MODER   &=  ~(0x0FFFFF00);  //SETTING PB 4,5,6,7,
 // GPIOB->MODER   |=   (0x05555500);   /* General purpose output mode*/
	//GPIOB->PUPDR   &=  ~(0x0FFFFF00);   /* No pull-up, pull-down      */
 // GPIOB->PUPDR   |=  (0x0A000000);    /* No pull-up, pull-down      */
	
}
/*----------------------------------------------------------------------------
  Function that outputs high for requested pin
 *----------------------------------------------------------------------------*/
void Pin_High (uint8_t num)
{
	if( num == 15 || num == 14 || num == 13 || num == 12 )
	GPIOB->BSRRL = (1<<num);
	else
	GPIOC->BSRRL = (1<<num);
} 
/*----------------------------------------------------------------------------
  Function that outputs low for requested pin
 *----------------------------------------------------------------------------*/
void Pin_Low (uint8_t num)
{
	if( num == 15 || num == 14 || num == 13 || num == 12 )
	GPIOB->BSRRH = (1<<num);
	else
	GPIOC->BSRRH = (1<<num);
} 

 
/*int main (void)
{   uint32_t i=2;
	  uint32_t j=2;
	  uint32_t count=0;
	  uint32_t count2=0;

	Motor motor_right; //RIGHT MOTOR
	Motor motor_left; //LEFT MOTOR
	
	
  if (SysTick_Config(SystemCoreClock / 1000)) { // SysTick 1 msec interrupts  
    while (1);                                  // Capture error              
  }
	
	Motor_Init();  // initilozing the PB AND PC PINS FOR MOTOR
	motor_right.step = 0;        // right motor
	motor_right.totalSteps = 0;  //right motor
	
	motor_left.step = 0;        // left motor
	motor_left.totalSteps = 0;  //left motor

	while(1){
		
		//MOTOR TESTING HALF/FULL LEFT /RIGHT
		 StepMotor_half_right(&motor_right); // half step stepper motor left
		//Delay (3);
   
		//StepMotor_full_right(&motor_right); // half step stepper motor left
		//Delay (2);
		
		 StepMotor_half_left(&motor_left); // half step stepper motor left
		 Delay (2);
		
	  //StepMotor_full_left(&motor_left); // half step stepper motor left
		//Delay (2);
		
		
		
		
			
		  //button for the pb13  and motor 
	   	if( GPIOB->IDR & (1<<13))
			{  while(1)
				{ StepMotor_half_right(&motor_right);
				 Delay(i);
				if( GPIOB->IDR & (1<<12)){  i++;
					  if (i>30)
					  {  
					      i==2;
					  }
				break;}
				}
		  } 
	 

	} //end while one loop
} //end_main */


// Motor States GENERIC WorksWith all PINS
void coil_m1 (uint8_t dir,uint8_t p1, uint8_t p2){
	

	if(dir == F) {
		Pin_High(p1);
		Pin_Low(p2);
	};
	if(dir == R){
		Pin_Low(p1);
		Pin_High(p2);
	}
	if(dir == O) {
		Pin_Low(p1);
		Pin_Low(p2);
	}
	return;
}

//-------------------MOTOR CASES FULL/HALF STEPS --------------------------

// half step eight cases only wokrs with pins 
void StepMotor_half_left (Motor *motor1){  //PB 15,14,13,12 WORKING HALF LEFT
													//4 // 5 // 6 //7

	switch (motor1->step){//15 //14 //13 //12
		case 0:             
			coil_m1(F,15,14); 
			coil_m1(O,13,12);
			break;
		case 1:
			coil_m1(F,15,14);
			coil_m1(F,13,12);
			break;
		case 2:
			coil_m1(O,15,14);
			coil_m1(F,13,12);
			break;
		case 3:
			coil_m1(R,15,14);
			coil_m1(F,13,12);
			break;
		case 4:
			coil_m1(R,15,14);
			coil_m1(O,13,12);
			break;
		case 5:
			coil_m1(R,15,14);
			coil_m1(R,13,12);
			break;
		case 6:
			coil_m1(O,15,14);
			coil_m1(R,13,13);
			break;
		case 7:
			coil_m1(F,15,14);
			coil_m1(R,13,12);
			break;
	}

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 8){
		motor1->step = 0;
	}
	return;

} 
void StepMotor_half_left_reverse (Motor *motor1){  //PB 15,14,13,12 WORKING HALF LEFT
													//4 // 5 // 6 //7

	switch (motor1->step){//15 //14 //13 //12
		case 7:             
			coil_m1(F,15,14); 
			coil_m1(O,13,12);
			break;
		case 6:
			coil_m1(F,15,14);
			coil_m1(F,13,12);
			break;
		case 5:
			coil_m1(O,15,14);
			coil_m1(F,13,12);
			break;
		case 4:
			coil_m1(R,15,14);
			coil_m1(F,13,12);
			break;
		case 3:
			coil_m1(R,15,14);
			coil_m1(O,13,12);
			break;
		case 2:
			coil_m1(R,15,14);
			coil_m1(R,13,12);
			break;
		case 1:
			coil_m1(O,15,14);
			coil_m1(R,13,13);
			break;
		case 0:
			coil_m1(F,15,14);
			coil_m1(R,13,12);
			break;
	}

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 8){
		motor1->step = 0;
	}
	return;

} 



void StepMotor_full_left (Motor *motor1){  //PB 15,14,13,12 WORKING FULL LEFT							
	
	switch (motor1->step)		
		{
		case 0:
			coil_m1(F,15,14);
			coil_m1(O,13,12);
			break;
		case 1:
			coil_m1(O,15,14);
			coil_m1(F,13,12);
			break;
		case 2:
			coil_m1(R,15,14);
			coil_m1(O,13,12);
			break;
		case 3:
			coil_m1(O,15,14);
			coil_m1(R,13,12);
			break;
	 }

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 4){
		motor1->step = 0;
	}
	return;
}

void StepMotor_full_left_reverse (Motor *motor1){  //PB 15,14,13,12 WORKING FULL LEFT							
	
	switch (motor1->step)		
		{
		case 3:
			coil_m1(F,15,14);
			coil_m1(O,13,12);
			break;
		case 2:
			coil_m1(O,15,14);
			coil_m1(F,13,12);
			break;
		case 1:
			coil_m1(R,15,14);
			coil_m1(O,13,12);
			break;
		case 0:
			coil_m1(O,15,14);
			coil_m1(R,13,12);
			break;
	 }

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 4){
		motor1->step = 0;
	}
	return;
}












void StepMotor_half_right (Motor *motor1){ // PC 6,7,8,9 WORKING HALF RIGHT 
												//4 // 5 // 6 //7
												//6 // 7 // 8 // 9
	switch (motor1->step){//15 //14 //13 //12
												
		case 7:             
			coil_m1(F,6,7); 
			coil_m1(O,8,9);
			break;
		case 6:
			coil_m1(F,6,7);
			coil_m1(F,8,9);
			break;
		case 5:
			coil_m1(O,6,7);
			coil_m1(F,8,9);
			break;
		case 4:
			coil_m1(R,6,7);
			coil_m1(F,8,9);
			break;
		case 3:
			coil_m1(R,6,7);
			coil_m1(O,8,9);
			break;
		case 2:
			coil_m1(R,6,7);
			coil_m1(R,8,9);
			break;
		case 1:
			coil_m1(O,6,7);
			coil_m1(R,8,9);
			break;
		case 0:
			coil_m1(F,6,7);
			coil_m1(R,8,9);
			break;
	}

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 8){
		motor1->step = 0;
	}
	return;

} 

void StepMotor_half_right_reverse (Motor *motor1){ // PC 6,7,8,9 WORKING HALF RIGHT 
												//4 // 5 // 6 //7
												//6 // 7 // 8 // 9
	switch (motor1->step){//15 //14 //13 //12
												
		case 0:             
			coil_m1(F,6,7); 
			coil_m1(O,8,9);
			break;
		case 1:
			coil_m1(F,6,7);
			coil_m1(F,8,9);
			break;
		case 2:
			coil_m1(O,6,7);
			coil_m1(F,8,9);
			break;
		case 3:
			coil_m1(R,6,7);
			coil_m1(F,8,9);
			break;
		case 4:
			coil_m1(R,6,7);
			coil_m1(O,8,9);
			break;
		case 5:
			coil_m1(R,6,7);
			coil_m1(R,8,9);
			break;
		case 6:
			coil_m1(O,6,7);
			coil_m1(R,8,9);
			break;
		case 7:
			coil_m1(F,6,7);
			coil_m1(R,8,9);
			break;
	}

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 8){
		motor1->step = 0;
	}
	return;

} 




void StepMotor_full_right (Motor *motor1){	// PC 6,7,8,9 WORKING FULL RIGHT											
	
	switch (motor1->step)		
		{
		case 3:
			coil_m1(F,6,7);
			coil_m1(O,8,9);
			break;
		case 2:
			coil_m1(O,6,7);
			coil_m1(F,8,9);
			break;
		case 1:
			coil_m1(R,6,7);
			coil_m1(O,8,9);
			break;
		case 0:
			coil_m1(O,6,7);
			coil_m1(R,8,9);
			break;
	 }

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 4){
		motor1->step = 0;
	}
	return;
}

void StepMotor_full_right_reverse (Motor *motor1){	// PC 6,7,8,9 WORKING FULL RIGHT											
	
	switch (motor1->step)		
		{
		case 0:
			coil_m1(F,6,7);
			coil_m1(O,8,9);
			break;
		case 1:
			coil_m1(O,6,7);
			coil_m1(F,8,9);
			break;
		case 2:
			coil_m1(R,6,7);
			coil_m1(O,8,9);
			break;
		case 3:
			coil_m1(O,6,7);
			coil_m1(R,8,9);
			break;
	 }

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 4){
		motor1->step = 0;
	}
	return;
}















// HALF step for PB 4,5,6,7




/*void StepMotor_1 (Motor *motor1){  // PB 4,5,6,7   DEFAULT 
																    
	switch (motor1->step){
		case 0:
			coil_m1(F,4,5);
			coil_m1(O,6,7);
			break;
		case 1:
			coil_m1(F,4,5);
			coil_m1(F,6,7);
			break;
		case 2:
			coil_m1(O,4,5);
			coil_m1(F,6,7);
			break;
		case 3:
			coil_m1(R,4,5);
			coil_m1(F,6,7);
			break;
		case 4:
			coil_m1(R,4,5);
			coil_m1(O,6,7);
			break;
		case 5:
			coil_m1(R,4,5);
			coil_m1(R,6,7);
			break;
		case 6:
			coil_m1(O,4,5);
			coil_m1(R,6,7);
			break;
		case 7:
			coil_m1(F,4,5);
			coil_m1(R,6,7);
			break;
	}

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 8){
		motor1->step = 0;
	}
	return;

} */


//full step four cases only works with pins 4,5,6,7

/*void StepMotor_2 (Motor *motor1)   // PB 4,5,6,7   DEFAULT 
{
	switch (motor1->step)
		{
		case 0:
			coil_m1(F,4,5);
			coil_m1(O,6,7);
			break;
		case 1:
			coil_m1(O,4,5);
			coil_m1(F,6,7);
			break;
		case 2:
			coil_m1(R,4,5);
			coil_m1(O,6,7);
			break;
		case 3:
			coil_m1(O,4,5);
			coil_m1(R,6,7);
			break;
	 }

	motor1->totalSteps++;
	motor1->step++;
	if (motor1->step == 4){
		motor1->step = 0;
	}
	return;
}*/
