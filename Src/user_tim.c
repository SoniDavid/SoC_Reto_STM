/*
 * user_tim.c
 *
 *  Created on: May 19, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "user_tim.h"

void USER_TIM3_PWM_Init( void ){
	/* STEP 0 Enable the clock signal for the TIM3 and GPIOB peripherals */
	RCC->IOPENR  |= (1U << 1);  // Enable GPIOB clock
	RCC->APBENR1 |= (1U << 1);  // Enable TIM3 clock

	/* STEP 0.1 Configure GPIOBs to use PWM
	 	 Note: ON THE REFERENCE MANUAL, PAGE 194 OR TABLE 8.5.12,
			IT CAN BE FOUND HOW TO CONFIGURE THE AFR*/

	// PB4 (CH1 - AF1)
	GPIOB->AFRL &= ~(0xFUL << 16U);
	GPIOB->AFRL |=  (0x1UL << 16U);
	GPIOB->PUPDR &= ~(0x3UL << 8U);
	GPIOB->OTYPER &= ~(1U << 4);
	GPIOB->MODER &= ~(0x3UL << 8U);
	GPIOB->MODER |=  (0x2UL << 8U);

	// PB5 (CH2 - AF1)
	GPIOB->AFRL &= ~( 0xFUL << 20U ); // Clear PB5 bits (23:20)
	GPIOB->AFRL |=  ( 0x1UL << 20U ); // Set PB5 to AF1
	GPIOB->PUPDR  &= ~( 0x3UL <<  10U );
	GPIOB->OTYPER	&= ~( 0x1UL <<  5U );
	GPIOB->MODER  &= ~( 0x1UL <<  10U );
	GPIOB->MODER  |=  ( 0x2UL <<  10U );

	// PB6 (CH3 - AF3)
	GPIOB->AFRL &= ~( 0xFUL << 24U ); // Clear PB6 bits (27:24)
	GPIOB->AFRL |=  ( 0x3UL << 24U); // Set PB6 to AF3
	GPIOB->PUPDR  &= ~( 0x3UL << 12U);
	GPIOB->OTYPER	&= ~( 0x1UL << 6U);
	GPIOB->MODER  &= ~( 0x1UL << 12U);
	GPIOB->MODER  |=  ( 0x2UL << 12U);

	// PB7 (CH4 - AF3)
	GPIOB->AFRL &= ~(0xFUL << 28U); // Clear PB7 bits (31:28)
	GPIOB->AFRL |=  (0x3UL << 28U); // Set PB7 to AF3
	GPIOB->PUPDR  &= ~(0x3UL << 14U);
	GPIOB->OTYPER	&= ~(0x1UL << 7U);
	GPIOB->MODER  &= ~(0x1UL << 14U);
	GPIOB->MODER  |=  (0x2UL << 14U);

	/* STEP 1. Configure the clock source (internal) */
	TIM3->SMCR	&= ~(0x1UL << 16U)
							&  ~(0x7UL <<  0U);//		Prescaler is clocked directed by the internal clock

	/* STEP 2. Configure the counter mode, the auto-reload and the overflow UEV-event  */
	TIM3->CR1	&= ~(0x3UL << 5U) //		Selects edge-aligned mode
						&  ~(0x1UL << 4U) //		Counter used as upcounter
						&  ~(0x1UL << 2U) //		UEV can be generated by software
						&  ~(0x1UL << 1U);//		Update Event (UEV) enabled
	TIM3->CR1	|=  (0x1UL << 7U);//		Auto-reload register is buffered

	/* STEP 3. Configure the prescaler, the period and the duty cycle register values */
	TIM3->PSC = 0U;
	TIM3->ARR = 47999U;//	for 1 KHz frequency
	uint16_t duty = USER_Duty_Cycle(0); // 0-100 range values
	TIM3->CCR1 = duty;
	TIM3->CCR2 = duty;
	TIM3->CCR3 = duty;
	TIM3->CCR4 = duty;

	/* STEP 4. Configure the PWM mode, the compare register load and channel direction */
	/* For CH1 and CH2 -> CCMR1 */
	TIM3->CCMR1 &= ~((0x3UL << 0U) | (0x7UL << 4U) | (0x1UL << 3U)); // Clear CC1S, OC1M, OC1PE
	TIM3->CCMR1 |=  ((0x0UL << 0U) | (0x6UL << 4U) | (0x1UL << 3U)); // CC1 as output, PWM1, preload
	TIM3->CCMR1 &= ~((0x3UL << 8U) | (0x7UL << 12U) | (0x1UL << 11U)); // Clear CC2S, OC2M, OC2PE
	TIM3->CCMR1 |=  ((0x0UL << 8U) | (0x6UL << 12U) | (0x1UL << 11U)); // CC2 as output, PWM1, preload

	/* For CH3 and CH4 -> CCMR2 */
	TIM3->CCMR2 &= ~((0x3UL << 0U) | (0x7UL << 4U) | (0x1UL << 3U)); // Clear CC3S, OC3M, OC3PE
	TIM3->CCMR2 |=  ((0x0UL << 0U) | (0x6UL << 4U) | (0x1UL << 3U)); // CC3 as output, PWM1, preload
	TIM3->CCMR2 &= ~((0x3UL << 8U) | (0x7UL << 12U) | (0x1UL << 11U)); // Clear CC4S, OC4M, OC4PE
	TIM3->CCMR2 |=  ((0x0UL << 8U) | (0x6UL << 12U) | (0x1UL << 11U)); // CC4 as output, PWM1, preload

	/* STEP 5. Generate the UEV-event to load the registers */
	TIM3->EGR |=  ( 0x1UL <<  0U );

	/* STEP 6. Enable the PWM signal output and set the polarity */
	// CH1: OC1 active high, output enable
	TIM3->CCER	&= ~( 0x1UL <<  3U ) //		for output mode, this bit must be cleared
				&  ~( 0x1UL <<  1U );//		OC1 active high
	TIM3->CCER	|=  ( 0x1UL <<  0U );//		OC1 signal is output on the corresponding pin

	// CH2: OC2 active high, output enable
	TIM3->CCER	&= ~( 0x1UL <<  7U ) //		for output mode, this bit must be cleared
				&  ~( 0x1UL <<  5U );//		OC2 active high
	TIM3->CCER	|=  ( 0x1UL <<  4U );//		OC2 signal is output on the corresponding pin

	// CH3: OC3 active high, output enable
	TIM3->CCER	&= ~( 0x1UL << 11U ) //		for output mode, this bit must be cleared
				&  ~( 0x1UL <<  9U );//		OC3 active high
	TIM3->CCER	|=  ( 0x1UL <<  8U );//		OC3 signal is output on the corresponding pin

	// CH4: OC4 active high, output enable
	TIM3->CCER	&= ~( 0x1UL << 15U ) //		for output mode, this bit must be cleared
				&  ~( 0x1UL << 13U );//		OC4 active high
	TIM3->CCER	|=  ( 0x1UL << 12U );//		OC4 signal is output on the corresponding pin

	/* STEP 7. Enable the Timer to start counting */
	TIM3->CR1	|=  ( 0x1UL <<  0U );
}

void USER_TIM14_Init(void){

	/*Enable clock signal for timer 14*/
	RCC->APBENR2 |= (1U << 15); // Enable TIM14 clock

	/*Enable internal clock source is not needed*/

	// Configure auto-reload preload, counter mode, and overflow
	TIM14->CR1 |= (1UL << 7U);    // ARPE = 1 (Enable preload)
	TIM14->CR1 |= (1UL << 3U);   // OPM = 1 (Continuous mode)
	TIM14->CR1 &= ~(1UL << 2U);   // URS = 0 (Allow any source to generate UEV)
	TIM14->CR1 &= ~(1UL << 1U);   // UDIS = 0 (Enable UEV)
}

void USER_TIM14_Delay(uint16_t prescaler, uint16_t AutoReload){

	// Configure Prescaler
	TIM14->PSC = prescaler;

	// Configure Auto Reload
	TIM14->ARR = AutoReload;

	//Clear TImer Update Interrupt Flag
	TIM14->SR &= ~(1UL << 0U);

	//Enable Counter
	TIM14->CR1 |= (1UL << 0U);

  // Wait until update event (UIF = 1)
  while (!(TIM14->SR & (1UL << 0U)));
  // Clear update flag again if needed
  TIM14->SR &= ~(1UL << 0U);
}


// function used internally for initialization
uint16_t USER_Duty_Cycle( uint8_t duty ){
	/* duty can be a value between 0% and 100% */
	if( duty <= 100 )
		return (( duty / 100.0 )*( TIM3->ARR + 1));
	else
		return 0;
}

// function to be used in main.c
void USER_Set_PWM_Duty(uint8_t duty_percent) {
	uint16_t duty = USER_Duty_Cycle(duty_percent);
	TIM3->CCR1 = duty;
	TIM3->CCR2 = duty;
	TIM3->CCR3 = duty;
	TIM3->CCR4 = duty;

	TIM3->EGR |= (1U << 0); // Force update event to load new CCR values
	TIM3->CR1 &= ~(0x1UL << 1U);
}

void USER_Change_PWM_Duty(int8_t delta_percent) {
	// Get current duty cycle from CCR1 and convert it to pe	rcent
	uint16_t current_duty_ticks = TIM3->CCR1;
	uint16_t arr_value = TIM3->ARR + 1; // ARR is zero-based
	float current_percent = ((float)current_duty_ticks / arr_value) * 100.0f;

	// Apply the delta and clamp
	float new_percent = current_percent + delta_percent;
	if (new_percent > 100.0f) new_percent = 100.0f;
	if (new_percent < 0.0f) new_percent = 0.0f;

	// Convert back to ticks and set new duty
	uint16_t new_duty = (uint16_t)((new_percent / 100.0f) * arr_value);
	TIM3->CCR1 = new_duty;
	TIM3->CCR2 = new_duty;
	TIM3->CCR3 = new_duty;
	TIM3->CCR4 = new_duty;
}




