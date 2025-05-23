/*
 * user_uart.h
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */

#ifndef USER_UART_H_
#define USER_UART_H_

#include <stdio.h>
int _write(int file, char *ptr, int len);

void USER_UART1_Init(void);
void USER_UART1_Transmit(uint8_t *pData, uint16_t size);
uint8_t USER_UART1_Receive_8bit(void);
void USER_USART1_Send_8bit(uint8_t);


/* USART registers */
typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t BRR;
	volatile uint32_t GTPR;
	volatile uint32_t RTOR;
	volatile uint32_t RQR;
	volatile uint32_t ISR;
	volatile uint32_t ICR;
	volatile uint32_t RDR;
	volatile uint32_t TDR;
	volatile uint32_t PRESC;
} USART_TypeDef;

#define USART1_BASE	0x40013800//		USART 1 base address
#define USART1	(( USART_TypeDef *)USART1_BASE )

#endif /* USER_UART_H_ */
