/*
 * user_adc.c
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "main.h"
//#include "user_core_cm0plus.h"
#include "user_adc.h"
#include "user_tim.h"


void USER_ADC_Init(void){
	/* STEP 0. Enable the clock peripheral for the ADC and GPIOx */
	//RCC->IOPENR	|= ( 0x1UL <<  0U );
	RCC->APBENR2 |= (0x1UL << 20U);

	/* STEP 0. Configure ADC_INx PIN with analog function to input the voltage */
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 0U);
	GPIOA->MODER = GPIOA->MODER |  (0x3UL << 0U);

	/* STEP 1. Select the ADC clock from the SYSCLK or the PCLK  */
	ADC->CFGR2 &= ~(0x3UL << 30U);

	/* STEP 2. Select the ADC Clock Prescaler according to the maximum frequency allowed by the peripheral */
	ADC->CCR &= ~(0xEUL << 18U);
	ADC->CCR |=  (0x1UL << 18U);

	/* STEP 3. Select the ADC resolution, conversion mode, and data alignment */
	ADC->CFGR1	&= ~(0x1UL << 13U) //	Single conversion
							&  ~(0x1UL <<  5U) //	Right alignment
							&  ~(0x3UL << 3U);//	12-bit resolution

	/* STEP 4. Select the sample time for the ADC channel */
	ADC->SMPR	&= ~(0x7UL << 0U);

	/* STEP 5. Select the channels, sequence and/or number of conversions for the ADC channels */
	ADC->ISR 	  &= ~(0x1UL << 13U);//				It must be cleared to perform a new channel configuration
	ADC->CFGR1	&= ~(0x1UL << 21U) //				Mode of CHSELR register
							&  ~(0x1UL << 2U);//				Forward scan
	ADC->CHSELR	|=  (0x1UL << 0U);//				IN0 selected for conversion
	while(!( ADC->ISR & (0x1UL << 13U)));// wait until channel configuration is applied

	/* STEP 6. Enable the internal voltaje regulator of the ADC */
	ADC->CR |= (0x1UL << 28U);
//	SysTick_Delay( 1 );		//wait 20us to ADC voltage regulator start-up time
	USER_TIM14_Delay(0, 959);

	/* STEP 7. It is recommended to perform a calibration after each power-up, before the ADC is enabled */
	while(!USER_ADC_Calibration());

	/* STEP 8. Enable the ADC module */
	ADC->CR |= (0x1UL << 0U);
	while(!(ADC->ISR & (0x1UL << 0U)));// wait until ADC is ready to start conversion
}

static uint8_t USER_ADC_Calibration(void){
	ADC->CR	|=  ( 0x1UL << 31U );	//Start calibration
	if( !( ADC->CR & ( 0x1UL << 31U )) ){ //if calibration is done
		ADC->CALFACT = ADC->CALFACT + 1;		//Resulting calibration factor must be incremented by 1
		if( ADC->CALFACT > 0x7F )						//Check to avoid overflow
			ADC->CALFACT = 0x7F;
		return 1;
	}
	else
		return 0;
}

uint16_t USER_ADC_Read(void){
    ADC->CR |= (0x1UL << 2U);  // Start conversion (ADSTART)
    while (!(ADC->ISR & (0x1UL << 2U)));  // Wait until conversion complete (EOC)
    return (uint16_t)(ADC->DR);  // Return 12-bit result
}
