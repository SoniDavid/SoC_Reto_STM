/*
 * lcd.c
 *
 *  Created on: May 23, 2025
 *      Author: sonid
 */
/* lcd.c - Updated to use available PA pins for D4-D7, avoiding PA13/14 and other used pins */

#include <stdint.h>
#include "main.h"
#include "lcd.h"

const int8_t UserFont[8][8] = {
	{ 0x11, 0x0A, 0x04, 0x1B, 0x11, 0x11, 0x11, 0x0E },
	{ 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
	{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
	{ 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C },
	{ 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E },
	{ 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

// Define safe pins for D4-D7 (avoid PA13 and PA14)
#define D4_PIN 1  // PA1
#define D5_PIN 2  // PA2
#define D6_PIN 3  // PA3
#define D7_PIN 4  // PA4

void LCD_Init(void) {
	int8_t const *p;
	RCC->IOPENR |= (1U << 0) | (1U << 1); // Enable GPIOA and GPIOB clocks

	// Configure control pins PB9–PB11 as output push-pull
	for (int pin = 9; pin <= 11; ++pin) {
		GPIOB->MODER &= ~(0x3U << (2 * pin));
		GPIOB->MODER |=  (0x1U << (2 * pin));
		GPIOB->OTYPER &= ~(1U << pin);
		GPIOB->PUPDR  &= ~(0x3U << (2 * pin));
	}

	// Configure data pins PA1–PA4 as output push-pull
	for (int pin = D4_PIN; pin <= D7_PIN; ++pin) {
		GPIOA->MODER &= ~(0x3U << (2 * pin));
		GPIOA->MODER |=  (0x1U << (2 * pin));
		GPIOA->OTYPER &= ~(1U << pin);
		GPIOA->PUPDR  &= ~(0x3U << (2 * pin));
	}

	GPIOB->BSRR = LCD_RS_PIN_LOW | LCD_RW_PIN_LOW | LCD_EN_PIN_LOW;
	GPIOA->BSRR = (1U << (D4_PIN + 16)) | (1U << (D5_PIN + 16)) | (1U << (D6_PIN + 16)) | (1U << (D7_PIN + 16));
	delay_ms(50);

	GPIOA->BSRR = (1U << D4_PIN) | (1U << D5_PIN);
	GPIOA->BSRR = (1U << (D6_PIN + 16)) | (1U << (D7_PIN + 16));
	LCD_Pulse_EN();
	delay_ms(5);
	LCD_Pulse_EN();
	delay_us(60);
	LCD_Pulse_EN();
	while (LCD_Busy());

	GPIOA->BSRR = (1U << (D4_PIN + 16)) | (1U << (D6_PIN + 16)) | (1U << (D7_PIN + 16));
	GPIOA->BSRR = (1U << D5_PIN);
	LCD_Pulse_EN();
	while (LCD_Busy());

	LCD_Write_Cmd(0x28);
	LCD_Write_Cmd(0x08);
	LCD_Write_Cmd(0x01);
	LCD_Write_Cmd(0x06);
	LCD_Write_Cmd(0x0F);
	LCD_Write_Cmd(0x40);

	p = &UserFont[0][0];
	for (int i = 0; i < sizeof(UserFont); i++, p++) {
		LCD_Put_Char(*p);
	}
	LCD_Write_Cmd(0x80);
}

void LCD_Out_Data4(uint8_t val) {
	(val & 0x01U) ? (GPIOA->BSRR = (1U << D4_PIN)) : (GPIOA->BSRR = (1U << (D4_PIN + 16)));
	(val & 0x02U) ? (GPIOA->BSRR = (1U << D5_PIN)) : (GPIOA->BSRR = (1U << (D5_PIN + 16)));
	(val & 0x04U) ? (GPIOA->BSRR = (1U << D6_PIN)) : (GPIOA->BSRR = (1U << (D6_PIN + 16)));
	(val & 0x08U) ? (GPIOA->BSRR = (1U << D7_PIN)) : (GPIOA->BSRR = (1U << (D7_PIN + 16)));
}

// Remaining functions (LCD_Write_Byte, LCD_Write_Cmd, LCD_Put_Char, etc.) stay as-is.
//Funcion que escribe 1 byte de datos en el LCD
void LCD_Write_Byte(uint8_t val){
	LCD_Out_Data4( ( val >> 4 ) & 0x0FU );
	LCD_Pulse_EN( );
	LCD_Out_Data4( val & 0x0FU );
	LCD_Pulse_EN( );
	while( LCD_Busy( ) );
}
//Funcion que escribe un comando en el LCD
void LCD_Write_Cmd(uint8_t val){
	GPIOB->BSRR	=	LCD_RS_PIN_LOW;
	LCD_Write_Byte( val );
}
//Escribe un caracter ASCII en el LCD
void LCD_Put_Char(uint8_t c){
	GPIOB->BSRR	=	LCD_RS_PIN_HIGH;
	LCD_Write_Byte( c );
}
//Funcion que establece el cursor en una posicion de la pantalla del LCD
//Minimum values for line and column must be 1
void LCD_Set_Cursor(uint8_t line, uint8_t column){
	uint8_t address;
	column--;
	line--;
	address = ( line * 0x40U ) + column;
	address = 0x80U + ( address & 0x7FU );
	LCD_Write_Cmd( address );
}
//Funcion que envia una cadena de caracteres ASCII al LCD
void LCD_Put_Str(char * str){
	for( int16_t i = 0; i < 16 && str[ i ] != 0; i++ )
		LCD_Put_Char( str[ i ] );
}
//Funcion que envia un caracter numerico al LCD
//El número debe ser entero y de 5 dígitos máximo
void LCD_Put_Num(int16_t num){
	int16_t p;
	int16_t f = 0;
	int8_t ch[ 5 ];
	for( int16_t i = 0; i < 5; i++ ){
		p = 1;
		for( int16_t j = 4 - i; j > 0; j-- )
			p = p * 10;
		ch[ i ] = ( num / p );
		if( num >= p && !f )
			f = 1;
		num = num - ch[ i ] * p;
		ch[ i ] = ch[ i ] + 48;
		if( f )
			LCD_Put_Char( ch[ i ] );
	}
}
//Funcion que provoca tiempos de espera en el LCD
char LCD_Busy(void){
/**
 * Configuracion de D7 as input floating
 */
	GPIOB->PUPDR &= ~( 0x3UL << 30U );
	GPIOB->MODER &= ~( 0x3UL << 30U );
	GPIOB->BSRR	  =	 LCD_RS_PIN_LOW;
	GPIOB->BSRR	  =	 LCD_RW_PIN_HIGH;
	GPIOB->BSRR	  =	 LCD_EN_PIN_HIGH;
	delay_us(100); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! wait for 100us
	if(( GPIOB->IDR	& LCD_D7_PIN_HIGH )) {
		GPIOB->BSRR	=  LCD_EN_PIN_LOW;
		GPIOB->BSRR	=	 LCD_RW_PIN_LOW;
/**
 * Configuracion de D7 as output push-pull
 */
	GPIOB->PUPDR  &= ~( 0x3UL << 30U );
 	GPIOB->OTYPER &= ~( 0x1UL << 15U );
 	GPIOB->MODER  &= ~( 0x2UL << 30U );
 	GPIOB->MODER  |=  ( 0x1UL << 30U );
		return 1;
	} else {
		GPIOB->BSRR	=  LCD_EN_PIN_LOW;
		GPIOB->BSRR	=	 LCD_RW_PIN_LOW;
/**
 * Configuracion de D7 as output push-pull
 */
		GPIOB->PUPDR  &= ~( 0x3UL << 30U );
		GPIOB->OTYPER &= ~( 0x1UL << 15U );
	 	GPIOB->MODER  &= ~( 0x2UL << 30U );
	 	GPIOB->MODER  |=  ( 0x1UL << 30U );
			return 0;
		}
	}
	//Funcion que genera un pulso en el pin EN del LCD
	void LCD_Pulse_EN(void){
		GPIOB->BSRR	=	LCD_EN_PIN_LOW;//
		delay_us(10); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! wait for	10us
		GPIOB->BSRR	=	LCD_EN_PIN_HIGH;
		delay_us(10);// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! wait for	10us
		GPIOB->BSRR	=	LCD_EN_PIN_LOW;
		delay_ms(1); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! wait for	1ms
	}
	/*
	* Funcion que muestra un caracter grafico en el LCD
	* en 'value' el valor de su posicion en CGRAM y
	* en 'size' especificamos su tamaño
	*/
	void LCD_BarGraphic(int16_t value, int16_t size){
		value = value * size / 20;//
		for( int16_t i = 0; i < size; i++ ){
			if( value > 5 ){
				LCD_Put_Char( 0x05U );
				value -= 5;
			} else {
				LCD_Put_Char( value );
				break;
			}
		}
	}
	/*
	* Funcion que muestra un caracter grafico en el LCD especificando
	* la posicion pos_x horizontal de inicio y la posicion pos_y vertical de la pantalla LCD
	*/
	void LCD_BarGraphicXY(int16_t pos_x, int16_t pos_y, int16_t value){
		LCD_Set_Cursor( pos_x, pos_y );
		for( int16_t i = 0; i < 16; i++ ){
			if( value > 5 ){
				LCD_Put_Char( 0x05U );
				value -= 5;
			} else {
				LCD_Put_Char( value );
				while( i++ < 16 )
					LCD_Put_Char( 0 );
			}
		}
}



