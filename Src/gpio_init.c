/*
 * gpio_init.c
 *
 *  Created on: May 20, 2025
 *      Author: soni
 */
#include <stdint.h>
#include "main.h"

void USER_GPIO_Init( void ){

  RCC->IOPENR	  = RCC->IOPENR   |  ( 0x1UL <<  0U );

  // Configure PA6 as input w pull up
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x2UL << 12U);
  GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 12U);
  GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 12U);
}

/* Delay using Assembly */
void USER_Delay(void){
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

