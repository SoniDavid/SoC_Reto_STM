/*
 * delays.c
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */

#include "delays.h"
#include <stdint.h>
void delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < ms * 4800; i++) {
    __asm__("nop");
  }
}
void delay_us(uint32_t us) {
  for (uint32_t i = 0; i < us * 4.8; i++) {
    __asm__("nop");
  }
}
