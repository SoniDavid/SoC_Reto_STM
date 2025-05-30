/*
 * user_tim.h
 *
 *  Created on: May 19, 2025
 *      Author: sonid
 */

#ifndef USER_TIM_H_
#define USER_TIM_H_

#include "main.h"

/* General Purpose Timers registers */
typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RESERVED1;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t RESERVED2;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t RESERVED3[5];
	volatile uint32_t AF1;
	volatile uint32_t RESERVED4;
	volatile uint32_t TISEL;
} TIM_TypeDef;

#define TIM3_BASE	0x40000400UL//	TIM3 base address
#define TIM3    	(( TIM_TypeDef * )TIM3_BASE )

#define TIM14_BASE 0x40002000UL //TIM14 base address
#define TIM14 ((TIM_TypeDef *) TIM14_BASE)

#define TIM16_BASE	0x40014400UL// 	TIM16 base address
#define TIM16    	(( TIM_TypeDef * )TIM16_BASE )

// TIM3 functions
void USER_TIM3_PWM_Init(void);
uint16_t USER_Duty_Cycle(uint8_t duty);
void USER_Set_PWM_Duty(uint8_t duty_percent);

// TIM14 functions
void USER_TIM14_Init(void); // Defined for using as debouunce timing
void USER_TIM14_Delay(uint16_t prescaler, uint16_t AutoReload);

// TIM16 functions
void USER_TIM16_Init(void);


#endif /* USER_TIM_H_ */
