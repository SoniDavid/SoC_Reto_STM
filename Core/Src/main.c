/**
 ******************************************************************************
 * @file           : main.c
 * @author         : D Soni
 * @briefs          : Main program body
 ******************************************************************************
 */

//INCLUDE FILES
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "user_tim.h"
#include "user_uart.h"
#include "lcd.h"
#include "user_adc.h"

//RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

//DEFINITIONS
#define RX_BUFFER_SIZE 64
#define BIT_TAREA1 (1 << 0)

// GLOBAL VARIABLES
char rx_buffer[RX_BUFFER_SIZE] = {0};
uint8_t rx_index = 0;

//RTOS
TaskHandle_t hTask_UpdateInputs;
TaskHandle_t hTask_UpdateOutputs;
TaskHandle_t hTask_Transmit_data;
TaskHandle_t hTask_SimulateInputs;

EventGroupHandle_t Event1Handle;

QueueHandle_t xQueueI;
QueueHandle_t xQueueO;

uint16_t rpm, vl, gear;


/* Superloop structure */
int main(void) {
  /* Initialization of Peripherals */
  HAL_Init();
  USER_RCC_Init(); 				// Set CLK to 48MHz

  // Initialize Inputs
  USER_GPIO_Init();				// Initialize push button (break)
  USER_TIM3_PWM_Init();			// Set TIM3 CH1-4 to PWM

  // UARTs
  USER_USART1_Init();			// Enable Full-Duplex UART communication
  //USER_UART2_Init();

  // Timers
  USER_TIM14_Init();			// Enable TIM14 for Delay
  //USER_TIM16_Init();
  //USER_TIM17_Init_Timer();

  // Initialize Outputs
  LCD_Init();					// Initialize LCD

  //Initialize Inputs
  USER_ADC_Init();

  //Driver Modules
  L298N_Init();

  // Create Task
  xTaskCreate(UpdateInputs, "UpdateInputs", 128, NULL, 3, &hTask_UpdateInputs);
  xTaskCreate(UpdateOutputs, "UpdateOutputs", 128, NULL, 1, &hTask_UpdateOutputs);
  xTaskCreate(transmit_data, "Transmit_data", 128, NULL, 2, &hTask_Transmit_data);
  xTaskCreate(BridgeUART_data, "BridgeUART_data", 128, NULL, 2, &hTask_SimulateInputs);

  //Create Queues
  xQueueI = xQueueCreate( 2, sizeof(struct message_inputs * ));
  xQueueO = xQueueCreate( 2, sizeof(struct message_outputs * ));

  // Start the scheduler
  vTaskStartScheduler();
}


void USART1_IRQHandler(void) {
   if (USART1->ISR & (1UL << 5U)) { // RXNE
    uint8_t received = (uint8_t) (USART1->RDR & 0xFF);
    int rpm_i1 = 0, rpm_i2 = 0, vl_i1 = 0, vl_i2 = 0, gear_i = 0;

    if (received == 'I') {
      rx_index = 0;
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
      rx_buffer[rx_index++] = received;
      return;
    }

    if (rx_buffer[0] == 'I') {
      if (received == 'E') {
        rx_buffer[rx_index] = '\0';

        GPIOA->ODR ^= (0x1UL << 5U); // Toggle USER LED
        if (sscanf(&rx_buffer[1], "%d,%d,%d,%d,%d", &rpm_i1, &rpm_i2, &vl_i1,
            &vl_i2, &gear_i) == 5) {

        	//printf("Parsed values -> rpm: %d.%d, vl: %d.%d, gear: %d\r\n", rpm_i1, rpm_i2, vl_i1, vl_i2, gear_i);


			rpm = (uint16_t)((rpm_i1 * 100) + rpm_i2);
			vl = (uint16_t)((vl_i1 * 100) + vl_i2);
			gear = (uint16_t)(gear_i);

			rpm = (uint16_t)(rpm_i1);
			vl = (uint16_t)(vl_i1);
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

		if(xQueueSend(xQueueI, &data, 10) != pdPASS)
		{
			xQueueReceive(xQueueI, &data, 0);
			xQueueSend(xQueueI, &data, 10);
		}
		vTaskDelayUntil(&xLastWakeTime, 10);
	}
}

void UpdateOutputs(void *pvParameters){
	message_outputs data;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		//printf("worksTask2\r\n");
		if(xQueueReceive(xQueueO, &data, 10) == pdPASS){
			//printf("worksTask2\r\n");
			Update_PWM_From_Velocity(data.vl);
			UpdateLCD(data.rpm, data.gear, data.vl);
		}
		vTaskDelayUntil(&xLastWakeTime, 1000);
	}
}

void transmit_data(void *pvParameters) {
  uint8_t tx_buffer[32];
  message_inputs data;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for(;;){
	  //printf("worksTask3\r\n");
	  if(xQueueReceive(xQueueI, &data, 10) == pdPASS){

		  // Format: I<acc_i1>.<acc_i2>,<button>,E
		  sprintf((char*) tx_buffer, "I%d.%02d,%d,E\n", data.acceleration / 100, data.acceleration % 100, (data.button_state));

		  USER_USART1_Transmit(tx_buffer, strlen((char*) tx_buffer));
	  }
	  vTaskDelayUntil(&xLastWakeTime, 200);
  }
}

void Update_PWM_From_Velocity(uint16_t input_vl) {
	// Clamp the input to [0.0, 200.0] for safety
	if (input_vl < 0)
		input_vl = 0;
	if (input_vl > 200)
		input_vl = 200;

	// Map velocity to 0–100% PWM duty
	USER_Set_PWM_Duty((uint8_t) (input_vl));
}

void BridgeUART_data(void *pvParameters){
    message_outputs simulatedData;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for(;;){
		simulatedData.rpm = rpm;
		simulatedData.gear = gear;
		simulatedData.vl = vl;

        // Send data to Queue
        xQueueSend(xQueueO, &simulatedData, portMAX_DELAY);

        // Wait 300ms
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(300));
    }
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


void USER_GPIO_Init(void) {
  // Enable GPIOA CLK
  RCC->IOPENR = RCC->IOPENR | (0x1UL << 0U);

  // Configure PA6 as input w pull up
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x2UL << 12U);
  GPIOA->PUPDR = GPIOA->PUPDR | (0x1UL << 12U);
  GPIOA->MODER = GPIOA->MODER & ~(0x3UL << 12U);

  // Configure PA5 as output push pull (It comes in the LCD, so JIC)
  GPIOA->BSRR = 0x1UL << 21U; // Reset PA5 low to turn off LED
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 10U); // Clear pull-up/pull-down bits for PA5
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 5U); // Clear output type bit for PA5
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 10U); // Set PA5 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 10U); // Set PA5 as output
}

void USER_RCC_Init( void ){
	/* System Clock (SYSCLK) configuration for 48 MHz */
	FLASH->ACR	&= ~( 0x6UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	FLASH->ACR	|=  ( 0x1UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	while(( FLASH->ACR & ( 0x7UL <<  0U )) != 0x001UL );// wait until LATENCY[2:0]=001
	RCC->CR		&= ~( 0x7UL << 11U );// select HSISYS division factor by 1
	while(!( RCC->CR & ( 0x1UL << 10U )));// wait until HSISYS is stable and ready
	RCC->CFGR	&= ~( 0x7UL <<  0U );// select HSISYS as the SYSCLK clock source
	RCC->CFGR	&= ~( 0x1UL << 11U );// select HCLK division factor by 1
	SystemCoreClock = 48000000;
}

void L298N_Init(void){
  //Configure Output Pins for Motor Driver

  //IN1
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 2U); // Clear pull-up/pull-down bits for PA1
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 1U); // Clear output type bit for PA1
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 2U); // Set PA1 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 2U); // Set PA1 as output

  //IN2
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 4U); // Clear pull-up/pull-down bits for PA2
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 2U); // Clear output type bit for PA2
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 4U); // Set PA2 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 4U); // Set PA2 as output

  //IN3
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 6U); // Clear pull-up/pull-down bits for PA3
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 3U); // Clear output type bit for PA3
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 6U); // Set PA3 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 6U); // Set PA3 as output

  //IN4
  GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 4U); // Clear pull-up/pull-down bits for PA4
  GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 2U); // Clear output type bit for PA4
  GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 4U); // Set PA4 as output
  GPIOA->MODER = GPIOA->MODER | (0x1UL << 4U); // Set PA4 as output

  //Set pins for forward configuration

  //Motor1: IN1=1, IN2=0
  GPIOA->ODR = GPIOA->ODR | (0x1UL << 1U);
  GPIOA->ODR = GPIOA->ODR & ~(0x1UL << 2U);

  //Motor2: IN3=1, IN4=0
  GPIOA->ODR = GPIOA->ODR | (0x1UL << 3U);
  GPIOA->ODR = GPIOA->ODR & ~(0x1UL << 4U);
}
