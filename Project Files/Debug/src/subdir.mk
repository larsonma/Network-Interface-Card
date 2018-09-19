################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ADC.c \
../src/RTC.c \
../src/gpio.c \
../src/keypad.c \
../src/lcd.c \
../src/main.c \
../src/piezo.c \
../src/ringbuffer.c \
../src/syscalls.c \
../src/timer.c \
../src/uart_driver.c 

OBJS += \
./src/ADC.o \
./src/RTC.o \
./src/gpio.o \
./src/keypad.o \
./src/lcd.o \
./src/main.o \
./src/piezo.o \
./src/ringbuffer.o \
./src/syscalls.o \
./src/timer.o \
./src/uart_driver.o 

C_DEPS += \
./src/ADC.d \
./src/RTC.d \
./src/gpio.d \
./src/keypad.d \
./src/lcd.d \
./src/main.d \
./src/piezo.d \
./src/ringbuffer.d \
./src/syscalls.d \
./src/timer.d \
./src/uart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DDEBUG -I"C:/Users/larsonma/Documents/CE 4951/Project/Network Card/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


