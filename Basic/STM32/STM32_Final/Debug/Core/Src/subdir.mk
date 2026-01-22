################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/SSD1680.c \
../Core/Src/font_cp866_8x14.c \
../Core/Src/font_cp866_8x16.c \
../Core/Src/font_cp866_8x16_r.c \
../Core/Src/font_cp866_8x8.c \
../Core/Src/font_cp866_8x8_r.c \
../Core/Src/girl15.c \
../Core/Src/haruhi15.c \
../Core/Src/main.c \
../Core/Src/noragami15.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c 

OBJS += \
./Core/Src/SSD1680.o \
./Core/Src/font_cp866_8x14.o \
./Core/Src/font_cp866_8x16.o \
./Core/Src/font_cp866_8x16_r.o \
./Core/Src/font_cp866_8x8.o \
./Core/Src/font_cp866_8x8_r.o \
./Core/Src/girl15.o \
./Core/Src/haruhi15.o \
./Core/Src/main.o \
./Core/Src/noragami15.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o 

C_DEPS += \
./Core/Src/SSD1680.d \
./Core/Src/font_cp866_8x14.d \
./Core/Src/font_cp866_8x16.d \
./Core/Src/font_cp866_8x16_r.d \
./Core/Src/font_cp866_8x8.d \
./Core/Src/font_cp866_8x8_r.d \
./Core/Src/girl15.d \
./Core/Src/haruhi15.d \
./Core/Src/main.d \
./Core/Src/noragami15.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/main.o: ../Core/Src/main.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/SSD1680.cyclo ./Core/Src/SSD1680.d ./Core/Src/SSD1680.o ./Core/Src/SSD1680.su ./Core/Src/font_cp866_8x14.cyclo ./Core/Src/font_cp866_8x14.d ./Core/Src/font_cp866_8x14.o ./Core/Src/font_cp866_8x14.su ./Core/Src/font_cp866_8x16.cyclo ./Core/Src/font_cp866_8x16.d ./Core/Src/font_cp866_8x16.o ./Core/Src/font_cp866_8x16.su ./Core/Src/font_cp866_8x16_r.cyclo ./Core/Src/font_cp866_8x16_r.d ./Core/Src/font_cp866_8x16_r.o ./Core/Src/font_cp866_8x16_r.su ./Core/Src/font_cp866_8x8.cyclo ./Core/Src/font_cp866_8x8.d ./Core/Src/font_cp866_8x8.o ./Core/Src/font_cp866_8x8.su ./Core/Src/font_cp866_8x8_r.cyclo ./Core/Src/font_cp866_8x8_r.d ./Core/Src/font_cp866_8x8_r.o ./Core/Src/font_cp866_8x8_r.su ./Core/Src/girl15.cyclo ./Core/Src/girl15.d ./Core/Src/girl15.o ./Core/Src/girl15.su ./Core/Src/haruhi15.cyclo ./Core/Src/haruhi15.d ./Core/Src/haruhi15.o ./Core/Src/haruhi15.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/noragami15.cyclo ./Core/Src/noragami15.d ./Core/Src/noragami15.o ./Core/Src/noragami15.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su

.PHONY: clean-Core-2f-Src

