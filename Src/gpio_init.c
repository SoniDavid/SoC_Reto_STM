/*
 * gpio_init.c
 *
 *  Created on: May 20, 2025
 *      Author: soni
 */
#include <stdint.h>
#include "main.h"

void USER_GPIO_Init(void) {
  // Enable GPIOA CLK
  RCC->IOPENR = RCC->IOPENR | (0x1UL << 0U);

  // Configure PA6 as input w pull up
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x2UL << 12U);
  GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 12U);
  GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 12U);

  // Configure PA5 as output push pull (It comes in the LCD, so JIC)
  GPIOA->BSRR = 0x1UL << 21U; // Reset PA5 low to turn off LED
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 10U); // Clear pull-up/pull-down bits for PA5
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 5U); // Clear output type bit for PA5
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 10U); // Set PA5 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 10U); // Set PA5 as output
}

/* Delay using Assembly */
void USER_Delay(void) {
  __asm(" ldr r0, =12999");
  __asm(" again: sub r0, r0, #1");
  __asm(" cmp r0, #0");
  __asm(" bne again");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
  __asm(" nop");
}

/* Delay using TIM14 */

