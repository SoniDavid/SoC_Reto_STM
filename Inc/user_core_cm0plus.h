/*
 * user_core_cm0plus.h
 *
 *  Created on: May 24, 2025
 *      Author: sonid
 */

#ifndef USER_CORE_CM0PLUS_H_
#define USER_CORE_CM0PLUS_H_

/* System Time registers */
typedef struct
{
	volatile uint32_t CSR;
	volatile uint32_t RVR;
	volatile uint32_t CVR;
	volatile uint32_t CALIB;
} Systick_TypeDef;

#define Systick_BASE  0xE000E010UL//	Systick base address
#define Systick    	  ((Systick_TypeDef * )Systick_BASE)

void USER_SysTick_Init(void);
void SysTick_Delay(uint32_t time);

#endif /* USER_CORE_CM0PLUS_H_ */
