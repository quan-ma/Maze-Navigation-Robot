//****************************************************

// includes system clock 

//****************************************************

#include "stm32l1xx.h"
#include "math.h"


volatile uint32_t msTicks;                      ///counts 1ms timeTicks    
uint32_t SystemCoreClock    = 2097000;
uint32_t val=0;

/*-------------------------------------------------------------------------
  SysTick_Handler
 //--------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*---------------------------------------------------------------------------
 // delays number of tick Systicks (happens every 1 ms)
 //----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}
