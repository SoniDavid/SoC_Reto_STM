# STM32 files for final project of SoC Class

## Current Status:
System works with all sensors. The UART full duplex sends and recieves via chars. PWM works although due to the small changes it doesn't notice as much with LEDs as it would do with motors

## To do:
- Change file structure for easier understandment
- Add FreeRTOS
- Change PWM to use motors with L298N H bridges

## Files Structure

| File      | Description   |
|-----------|---------------|
|user_tim   | CLK and PWM configuration for all signals |
|rcc_init   | Internal CLK configuration for 48 MHz     |
|user_uart  | Full-Duplex uart transmission configuration| 
|gpio_init  | Intialization of Push button and internal LED |
|lcd        | LCD complete configuration                    |
|user_core_cm0plus| Configuration of Systick (Mainly used for debugging)|
|user_adc   | Adc configuration to be used with potentiometer|
|delays     | Delays implementation used for LCD             |
|tests      | Files for testing individual functionality of inputs/outputs|

## GPIOs
|PIN 		| FUNCTION				|
|-----------|-----------------------|
| PA0 		| ADC Potentiometer		|
| PA5 		| Normal LED			|
| PA6 (D12)	| push button			|
| PA9 (D8)	| TX uart 				|
| PA10 (D2)	| RX uart				|
| PB0 		| LCD Data				|
| PB1		| LCD Data				|
| PB2		| LCD Data				|
| PB3		| LCD Data 				|
| PB4 (D5)	| PWM signal CH1		|
| PB5 (D6)	| PWM signal CH2		|
| PB6 (D1)	| PWM signal CH3		|
| PB7 (D0)	| PWM signal CH4		|
| PB9 		| LCD					|
| PB10 (D4) | LCD 					|
| PB11 		| LCD 					|
| PB12.2	| LCD					|
| PB13		| LCD					|
| PB14 		| LCD					|
| PB15 		| LCD					|


		