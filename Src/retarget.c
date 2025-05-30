/*
 * retarget.c
 *
 *  Created on: May 29, 2025
 *      Author: sonid
 */

#include "user_uart.h"  // for USER_USART1_Transmit
#include <stdio.h>

int __io_putchar(int ch) {
  uint8_t c = (uint8_t) ch;
  USER_USART1_Transmit(&c, 1);
  return ch;
}

