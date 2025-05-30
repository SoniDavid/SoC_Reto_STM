/*
 * rcc_init.c
 *
 *  Created on: May 19, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "rcc_init.h"
#include "main.h"

void USER_RCC_Init( void ){
	/* System Clock (SYSCLK) configuration for 48 MHz */
	FLASH->ACR	&= ~( 0x6UL <<  0U ) ;// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	FLASH->ACR	|=  ( 0x1UL <<  0U ); // 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	while(( FLASH->ACR & ( 0x7UL <<  0U )) != 0x001UL );// wait until LATENCY[2:0]=001
	RCC->CR		&= ~( 0x7UL << 11U ); // select HSISYS division factor by 1
	while(!( RCC->CR & ( 0x1UL << 10U ))); // wait until HSISYS is stable and ready
	RCC->CFGR	&= ~( 0x7UL <<  0U );  // select HSISYS as the SYSCLK clock source
	RCC->CFGR	&= ~( 0x1UL << 11U ); // select HCLK division factor by 1
}
