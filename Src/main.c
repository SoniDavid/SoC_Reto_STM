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
// #include "user_keypad.h"
#include "user_tim.h"
#include "rcc_init.h"
#include "gpio_init.h"
#include "user_uart.h"

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
