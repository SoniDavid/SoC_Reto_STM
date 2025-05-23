/*
 * user_uart.c
 *
 *  Created on: May 19, 2025
 *      Author: sonid
 */

#include <stdint.h>
#include "main.h"
#include "user_uart.h"

//static void USER_USART1_Send_8bit(uint8_t Data);

void USER_UART1_Init(void){
	/* STEP 0. Enable the clock peripheral for the USART1 and the GPIOA*/
	RCC->IOPENR	= RCC->IOPENR   |  ( 0x1UL <<  0U );
	RCC->APBENR2  = RCC->APBENR2  |  ( 0x1UL << 14U );
	/* STEP 0. Configure the TX pin (PA9) as Alternate Function Push-Pul*/
	GPIOA->AFRH = GPIOA->AFRH & ~( 0xEUL <<  4U ); // Select the AF1 for the PA9
	GPIOA->AFRH = GPIOA->AFRH |  ( 0x1UL <<  4U ); // Select the AF1 for the PA9
	GPIOA->PUPDR  = GPIOA->PUPDR  & ~( 0x3UL << 18U ); // Clear pull-up/pull-down bits for PA9
	GPIOA->OTYPER = GPIOA->OTYPER & ~( 0x1UL <<  9U ); // Clear output type bit for PA9
	GPIOA->MODER  = GPIOA->MODER  & ~( 0x1UL << 18U ); // Set PA9 as AF
	GPIOA->MODER  = GPIOA->MODER  |  ( 0x2UL << 18U ); // Set PA9 as AF
	/* RX pin (PA10)*/
	GPIOA->AFRH = GPIOA->AFRH & ~( 0xEUL <<  8U );
	GPIOA->AFRH = GPIOA->AFRH |  ( 0x1UL <<  8U );
	GPIOA->PUPDR  = GPIOA->PUPDR  & ~( 0x3UL << 20U ); // Clear pull-up/pull-down bits for PA10
	GPIOA->OTYPER = GPIOA->OTYPER & ~( 0x1UL <<  10U ); // Clear output type bit for PA10
	GPIOA->MODER  = GPIOA->MODER  & ~( 0x1UL << 20U ); // Set PA10 as AF
	GPIOA->MODER  = GPIOA->MODER  |  ( 0x2UL << 20U ); // Set PA10 as AF
	/* STEP 1. Program the M bits in USART_CR1 to define the word length (8 bits)*/
	USART1->CR1   = USART1->CR1   & ~( 0x1UL << 28U );
	USART1->CR1   = USART1->CR1   & ~( 0x1UL << 12U );
	// Recibir
	USART1->CR1 = USART1->CR1 | (0x1UL << 0U);  // UE (USART Enable)
	USART1->CR1 = USART1->CR1 | (0x1UL << 3U);  // TE (Transmitter Enable)

	/* STEP 2. Select the desired baud rate using the USART_BRR register */
	USART1->BRR   = 0x68;   // 12 Mhz / 115200 = 0d104.16 = 0x68
	/* STEP 3. Program the number of STOP bits in USART_CR2 (1 stop bit) */
	USART1->CR2   = USART1->CR2   & ~( 0x3UL << 12U );
	/* STEP 4. Enable the USART by writting the UE bit in USART_CR1 register */
	USART1->CR1   = USART1->CR1   |  ( 0x1UL <<  0U );
	/* STEP 6. Set the TE bit in USART_CR1 to send and idle frame as first transmission */
	USART1->CR1   = USART1->CR1   |  ( 0x1UL <<  3U );
	USART1->CR1 = USART1->CR1 | (0x1UL << 2U);  // RE (Receiver Enable)
}

void USER_USART1_Send_8bit( uint8_t Data ){
	while(!( USART1->ISR & ( 0x1UL <<  7U)));//	wait until next data can be written
	USART1->TDR = Data;// Data to send
}

void USER_UART1_Transmit( uint8_t *pData, uint16_t size ){
	for( int i = 0; i < size; i++ ){
		USER_USART1_Send_8bit( *pData++ );
	}
}

uint8_t USER_UART1_Receive_8bit( void ){
	while(!( USART1->ISR & ( 1UL << 5U)));  // Espera hasta que RXNE esté en 1 (dato recibido)
	return (uint8_t)(USART1->RDR & 0xFF);  // Lee el dato recibido
}
