/**
 ******************************************************************************
 * @file           : main.c
 * @author         : D Soni
 * @briefs          : Main program body
 ******************************************************************************
 */

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

//INCLUDE FILES
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "user_tim.h"
#include "rcc_init.h"
#include "gpio_init.h"
#include "user_uart.h"
#include "lcd.h"
#include "delays.h"
#include "user_core_cm0plus.h"
#include "user_adc.h"
#include "tests.h"

//DEFINITIONS
#define RX_BUFFER_SIZE 64

//GLOBAL VARIABLES
volatile float rpm = 0.0f, vl = 0.0f;
volatile float gear = 0.0f;
volatile uint16_t acceleration = 0;
volatile uint8_t button_state = 0;
volatile uint8_t paqueteListo = 0;

volatile uint32_t tim16_tick = 0;
char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_index = 0;



/* Superloop structure */
int main(void)
{
	/* Initialization of Peripherals */
	USER_RCC_Init(); 				// Set CLK to 48MHz
	USER_SysTick_Init();		//
  USER_GPIO_Init();				// Initialize push button (break)
  USER_TIM3_PWM_Init();		// Set TIM3 CH1-4 to PWM
  USER_TIM14_Init();			// Enable TIM14 for Delay
  USER_USART1_Init();			// Enable Full-Duplex UART communication
//	LCD_Init();							// Initialize LCD
	USER_ADC_Init();
	USER_TIM16_Init();

	//Local variables
	uint32_t start_tx = 0;
	uint32_t start_led = 0;

  for(;;){
		if (paqueteListo) {
			paqueteListo = 0;
			Update_PWM_From_Velocity(vl);
			// Aquí puedes procesar los datos almacenados en rpm, vl y gear
      Update_PWM_From_Velocity(vl);
		}

		if (delay_elapsed(&start_tx, 100)) {
			Update_Inputs();
			transmit_data();
		}

		if (delay_elapsed(&start_led, 100)) {
			GPIOA->ODR ^= (0x1UL << 5U); // Toggle USER LED
		}


	}

}


void USART1_IRQHandler(void) {
    if (USART1->ISR & (1UL << 5U)) { // RXNE
        uint8_t received = (uint8_t)(USART1->RDR & 0xFF);

        if(received == 'I') {
            rx_index = 0;
            memset(rx_buffer, 0, RX_BUFFER_SIZE);
            rx_buffer[rx_index++] = received;
            return;
        }

        if (rx_buffer[0] == 'I') {
            if(received == 'E') {
                rx_buffer[rx_index] = '\0';

                int rpm_i1, rpm_i2, vl_i1, vl_i2, gear_i;
                if (sscanf(&rx_buffer[1], "%d,%d,%d,%d,%d,", &rpm_i1, &rpm_i2, &vl_i1, &vl_i2, &gear_i) == 5) {
                    rpm = rpm_i1 + (rpm_i2 / 100.0f);
                    vl = vl_i1 + (vl_i2 / 100.0f);
                    gear = (float)gear_i;
                    paqueteListo = 1;
                }
            } else if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = received;
            } else {
                rx_index = 0;
                memset(rx_buffer, 0, RX_BUFFER_SIZE);
            }
        }
    }
}

//void transmit_data(){
//	uint8_t tx_buffer[64];
//	int rpm_i = (int)(rpm * 100);
//	int vl_i = (int)(vl * 100);
//	int gear_i = (int)(gear);
//	sprintf((char*)tx_buffer,
//		             "I%d.%02d,%d.%02d,%d,E\n",
//		             rpm_i / 100, rpm_i % 100,
//		             vl_i / 100,  vl_i % 100,
//		             gear_i);
//	USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
//}

void transmit_data() {
    uint8_t tx_buffer[64];

    int rpm_i = (int)(rpm * 100);
    int vl_i = (int)(vl * 100);
    int gear_i = (int)(gear);
    int acc_i1 = acceleration / 100;
    int acc_i2 = acceleration % 100;
    int btn_i = (int)(button_state);

    // Format everything as comma-separated values
    sprintf((char*)tx_buffer,
            "I%d.%02d,%d.%02d,%d,%d.%02d,%d,E\n",
            rpm_i / 100, rpm_i % 100,
            vl_i / 100,  vl_i % 100,
            gear_i,
            acc_i1,
            acc_i2,
            btn_i);

    USER_USART1_Transmit(tx_buffer, strlen((char*)tx_buffer));
}


void TIM16_IRQHandler(void) {
    if (TIM16->SR & (1UL << 0)) {
        TIM16->SR &= ~(1UL << 0);
        tim16_tick++;
    }
}

uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks) {
    if ((tim16_tick - *start) >= n_ticks) {
        *start = tim16_tick;
        return 1;
    }
    return 0;
}

void Update_Inputs(void) {
    // Read ADC and update global acceleration
    acceleration = USER_ADC_Read();

    // Read push button (PA6), 0 if pressed, 1 if not pressed
    button_state = (GPIOA->IDR & (1UL << 6U)) ? 0 : 1;
}



