void distance(void);

void distance(){
	
	RCC->AHBENR				|=RCC_AHBENR_GPIOAEN; //Enable GPIOA for ultrasonic
//Set PA 4 as analog input
// BY DEFINITION PA 4 HAS ANALOG INPUT SIGNAL CONNECTED TO CHANNEL 4
// ADC_IN4
	GPIOA->MODER 		&= ~(0x03 << (2*4));
	GPIOA->MODER 		|= (0x03 << (2*4)); // 11 is analog 
	
// Set PA 5
	GPIOA->MODER 		&= ~(0x03 << (2*5));
	GPIOA->MODER		|= (0x03 << (2*5));
	
	//I/O at 2MHz low speed 01
	GPIOA->OSPEEDR		&= ~(0x03 << (2*4)); //bit clear
	GPIOA->OSPEEDR		|= (0x01 << (2*4)); // or 01 on PA4
	
	GPIOA->OSPEEDR		&= ~(0x03 << (2*5)); //bit clear
	GPIOA->OSPEEDR		|= (0x01 << (2*5)); // or 01 on PA5
	
	//output push pull state
	GPIOA->OTYPER 		&= ~(1<<4); //Reset state 0 on PA4
	GPIOA->OTYPER 		&= ~(1<<5); //Reset state 0 on PA5
	
	//Pull up Pull down Register
	GPIOA->PUPDR		&= ~(0x03 << (2*4)); //clears for no pupdr
	GPIOA->PUPDR		&= ~(0x03 << (2*5)); //clears for no pupdr

	
//------------------------------	
	
  RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY)==0);
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	ADC1->CR1 &= ~(ADC_CR1_RES);				//reset
	ADC1->CR1 |= ADC_CR1_EOCIE;					//end of conversion
	ADC1->CR1 |= (ADC_CR1_SCAN);				//scan
	
	ADC1->SQR1 &= ~ADC_SQR1_L; 					// clears 00000 for 1 conversion
	
	ADC1->SQR5 |= (4&ADC_SQR5_SQ1);			// 1st conversion in regular sequence
		
	ADC1->SMPR3 &= ~ADC_SMPR3_SMP4;		
	
	ADC1->SMPR3  |= ADC_SMPR3_SMP4; 		// 111: 384 cycles for channel 4							 

	ADC1->CR2 	 &= ~(ADC_CR2_DELS); 		//clear delay
	ADC1->CR2 	 |= (ADC_CR2_DELS_0);		//no delay

	ADC1->CR2 |= ADC_CR2_ADON;					//AD converter on 
	ADC1->CR2 	&= ~(ADC_CR2_EXTEN);		//trigger on rising edge bit clear
	ADC1->CR2   |= (0x03)<<28;						//trigger on rising edge

	ADC1->CR2 |= ADC_CR2_SWSTART;				//start of conversion
}


uint16_t sensor_side(uint16_t data){  //SIDE SENSOR
 uint16_t cm=0;
	uint8_t num[2], distance;
	uint8_t i;
	uint8_t count;

  cm =(4096 * 1000)/((155 * data) - 13 * 4096);  // good formula for side sensor (0A41SK)
	//cm -= 4;
	if (cm > 40)   // constrain the results to be between 4 and 40 
		cm = 40;
	if (cm < 4)
		cm =4;
	
	num[0] = cm/10;    // lcd purpose
	num[1] = cm%10;
	
	return cm;
}

uint16_t sensor_front(uint16_t data){   //FRONT SENSOR
 uint16_t cm=0;
	uint8_t num[2], distance;
	uint8_t i;
	uint8_t count;

	cm =(4096 * 1000)/((155 * data) - 13 * 4096);   // good formula for front sesnor (2Y0A21)
	//cm -= 4;
	if (cm > 30)
		cm = 30;
	if (cm < 4)
		cm =4;
	
	num[0] = cm/10;
	num[1] = cm%10;
	
	return cm;
	//for(i = 0; i<2; i++)
	//{
	//	distance = num[i] + 48;
	//	LCD_WriteChar(&distance,0,0,i+4,cm);
	//}	
}