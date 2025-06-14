/*
 * user_uart.h
 *
 *  Created on: Jun 10, 2025
 *      Author: abrah
 */

#ifndef INC_USER_UART_H_
#define INC_USER_UART_H_

void USER_USART1_Init(void);
void USER_UART2_Init( void );
void USER_USART1_Send_8bit(uint8_t Data);
void USER_USART1_Transmit(uint8_t *pData, uint16_t size);
uint8_t USER_USART1_Receive_8bit(void);
int _write(int file, uint8_t *ptr, int len);

#endif /* INC_USER_UART_H_ */
