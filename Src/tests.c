/*
 * tests.c
 *
 *  Created on: May 24, 2025
 *      Author: sonid
 */

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

void Test_ADC_LED(void) {	// TESTED, WORKING
    // Assumes USER_ADC_Init and USER_GPIO_Init have been called before
    for (;;) {
        uint16_t adc_value = USER_ADC_Read();
        // LED ON if ADC value > 75% of 4095 (~3071)
        if (adc_value > 3071) {
            GPIOA->BSRR = (1UL << 5); // Set PA5(LED ON)
        } else {
            GPIOA->BSRR = (1UL << (5 + 16)); // Reset PA5(LED OFF)
        }

        SysTick_Delay(100); // Optional delay: check every 100ms
    }
}


//void Test_UART_COM(void){
///* insert code here @abraham" */
//
//}

void Test_LCD(void) {	// TESTED, NOT WORKING WTH PWM
	uint8_t col = 16;

	USER_RCC_Init();	// Ensure system clock is configured
	USER_GPIO_Init();	// Ensure GPIO (required by LCD or other init)
	LCD_Init();				// Initialize the LCD

	for (;;) {
		LCD_Clear();
		LCD_Set_Cursor(1, 1);
		LCD_Put_Str("TE");
		LCD_Put_Num(2003);
		LCD_Put_Char('B');
		LCD_Put_Str(" SoC");

		LCD_Set_Cursor(2, col--);
		LCD_Put_Str("Prueba de LCD ");
		LCD_BarGraphic(0, 64);

		delay_ms(200);

		if (col == 0) {
			delay_ms(500);
			col = 16;
		}
	}
}

void Test_PB_LED(void){	// TESTED AND WORKING
	// IMPORTANT TO CHECK THE TIME OF DELAY
	/* Declarations and Initializations */
	USER_GPIO_Init( );
    /* Repetitive block */
    for(;;){
      if (!(GPIOA->IDR & (0x1UL << 6U))){
//      	SysTick_Delay(500);
        delay_ms(20);
      	// add tim 14 timer for testing
        if (!(GPIOA->IDR & (0x1UL << 6U))){
          GPIOA->ODR = GPIOA->ODR ^ (0x1UL << 5U);
          while(!(GPIOA->IDR & (0x1UL << 6U))){}
//          SysTick_Delay(500);
          delay_ms(20);
          // add tim 14 timer for testing
        }
      }
    }

}

void Test_PWM_LED_BASIC(void){	//TESTED, NOT WORKING WITH LCD
  USER_Set_PWM_Duty(20);
  delay_ms(1000);
  USER_Set_PWM_Duty(80);
  delay_ms(1000);
}

void Control_PWM_With_ADC(void) {	//TESTED WORKING
    // Assumes USER_ADC_Init, USER_GPIO_Init, and USER_TIM3_PWM_Init have been called before
    while (1) {
        uint16_t adc_value = USER_ADC_Read();  // Read ADC (0 to 4095)

        // Map ADC value to duty cycle (0% to 100%)
        uint8_t duty = (adc_value * 100) / 4095;

        // Set PWM duty cycle
        USER_Set_PWM_Duty(duty);

        // Optional: short delay to reduce update rate
        SysTick_Delay(100);
    }
}

void Update_PWM_From_Velocity(float input_vl) {
    // Clamp the input to [0.0, 200.0] for safety
    if (input_vl < 0.0f) input_vl = 0.0f;
    if (input_vl > 200.0f) input_vl = 200.0f;

    // Map velocity to 0–100% PWM duty
    uint8_t duty = (uint8_t)((input_vl / 200.0f) * 100.0f);

    USER_Set_PWM_Duty(duty);
}


//void Test_PWM_LED(void){
//
//}

