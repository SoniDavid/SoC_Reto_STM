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


void USER_GPIO_Init( void );

/* Superloop structure */
int main(void)
{
	/* Declarations and Initializations */
  USER_RCC_Init( );
  USER_TIM3_PWM_Init( );

    /* Repetitive block */
    for(;;){
    // include function for PA6 (push button debounce)
    // include function for controlling PWM from main file
    }
 }


void PA6_ButtonCheck( void ) {
  if (!(GPIOA->IDR & (1UL << 6))) {  // Button press detected (logic low)
    USER_Delay();

      if (!(GPIOA->IDR & (1UL << 6))) {  // Double-check press
	    // ======= DO SOME TASK HERE =======
	    /* probablemente actualizar una variable global que haga
	     * que el PWM baje
         */
		// USER_Set_PWM_Duty(50);
		// Wait for button release
		while (!(GPIOA->IDR & (1UL << 6)));
            USER_Delay();
        }
    }
}


