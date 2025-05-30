/*
 * user_adc.c
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "main.h"
#include "user_core_cm0plus.h"
#include "user_adc.h"
#include "user_tim.h"

//void USER_ADC_Init(void){
//	/* STEP 0. Enable the clock peripheral for the ADC and GPIOx */
//	//RCC->IOPENR	|= ( 0x1UL <<  0U );
//	RCC->APBENR2 |= (0x1UL << 20U);
//
//	/* STEP 0. Configure ADC_INx PIN with analog function to input the voltage */
//	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 0U);
//	GPIOA->MODER = GPIOA->MODER |  (0x3UL << 0U);
//
//	/* STEP 1. Select the ADC clock from the SYSCLK or the PCLK  */
//	ADC->CFGR2 &= ~(0x3UL << 30U);
//
//	/* STEP 2. Select the ADC Clock Prescaler according to the maximum frequency allowed by the peripheral */
//	ADC->CCR &= ~(0xEUL << 18U);
//	ADC->CCR |=  (0x1UL << 18U);
//
//	/* STEP 3. Select the ADC resolution, conversion mode, and data alignment */
//	ADC->CFGR1	&= ~(0x1UL << 13U) //	Single conversion
//							&  ~(0x1UL <<  5U) //	Right alignment
//							&  ~(0x3UL << 3U);//	12-bit resolution
//
//	/* STEP 4. Select the sample time for the ADC channel */
//	ADC->SMPR	&= ~(0x7UL << 0U);
//
//	/* STEP 5. Select the channels, sequence and/or number of conversions for the ADC channels */
//	ADC->ISR 	  &= ~(0x1UL << 13U);//				It must be cleared to perform a new channel configuration
//	ADC->CFGR1	&= ~(0x1UL << 21U) //				Mode of CHSELR register
//							&  ~(0x1UL << 2U);//				Forward scan
//	ADC->CHSELR	|=  (0x1UL << 0U);//				IN0 selected for conversion
//	while(!(ADC->ISR & (0x1UL << 13U)));// wait until channel configuration is applied
//
//	/* STEP 6. Enable the internal voltaje regulator of the ADC */
//	ADC->CR |= (0x1UL << 28U);
//	SysTick_Delay( 2 );		//wait 20us to ADC voltage regulator start-up time
////	USER_TIM14_Delay(0, 959);
//
//	/* STEP 7. It is recommended to perform a calibration after each power-up, before the ADC is enabled */
//	while(!USER_ADC_Calibration());
//
//	/* STEP 8. Enable the ADC module */
//	ADC->CR |= (0x1UL << 0U);
//	while(!(ADC->ISR & (0x1UL << 0U)));// wait until ADC is ready to start conversion
//}

void USER_ADC_Init(void) {
  // Enable ADC peripheral clock
  RCC->APBENR2 |= (0x1UL << 20U);

  // Configure PA0 as analog input (ADC_IN0)
  GPIOA->PUPDR &= ~(0x3UL << 0U);
  GPIOA->MODER |= (0x3UL << 0U);

  // Select ADC clock source (default: SYSCLK)
  ADC->CFGR2 &= ~(0x3UL << 30U);

  // Set ADC clock prescaler
  ADC->CCR &= ~(0xEUL << 18U);
  ADC->CCR |= (0x1UL << 18U); // ÷2

  // Ensure ADC is disabled before proceeding
  if (ADC->CR & (1U << 0)) {
    ADC->CR |= (1U << 1); // ADDIS
    while (ADC->CR & (1U << 0)); // Wait until ADEN == 0
  }

  // Enable internal voltage regulator for ADC
  ADC->CR |= (1U << 28); // ADVREGEN
  SysTick_Delay(2); // Wait at least 20µs for regulator startup

  // Perform calibration
  ADC->CR &= ~(1U << 0); // Ensure ADEN == 0
  ADC->CR |= (1U << 31); // Start calibration (ADCAL)
  while (ADC->CR & (1U << 31)); // Wait until calibration completes

  // Configure input channel
  ADC->ISR &= ~(1U << 13); // Clear CCRDY
  ADC->CFGR1 &= ~((1U << 21) | (1U << 2)); // CHSELRMOD = 0, SCANDIR = 0
  ADC->CHSELR = (1U << 0); // Select channel IN0
  while (!(ADC->ISR & (1U << 13))); // Wait for CCRDY

  // Configure sampling time
  ADC->SMPR &= ~(0x7UL << 0U); // Minimal sample time

  // Enable ADC
  ADC->ISR |= (1U << 0); // Clear ADRDY
  ADC->CR |= (1U << 0);  // Set ADEN
  while (!(ADC->ISR & (1U << 0))); // Wait until ADC is ready (ADRDY)
}

uint16_t USER_ADC_Read(void) {
  ADC->CR |= (0x1UL << 2U);  // Start conversion (ADSTART)
  while (!(ADC->ISR & (0x1UL << 2U)));  // Wait until conversion complete (EOC)
  return (uint16_t) (ADC->DR);  // Return 12-bit result
}
