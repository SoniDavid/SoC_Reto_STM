/**
 ******************************************************************************
 * @file           : main.c
 * @author         : D Soni
 * @briefs          : Main program body
 ******************************************************************************
 */

#if !defined(_SOFT_FP) && defined(_ARM_FP)
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

//RTOS
#include "FreeRTOSConfig.h"
#include "app_freertos.h"
#include "queue.h"

//DEFINITIONS
#define RX_BUFFER_SIZE 32
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

//RTOS
TaskHandle_t hTask_UpdateInputs;
TaskHandle_t hTask_UpdateOutputs;
TaskHandle_t hTask_Transmit_data;

QueueHandle_t xQueueI, xQueueO;

//GLOBAL VARIABLES
volatile uint16_t acceleration = 0;
volatile uint8_t button_state = 0;

volatile uint32_t tim16_tick = 0;

/* Superloop structure */
int main(void) {
  /* Initialization of Peripherals */
  USER_RCC_Init(); 				// Set CLK to 48MHz
  USER_SysTick_Init();			//

  // Initialize Inputs
  USER_GPIO_Init();				// Initialize push button (break)
  USER_TIM3_PWM_Init();			// Set TIM3 CH1-4 to PWM

  // UARTs
  USER_USART1_Init();			// Enable Full-Duplex UART communication
  USER_UART2_Init();

  // Initialize Outputs
  //LCD_Init();					// Initialize LCD
  USER_ADC_Init();

  //Timers
  USER_TIM14_Init();			// Enable TIM14 for Delay
  USER_TIM16_Init();
  USER_TIM17_Init_Timer();

  // Create Task
  xTaskCreate(UpdateInputs, "UpdateInputs", 32, NULL, 1, &hTask_UpdateInputs);
  xTaskCreate(UpdateOutputs, "UpdateOutputs", 32, NULL, 2, &hTask_UpdateOutputs);
  xTaskCreate(transmit_data, "Transmit_data", 32, NULL, 3, &hTask_Transmit_data);

  //Create Queues
  xQueueI = xQueueCreate( 3, sizeof(struct message_inputs * ));
  xQueueO = xQueueCreate( 3, sizeof(struct message_outputs * ));

  //Local variables
  uint32_t start_tx = 0;
  uint32_t start_led = 0; // used only for debugging

  // Start the scheduler
  //printf("works1\r\n");
  vTaskStartScheduler();
  //printf("works2\r\n");

  GPIOA->ODR ^= (0x1UL << 5U); // Toggle USER LED

  for(;;){
	  //printf("works\r\n");
	  if(delay_elapsed(&start_tx, 100)){
		  //printf("works\r\n");
	  }
	  if(delay_elapsed(&start_led, 100)) {
		  //printf("works2\r\n");
		  GPIOA->ODR ^= (0x1UL << 5U); // Toggle USER LED
	  }
  }
}


void USART1_IRQHandler(void) {
   if (USART1->ISR & (1UL << 5U)) { // RXNE
    uint8_t received = (uint8_t) (USART1->RDR & 0xFF);
    char rx_buffer[RX_BUFFER_SIZE] = {0};
    uint8_t rx_index = 0;

    if (received == 'I') {
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
      rx_buffer[rx_index++] = received;
      return;
    }

    if (rx_buffer[0] == 'I') {
      if (received == 'E') {
        rx_buffer[rx_index] = '\0';

        uint8_t rpm_i2, vl_i1, vl_i2, gear_i;
        uint16_t rpm_i1;
        if (sscanf(&rx_buffer[1], "%hu,%hhu,%hhu,%hhu,%hhu,", &rpm_i1, &rpm_i2, &vl_i1,
            &vl_i2, &gear_i) == 5) {
          message_outputs data;
          data.rpm = rpm_i1 * 100 + rpm_i2;
          data.vl = vl_i1 * 100 + vl_i2;
          data.gear = gear_i;
          if(xQueueSend(xQueueO, &data, 10 ) != pdPASS)
          {
        	  xQueueReceive(xQueueO, &data, 0);
        	  xQueueSend(xQueueO, &data, 10);
          }
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

void UpdateInputs(void *pvParameters) {
	message_inputs data;
	TickType_t xLastWakeTime = xTaskGetTickCount();;
	for(;;){
		//printf("worksTask1\r\n");
		// Read ADC and update global acceleration
		data.acceleration = USER_ADC_Read();
		// Read push button (PA6), 0 if pressed, 1 if not pressed
		data.button_state = (GPIOA->IDR & (1UL << 6U)) ? 0 : 1;

		//printf("send %d \r\n", data.acceleration);
		if(xQueueSend(xQueueI, &data, 10) != pdPASS)
		{
			//printf("send\r\n");
			xQueueReceive(xQueueI, &data, 0);
			xQueueSend(xQueueI, &data, 10);
		}
		vTaskDelayUntil(&xLastWakeTime, 10);
	}
}

void UpdateOutputs(void *pvParameters){
	message_outputs data;
	TickType_t xLastWakeTime = xTaskGetTickCount();;
	for(;;)
	{
		//printf("worksTask2\r\n");
		if(xQueueReceive(xQueueO, &data, 10) == pdPASS){
			//printf("worksTask2\r\n");
			//UpdateLCD(data.rpm, data.gear, data.vl);
			Update_PWM_From_Velocity(data.vl);
		}
		vTaskDelayUntil(&xLastWakeTime, 500);
	}
}

void transmit_data(void *pvParameters) {
  uint8_t tx_buffer[32];
  uint8_t acc_i1 = 0.0;
  uint8_t acc_i2 = 0.0;
  uint8_t btn_i = 0.0;
  message_inputs data;
  TickType_t xLastWakeTime = xTaskGetTickCount();;

  for(;;){
	  //printf("worksTask3\r\n");
	  if(xQueueReceive(xQueueI, &data, 10) == pdPASS){
		  acc_i1 = data.acceleration / 100;
		  acc_i2 = data.acceleration % 100;
		  btn_i = (uint8_t) (data.button_state);
		  //printf("worksTask3 %d\r\n", acc_i1);

		  // Format: I<acc_i1>.<acc_i2>,<button>,E
		  sprintf((char*) tx_buffer, "I%d.%02d,%d,E\n", acc_i1, acc_i2, btn_i);

		  USER_USART1_Transmit(tx_buffer, strlen((char*) tx_buffer));
	  }
	  vTaskDelayUntil(&xLastWakeTime, 300);
  }
}

uint8_t delay_elapsed(uint32_t *start, uint32_t n_ticks) {
  if ((tim16_tick - *start) >= n_ticks) {
    *start = tim16_tick;
    return 1;
  }
  return 0;
}

void Update_PWM_From_Velocity(uint16_t input_vl) {
	// Clamp the input to [0.0, 200.0] for safety
	if (input_vl < 0)
		input_vl = 0;
	if (input_vl > 200)
		input_vl = 200;

	// Map velocity to 0–100% PWM duty
	uint8_t duty = (uint8_t) ((input_vl / 200.0f) * 100.0f);

	USER_Set_PWM_Duty(duty);
}

void UpdateLCD(uint16_t rpm, uint16_t gear, uint16_t vl){
  LCD_Clear();
  LCD_Set_Cursor(1, 1);
  LCD_Put_Str("Rpm: ");
  LCD_Put_Num(rpm);

  LCD_Set_Cursor(2, 1);
  LCD_Put_Str("Gr:");
  LCD_Put_Num(gear);

  LCD_Set_Cursor(2, 8);
  LCD_Put_Str("Sp: ");
  LCD_Put_Num(vl);
}

void TIM16_IRQHandler(void) {
  if (TIM16->SR & (1UL << 0)) {
    TIM16->SR &= ~(1UL << 0);
    tim16_tick++;
  }
}
