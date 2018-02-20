#define bool _Bool

void LCD_Clock_Init(void);
void LCD_PIN_Init(void);
void LCD_Configure(void);
void reflective_sensors(void);

//void LCD_Display_String2(void); //white
		
/*  =========================================================================
                                 LCD MAPPING
    =========================================================================
	           A
     _  ----------
COL |_| |\   |J  /|
       F| H  |  K |B
     _  |  \ | /  |
COL |_| --G-- --M--
        |   /| \  |
       E|  Q |  N |C
     _  | /  |P  \|   
DP  |_| -----------  
	           D         
*/

 
/* Constant table for cap characters 'A' --> 'Z' */
const uint16_t CapLetterMap[26] = {
        /* A      B      C      D      E      F      G      H      I  */
        0xFE00,0x6714,0x1d00,0x4714,0x9d00,0x9c00,0x3f00,0xfa00,0x0014,
        /* J      K      L      M      N      O      P      Q      R  */
        0x5300,0x9841,0x1900,0x5a48,0x5a09,0x5f00,0xFC00,0x5F01,0xFC01,
        /* S      T      U      V      W      X      Y      Z  */
        0xAF00,0x0414,0x5b00,0x18c0,0x5a81,0x00c9,0x0058,0x05c0
};
 
/* Constant table for number '0' --> '9' */

const uint16_t NumberMap[10] = {
        /* 0      1      2      3      4      5      6      7      8      9  */
        0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
};



volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */
uint32_t SystemCoreClock    = 2097000;

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
*----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
 while ((msTicks - curTicks) < dlyTicks);
}


// Converts an ascii char to the a LCD digit
static void LCD_Conv_Char_Seg(uint8_t* c, bool point, bool column, uint8_t* digit) {
  uint16_t ch = 0 ;
  uint8_t i,j;
 
  switch (*c) {
    case ' ' : 
      ch = 0x00;
      break;
 
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':			
      ch = NumberMap[*c-0x30];		
      break;
 
    default:
      /* The character c is one letter in upper case*/
      if ( (*c < 0x5b) && (*c > 0x40) ) {
        ch = CapLetterMap[*c - 'A'];
      }
      /* The character c is one letter in lower case*/
      if ( (*c <0x7b) && ( *c> 0x60) ) {
        ch = CapLetterMap[*c - 'a'];
      }
      break;
  }
 
  /* Set the digital point can be displayed if the point is on */
  if (point) {
    ch |= 0x0002;
  }
 
  /* Set the "COL" segment in the character that can be displayed if the column is on */
  if (column) {
    ch |= 0x0020;
  }		
 
  for (i = 12,j=0; j<4; i-=4,j++) {
    digit[j] = (ch >> i) & 0x0f; //To isolate the less signifiant dibit
  }
}
 
// This function is to display a given ASCII character on a specified position
// Input:
// ch: the ASCII value of the character to be display
// colon: The colon boolean flag indicates whether the colon is display.
// position: The LCD can display six decimal digits, thus the position is between 1 and 6.
void LCD_WriteChar(uint8_t* ch, bool point, bool colon, uint8_t position,uint8_t cm) {
  uint8_t digit[4];     /* Digit frame buffer */
 
  // Convert displayed character in segment in array digit 
  LCD_Conv_Char_Seg(ch, point, colon, digit);
 
	
  while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
    
    LCD->RAM[4] &= 0x2000; // clear bar 3
		LCD->RAM[6] &= 0x2000; // clear bar 2
		LCD->RAM[4] &= 0x8000; // clear bar 1
		LCD->RAM[6] &= 0x8000; // bar 0
	if (cm < 30)
	{
		LCD->RAM[6] |= 0x8000; // bar 0
	}
  if (cm <21)
	{
		LCD->RAM[4] |= 0x8000; // bar 1
	}
	if (cm < 13)
	{
		LCD->RAM[6] |= 0x2000; // bar 2
	}
	if (cm < 5)
	{
		LCD->RAM[4] |= 0x2000; // bar 3
	}
	
	
  switch (position) {
    /* Position 1 on LCD (Digit 1)*/
    case 1:
      LCD->RAM[0] &= 0xcffffffc;
      LCD->RAM[2] &= 0xcffffffc;
      LCD->RAM[4] &= 0xcffffffc;
      LCD->RAM[6] &= 0xcffffffc;
 
      LCD->RAM[0] |= ((digit[0]& 0x0c) << 26 ) | (digit[0]& 0x03) ; // 1G 1B 1M 1E	    
      LCD->RAM[2] |= ((digit[1]& 0x0c) << 26 ) | (digit[1]& 0x03) ; // 1F 1A 1C 1D 
      LCD->RAM[4] |= ((digit[2]& 0x0c) << 26 ) | (digit[2]& 0x03) ; // 1Q 1K 1Col 1P                                                                                                                                    
      LCD->RAM[6] |= ((digit[3]& 0x0c) << 26 ) | (digit[3]& 0x03) ; // 1H 1J 1DP 1N
 
      break;
 
    /* Position 2 on LCD (Digit 2)*/
    case 2:
      LCD->RAM[0] &= 0xf3ffff03;
      LCD->RAM[2] &= 0xf3ffff03;      
      LCD->RAM[4] &= 0xf3ffff03;
      LCD->RAM[6] &= 0xf3ffff03;
 
      LCD->RAM[0] |= ((digit[0]& 0x0c) << 24 )|((digit[0]& 0x02) << 6 )|((digit[0]& 0x01) << 2 ) ; // 2G 2B 2M 2E	  
      LCD->RAM[2] |= ((digit[1]& 0x0c) << 24 )|((digit[1]& 0x02) << 6 )|((digit[1]& 0x01) << 2 ) ; // 2F 2A 2C 2D
      LCD->RAM[4] |= ((digit[2]& 0x0c) << 24 )|((digit[2]& 0x02) << 6 )|((digit[2]& 0x01) << 2 ) ; // 2Q 2K 2Col 2P
      LCD->RAM[6] |= ((digit[3]& 0x0c) << 24 )|((digit[3]& 0x02) << 6 )|((digit[3]& 0x01) << 2 ) ; // 2H 2J 2DP 2N
 
      break;
 
    /* Position 3 on LCD (Digit 3)*/
    case 3:
      LCD->RAM[0] &= 0xfcfffcff;
      LCD->RAM[2] &= 0xfcfffcff;
      LCD->RAM[4] &= 0xfcfffcff;
      LCD->RAM[6] &= 0xfcfffcff;
 
      LCD->RAM[0] |= ((digit[0]& 0x0c) << 22 ) | ((digit[0]& 0x03) << 8 ) ; // 3G 3B 3M 3E	
      LCD->RAM[2] |= ((digit[1]& 0x0c) << 22 ) | ((digit[1]& 0x03) << 8 ) ; // 3F 3A 3C 3D
      LCD->RAM[4] |= ((digit[2]& 0x0c) << 22 ) | ((digit[2]& 0x03) << 8 ) ; // 3Q 3K 3Col 3P
      LCD->RAM[6] |= ((digit[3]& 0x0c) << 22 ) | ((digit[3]& 0x03) << 8 ) ; // 3H 3J 3DP 3N
 
      break;
 
    /* Position 4 on LCD (Digit 4)*/
    case 4:
      LCD->RAM[0] &= 0xffcff3ff;
      LCD->RAM[2] &= 0xffcff3ff;
      LCD->RAM[4] &= 0xffcff3ff;
      LCD->RAM[6] &= 0xffcff3ff;
 
      LCD->RAM[0] |= ((digit[0]& 0x0c) << 18 ) | ((digit[0]& 0x03) << 10 ) ; // 4G 4B 4M 4E	
      LCD->RAM[2] |= ((digit[1]& 0x0c) << 18 ) | ((digit[1]& 0x03) << 10 ) ; // 4F 4A 4C 4D
      LCD->RAM[4] |= ((digit[2]& 0x0c) << 18 ) | ((digit[2]& 0x03) << 10 ) ; // 4Q 4K 4Col 4P
      LCD->RAM[6] |= ((digit[3]& 0x0c) << 18 ) | ((digit[3]& 0x03) << 10 ) ; // 4H 4J 4DP 4N
 
      break;
 
    /* Position 5 on LCD (Digit 5)*/
    case 5:
      LCD->RAM[0] &= 0xfff3cfff;
      LCD->RAM[2] &= 0xfff3cfff;
      LCD->RAM[4] &= 0xfff3efff;
      LCD->RAM[6] &= 0xfff3efff;
 
      LCD->RAM[0] |= ((digit[0]& 0x0c) << 16 ) | ((digit[0]& 0x03) << 12 ) ; // 5G 5B 5M 5E	
      LCD->RAM[2] |= ((digit[1]& 0x0c) << 16 ) | ((digit[1]& 0x03) << 12 ) ; // 5F 5A 5C 5D
      LCD->RAM[4] |= ((digit[2]& 0x0c) << 16 ) | ((digit[2]& 0x01) << 12 ) ; // 5Q 5K   5P 
      LCD->RAM[6] |= ((digit[3]& 0x0c) << 16 ) | ((digit[3]& 0x01) << 12 ) ; // 5H 5J   5N
 
      break;
 
    /* Position 6 on LCD (Digit 6)*/
    case 6:
      LCD->RAM[0] &= 0xfffc3fff;
      LCD->RAM[2] &= 0xfffc3fff;
      LCD->RAM[4] &= 0xfffc3fff;
      LCD->RAM[6] &= 0xfffc3fff;
 
      LCD->RAM[0] |= ((digit[0]& 0x04) << 15 ) | ((digit[0]& 0x08) << 13 ) | ((digit[0]& 0x03) << 14 ) ; // 6B 6G 6M 6E	
      LCD->RAM[2] |= ((digit[1]& 0x04) << 15 ) | ((digit[1]& 0x08) << 13 ) | ((digit[1]& 0x03) << 14 ) ; // 6A 6F 6C 6D
      LCD->RAM[4] |= ((digit[2]& 0x04) << 15 ) | ((digit[2]& 0x08) << 13 ) | ((digit[2]& 0x01) << 14 ) ; // 6K 6Q    6P 
      LCD->RAM[6] |= ((digit[3]& 0x04) << 15 ) | ((digit[3]& 0x08) << 13 ) | ((digit[3]& 0x01) << 14 ) ; // 6J 6H   6N
 
      break;
 
     default:
      break;
  }
 
  LCD->SR |= LCD_SR_UDR; 
  while ((LCD->SR & LCD_SR_UDD) == 0);
}

void LCD_PIN_Init(void){	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
	
	GPIOA->MODER &= ~(0x0C03F00FC);
	GPIOA->MODER |= 0x802A00A8;
	
	GPIOA->AFR[0] &= ~(0x0FFF0);
	GPIOA->AFR[0] |=	0x0BBB0;
	GPIOA->AFR[1] &= ~(0x0F0000FFF);
	GPIOA->AFR[1] |=	0x0B0000BBB;
	
	GPIOB->MODER &= ~(0x0FFFF0FC0);
	GPIOB->MODER |= 0x0AAAA0A80;
	
	GPIOB->AFR[0] &= ~(0x0FFF000);
	GPIOB->AFR[0] |=	0x0BBB000;
	GPIOB->AFR[1] &= ~(0x0FFFFFFFF);
	GPIOB->AFR[1] |=	0x0BBBBBBBB;
	
	GPIOC->MODER &= ~(0x0FFF0FF);
	GPIOC->MODER |= 0x0AAA0AA;
	GPIOC->AFR[0] &= ~(0x0FF00FFFF);
	GPIOC->AFR[0] |=	0x0BB00BBBB;
	GPIOC->AFR[1] &= ~(0x0FFFF);
	GPIOC->AFR[1] |=	0x0BBBB;
}

void LCD_Configure(void) {
	LCD->CR &= ~LCD_CR_BIAS;
	LCD->CR |= (2UL<<5);
	LCD->CR &= ~LCD_CR_DUTY;
	LCD->CR |= (3UL<<2);
	LCD->FCR &= ~LCD_FCR_CC;
	LCD->FCR |= (4UL<<10);
	LCD->FCR &= ~LCD_FCR_PON;
	LCD->FCR |=LCD_FCR_PON_2;
	LCD->CR  |= LCD_CR_MUX_SEG;
	LCD->CR &= ~LCD_CR_VSEL;
	
	//LCD->FCR &= LCD_FCR_PS;
	//LCD->FCR |= 0x2<<22;
	
	//LCD->FCR &= ~LCD_FCR_BLINK;
	//LCD->FCR |= 0x3<<16;
	//LCD->FCR &= ~LCD_FCR_BLINKF;
	//LCD->FCR |= 0x7<<13;
	
	while (!(LCD->SR & LCD_SR_FCRSR)) ;
	LCD->CR |= LCD_CR_LCDEN;
	while (!(LCD->CR & LCD_CR_LCDEN));
	while (!(LCD->SR & LCD_SR_RDY)) ;
	
}




void LCD_Clock_Init(void){
	// Note from STM32L Reference Manual: 	
	// After reset, the RTC Registers (RTC registers and RTC backup registers) are protected
	// against possible stray write accesses. To enable access to the RTC Registers, proceed as
	// follows:
	// 1. Enable the power interface clock by setting the PWREN bits in the RCC_APB1ENR register.
	// 2. Set the DBP bit in the PWR_CR register (see Section 4.4.1).
	// 3. Select the RTC clock source through RTCSEL[1:0] bits in RCC_CSR register.
	// 4. Enable the RTC clock by programming the RTCEN bit in the RCC_CSR register.
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// Power interface clock enable
	PWR->CR      |= PWR_CR_DBP;					// Disable Backup Domain write protection
	RCC->CSR	 	 |= RCC_CSR_RTCSEL_LSI;	// LSI oscillator clock used as RTC clock
	//LSI clock varies due to frequency dispersion
	//RCC->CSR		 |= RCC_CSR_RTCSEL_LSE;	// LSE oscillator clock used as RTC clock
	RCC->CSR		 |= RCC_CSR_RTCEN;			// RTC clock enable
	
	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCA;					// RTC write protection register (WPR)
	RTC->WPR = 0x53;					// Write "0xCA" and "0x53" to unlock the write protection
	
	// Wait until MSI clock ready
	while((RCC->CR & RCC_CR_MSIRDY) == 0); // MSI Ready Flag is set by hardware
	
	/* Enable comparator clock LCD */
	RCC->APB1ENR |= RCC_APB1ENR_LCDEN;
	
	/* Enable SYSCFG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	RCC->CSR |= RCC_CSR_LSION;
	while( (RCC->CSR & RCC_CSR_LSIRDY) == 0 );
	
	/* Select LSI as LCD Clock Source */
	RCC->CSR &= ~RCC_CSR_RTCSEL_LSI;  
	RCC->CSR |= RCC_CSR_RTCSEL_LSI;    // LSI oscillator clock used as RTC and LCD clock 	
	RCC->CSR |= RCC_CSR_RTCEN;	
}
void myLCD6(int color){  //postion  6
	    while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
	switch(color){
	  case 1: //B
			LCD->RAM[0] &= 0xfffc3fff;
      LCD->RAM[2] &= 0xfffc3fff;
      LCD->RAM[4] &= 0xfffc35ff;
      LCD->RAM[6] &= 0xfffc35ff;
			
      LCD->RAM[0] |= (0x00038000) ; // 6B 6G 6M    
      LCD->RAM[2] |= (0x0002C000) ; // 6A 6C 6D 
      LCD->RAM[4] |= (0x00004000) ; // 6P                                                                                                                                  
      LCD->RAM[6] |= (0x00020000) ; // 6J
	
		   break;
		
		case 2:  //W
	    LCD->RAM[0] &= 0xfffc3fff;
      LCD->RAM[2] &= 0xfffc3fff;
      LCD->RAM[4] &= 0xfffc35ff;
      LCD->RAM[6] &= 0xfffc35ff;
			
      LCD->RAM[0] |= (0x00024000) ; // 6B 6E	    
      LCD->RAM[2] |= (0x00018000) ; // 6F 6C
      LCD->RAM[4] |= (0x00010000) ; // 6Q                                                                                                                                 
      LCD->RAM[6] |= (0x00004000) ; // 6N
	//Delay(500);
		break;
		default: 		
			break;
	}
	 LCD->SR |= LCD_SR_UDR; 
  while ((LCD->SR & LCD_SR_UDD) == 0);
}	
void myLCD5(int color){  //position 5 
	    while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
	switch(color){
	  case 1: //B
      LCD->RAM[0] &= 0xfff3cfff;
      LCD->RAM[2] &= 0xfff3cfff;
      LCD->RAM[4] &= 0xfff3e5ff;
      LCD->RAM[6] &= 0xfff3e5ff;
			
      LCD->RAM[0] |= (0x000C2000) ; // 5B 5G 5M    
      LCD->RAM[2] |= (0x00043000) ; // 5A 5C 5D 
      LCD->RAM[4] |= (0x00001000) ; // 5P                                                                                                                                  
      LCD->RAM[6] |= (0x00040000) ; // 5J
	
		   break;
		
		case 2:  //W
      LCD->RAM[0] &= 0xfff3cfff;
      LCD->RAM[2] &= 0xfff3cfff;
      LCD->RAM[4] &= 0xfff3e5ff;
      LCD->RAM[6] &= 0xfff3e5ff;
			
      LCD->RAM[0] |= (0x00041000) ; // 5B 6E	    
      LCD->RAM[2] |= (0x00082000) ; // 5F 5C
      LCD->RAM[4] |= (0x00080000) ; // 5Q                                                                                                                                 
      LCD->RAM[6] |= (0x00001000) ; // 5N
	//Delay(500);
		break;
		default: 		
			break;
	}
	 LCD->SR |= LCD_SR_UDR; 
  while ((LCD->SR & LCD_SR_UDD) == 0);
}

void myLCD4(int color){  //position 4
	  while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
	switch(color){
	  case 1: //B
      LCD->RAM[0] &= 0xffcff3ff;
      LCD->RAM[2] &= 0xffcff3ff;
      LCD->RAM[4] &= 0xffcff5ff;
      LCD->RAM[6] &= 0xffcff5ff;
			
      LCD->RAM[0] |= (0x00300800) ; // 4G 4B 4M    
      LCD->RAM[2] |= (0x00100C00) ; // 4A 4C 4D 
      LCD->RAM[4] |= (0x00000400) ; // 4P                                                                                                                                  
      LCD->RAM[6] |= (0x00100000) ; // 4J
	
		   break;
		
		case 2:  //W
      LCD->RAM[0] &= 0xffcff3ff;
      LCD->RAM[2] &= 0xffcff3ff;
      LCD->RAM[4] &= 0xffcff5ff;
      LCD->RAM[6] &= 0xffcff5ff;
			
      LCD->RAM[0] |= (0x00100400) ; // 4B 4E	    
      LCD->RAM[2] |= (0x00200800) ; // 4F 4C
      LCD->RAM[4] |= (0x00200000) ; // 4Q                                                                                                                                 
      LCD->RAM[6] |= (0x00000400) ; // 4N
	//Delay(500);
		break;
		default: 		
			break;
	}
	 LCD->SR |= LCD_SR_UDR; 
  while ((LCD->SR & LCD_SR_UDD) == 0);
}
void myLCD3(int color){  //position 3
	 while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit
	switch(color){
	  case 1:			//B
      LCD->RAM[0] &= 0xfcfffcff;
      LCD->RAM[2] &= 0xfcfffcff;
      LCD->RAM[4] &= 0xfcfff5ff;
      LCD->RAM[6] &= 0xfcfff5ff;
			
      LCD->RAM[0] |= (0x03000200) ; // 3G 3B 3M    
      LCD->RAM[2] |= (0x01000300) ; // 3A 3C 3D 
      LCD->RAM[4] |= (0x00000100) ; // 3P                                                                                                                                  
      LCD->RAM[6] |= (0x01000000) ; // 3J
	
		   break;
		
		case 2:  //W
      LCD->RAM[0] &= 0xfcfffcff;
      LCD->RAM[2] &= 0xfcfffcff;
      LCD->RAM[4] &= 0xfcfff5ff;
      LCD->RAM[6] &= 0xfcfff5ff;
			
      LCD->RAM[0] |= (0x01000100) ; // 3B 3E	    
      LCD->RAM[2] |= (0x02000200) ; // 3F 3C
      LCD->RAM[4] |= (0x02000000) ; // 3Q                                                                                                                                 
      LCD->RAM[6] |= (0x00000100) ; // 3N
		//Delay(500);
		break;
		default: 		
			break;
	}
	 LCD->SR |= LCD_SR_UDR; 
  while ((LCD->SR & LCD_SR_UDD) == 0);
}





void LCD_Display(uint16_t data){  //SIDE SENSOR
 uint16_t cm=0;
	uint8_t num[2], distance;
	uint8_t i;
	uint8_t count;

  cm =(4096 * 1000)/((300 * data) - 13 * 4096);  // good formula for side sensor (0A41SK)
	//cm -= 4;
	if (cm > 30)
		cm = 30;
	if (cm < 4)
		cm =4;
	
	num[0] = cm/10;
	num[1] = cm%10;
	
	for(i = 0; i<2; i++)
	{
		distance = num[i] + 48;
		LCD_WriteChar(&distance,0,0,i+1,cm);
	}	
}

void LCD_Display_2(uint16_t data){   //FRONT SENSOR
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
	
	for(i = 0; i<2; i++)
	{
		distance = num[i] + 48;
		LCD_WriteChar(&distance,0,0,i+4,cm);
	}	
}
