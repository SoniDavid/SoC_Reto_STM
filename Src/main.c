/**
 ******************************************************************************
 * @file           : main.c
 * @author         : D Soni
 * @briefs          : Main program body
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
#include "lcd.h"
#include "delays.h"
#include "user_core_cm0plus.h"
#include "user_adc.h"
#include "tests.h"

void Break_PWM( void );		// Use push button to break PWM

/* Superloop structure */
int main(void)
{
	/* Initialization of Peripherals */
	USER_RCC_Init(); 				// Set CLK to 48MHz
	USER_SysTick_Init();		//
  USER_GPIO_Init();				// Initialize push button (break)
  USER_TIM3_PWM_Init();		// Set TIM3 CH1-4 to PWM
  USER_TIM14_Init();			// Enable TIM14 for Delay
	USER_UART1_Init();			// Enable Full-Duplex UART communication
	LCD_Init();							// Initialize LCD
	USER_ADC_Init();


  //Set initial values
  USER_Set_PWM_Duty(50);	// this function doesn't work

  for(;;){
  	  USER_Set_PWM_Duty(20);
  	  delay_ms(1000);
  	  USER_Set_PWM_Duty(80);
  	  delay_ms(1000);
  }

}

void Break_PWM(void) {
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




