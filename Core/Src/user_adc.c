/*
 * user_adc.c
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "main.h"
#include "user_adc.h"
#include "user_tim.h"

void USER_ADC_Init(void) {
  // Enable ADC peripheral clock
  RCC->APBENR2 |= (0x1UL << 20U);

  // Configure PA0 as analog input (ADC_IN0)
  GPIOA->PUPDR &= ~(0x3UL << 0U);
  GPIOA->MODER |= (0x3UL << 0U);

  // Select ADC clock source (default: SYSCLK)
  ADC1->CFGR2 &= ~(0x3UL << 30U);

  // Set ADC clock prescaler
  ADC1->CR &= ~(0xEUL << 18U);
  ADC1->CR |= (0x1UL << 18U); // ÷2

  // Ensure ADC is disabled before proceeding
  if (ADC1->CR & (1U << 0)) {
    ADC1->CR |= (1U << 1); // ADDIS
    while (ADC1->CR & (1U << 0)); // Wait until ADEN == 0
  }

  // Enable internal voltage regulator for ADC
  ADC1->CR |= (1U << 28); // ADVREGEN
  //SysTick_Delay(2); // Wait at least 20µs for regulator startup
  USER_TIM14_Delay(47, 20);  // Prescaler 47 → 1 µs por tick, ARR = 20 ticks = 20 µs

  // Perform calibration
  ADC1->CR &= ~(1U << 0); // Ensure ADEN == 0
  ADC1->CR |= (1U << 31); // Start calibration (ADCAL)
  while (ADC1->CR & (1U << 31)); // Wait until calibration completes

  // Configure input channel
  ADC1->ISR &= ~(1U << 13); // Clear CCRDY
  ADC1->CFGR1 &= ~((1U << 21) | (1U << 2)); // CHSELRMOD = 0, SCANDIR = 0
  ADC1->CHSELR = (1U << 0); // Select channel IN0
  while (!(ADC1->ISR & (1U << 13))); // Wait for CCRDY

  // Configure sampling time
  ADC1->SMPR &= ~(0x7UL << 0U); // Minimal sample time

  // Enable ADC
  ADC1->ISR |= (1U << 0); // Clear ADRDY
  ADC1->CR |= (1U << 0);  // Set ADEN
  while (!(ADC1->ISR & (1U << 0))); // Wait until ADC is ready (ADRDY)
}

uint16_t USER_ADC_Read(void) {
  ADC1->CR |= (0x1UL << 2U);  // Start conversion (ADSTART)
  while (!(ADC1->ISR & (0x1UL << 2U)));  // Wait until conversion complete (EOC)
  return (uint16_t) (ADC1->DR);  // Return 12-bit result
}
