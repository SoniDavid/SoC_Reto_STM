################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio_init.c \
../Src/main.c \
../Src/rcc_init.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/user_tim.c \
../Src/user_uart.c 

OBJS += \
./Src/gpio_init.o \
./Src/main.o \
./Src/rcc_init.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/user_tim.o \
./Src/user_uart.o 

C_DEPS += \
./Src/gpio_init.d \
./Src/main.d \
./Src/rcc_init.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/user_tim.d \
./Src/user_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DNUCLEO_C031C6 -DSTM32 -DSTM32C0 -DSTM32C031C6Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/gpio_init.cyclo ./Src/gpio_init.d ./Src/gpio_init.o ./Src/gpio_init.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/rcc_init.cyclo ./Src/rcc_init.d ./Src/rcc_init.o ./Src/rcc_init.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/user_tim.cyclo ./Src/user_tim.d ./Src/user_tim.o ./Src/user_tim.su ./Src/user_uart.cyclo ./Src/user_uart.d ./Src/user_uart.o ./Src/user_uart.su

.PHONY: clean-Src

