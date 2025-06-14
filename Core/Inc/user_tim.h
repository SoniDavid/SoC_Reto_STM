/*
 * user_tim.h
 *
 *  Created on: Jun 10, 2025
 *      Author: abrah
 */

#ifndef INC_USER_TIM_H_
#define INC_USER_TIM_H_

void USER_TIM3_PWM_Init( void );
void USER_TIM14_Init(void);
void USER_TIM14_Delay(uint16_t, uint32_t);
uint16_t USER_Duty_Cycle( uint8_t);
void USER_Set_PWM_Duty(uint8_t);

#endif /* INC_USER_TIM_H_ */
