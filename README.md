# STM32 files for final project of SoC Class

## To do:
- Add push button config and pwm stopper 
- Test UART Full Duplex
- Add Display Interface
- Add ADC for controlling PWM with a potentiometer

### user_tim
Implement the clock and the pwm configuration for all signals

### rcc_init
Implement clock configuration for 48 MHz

### user_uart
Implement Full-Duplex uart transmission with PA9 and PA10

## GPIOs
- PA0 		: ADC Potentiometer
- PA5 		: Normal LED
- PA6 (D12)	: push button
- PA9 (D8)	: TX uart 
- PA10 (D2)	: RX uart
- PB0 		: LCD Data
- PB1		: LCD Data
- PB2		: LCD Data
- PB3		: LCD Data 
- PB4 (D5)	: PWM signal CH1
- PB5 (D6)	: PWM signal CH2
- PB6 (D1)	: PWM signal CH3
- PB7 (D0)	: PWM signal CH4
- PB9 		: LCD
- PB10 (D4) : LCD 
- PB11 		: LCD 
- PB12.2	: LCD
- PB13		: LCD
- PB14 		: LCD
- PB15 		: LCD

		