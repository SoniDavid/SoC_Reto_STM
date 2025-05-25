/*
 * user_uart.c
 *
 *  Created on: May 20, 2025
 *      Author: abrah
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "user_uart.h"
#include "user_tim.h"
#include "user_core_cm0plus.h"

void USER_USART1_Init(void) {
	/* STEP 0. Enable the clock peripheral for the USART1 */
	RCC->IOPENR = RCC->IOPENR | (0x1UL << 0U);
	RCC->APBENR2 = RCC->APBENR2 | (0x1UL << 14U);

	/* STEP 0. Configure the TX pin (PA9) as Alternate Function Push-Pull */
	GPIOA->AFRH = GPIOA->AFRH & ~(0xEUL << 4U);
	GPIOA->AFRH = GPIOA->AFRH | (0x1UL << 4U);
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 18U);
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 9U);
	GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 18U);
	GPIOA->MODER = GPIOA->MODER | (0x2UL << 18U);

	GPIOA->AFRH = GPIOA->AFRH & ~(0xEUL << 8U);
	GPIOA->AFRH = GPIOA->AFRH | (0x1UL << 8U);
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 20U);
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 10U);
	GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 20U);
	GPIOA->MODER = GPIOA->MODER | (0x2UL << 20U);

	/* STEP 1. Program the M bits in USART_CR1 to define the word length (8 bits) */
	USART1->CR1 = USART1->CR1 & ~(0x1UL << 28U);
	USART1->CR1 = USART1->CR1 & ~(0x1UL << 12U);

	/* STEP 2. Select the desired baud rate using the USART_BRR register */
	USART1->BRR = 417;
	/* STEP 3. Program the number of STOP bits in USART_CR2 (1 stop bit) */\
	USART1->CR2 = USART1->CR2 & ~(0x3UL << 12U);
	/* STEP 4. Enable TE (Transmitter Enable) and RE (Receiver Enable) */
	USART1->CR1 |= (1UL << 2U); // RE - Receiver enable
	USART1->CR1 |= (1UL << 3U); // TE - Transmitter enable
	/* STEP 5. Enable the USART */
	USART1->CR1 |= (1UL << 0U); // UE - USART enable

	USART1->CR1 |= (1UL << 5U);
	NVIC_ISER0 = (1UL << 27);
}
int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		while (!( USART1->ISR & (0x1UL << 7U)))
			;
		USART1->TDR = *ptr++;
	}
	return len;
}
void USER_USART1_Send_8bit(uint8_t Data) {
	while (!( USART1->ISR & (0x1UL << 7U)))
		; // wait until next data can be written
	USART1->TDR = Data; // Data to send
}
void USER_USART1_Transmit(uint8_t *pData, uint16_t size) {
	for (int i = 0; i < size; i++) {
		USER_USART1_Send_8bit(*pData++);
	}
}
uint8_t USER_USART1_Receive_8bit(void) {
	while (!( USART1->ISR & (1UL << 5U)))
		; // Espera hasta que RXNE esté en 1 (dato recibido)
	return (uint8_t) (USART1->RDR & 0xFF); // Lee el dato recibido
}
