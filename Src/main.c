/**
 ******************************************************************************
 * @file           : main.c
 * @author         : D Soni
 * @brief          : Main program body
 ******************************************************************************
 */

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include <stdint.h>
#include "main.h"
#include "user_tim.h"
#include "rcc_init.h"
#include "gpio_init.h"
#include "user_uart.h"

void Break_PWM( void );		// Use push button to break PWM

/* Superloop structure */
int main(void)
{
	/* Initialization of Peripherals */
	USER_RCC_Init( ); 			// Set CLK to 48MHz
	USER_UART1_Init( );			// Enable Full-Duplex UART communication
  USER_TIM3_PWM_Init( );	// Set TIM3 CH1-4 to PWM
  USER_GPIO_Init( );			// Initialize push button (break)

  //Set initial values
  USER_Set_PWM_Duty(0);		// set initial PWM value to be 0

  for(;;){

  }
}

void Break_PWM( void ) {
  if (!(GPIOA->IDR & (1UL << 6))) {  // Button press detected (logic low)
    USER_Delay();
      if (!(GPIOA->IDR & (1UL << 6))) {  // Double-check press
//      	void USER_Set_PWM_Duty();
		// Wait for button release
		while (!(GPIOA->IDR & (1UL << 6)));
            USER_Delay();
        }
    }
}


