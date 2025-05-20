/*
 * gpio_init.c
 *
 *  Created on: May 20, 2025
 *      Author: soni
 */
#include <stdint.h>
#include "main.h"

void USER_GPIO_Init( void ){
  /* Enable GPIOA clock (creo que ya lo inicializo con UART),
   * see if uart should be initalize also here
   */
  RCC->IOPENR	  = RCC->IOPENR   |  ( 0x1UL <<  0U );

  // Configure PA6 as input w pull up
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x2UL << 12U);
  GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 12U);
  GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 12U);
}
