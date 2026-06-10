################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Hardware/MPU6050/%.o: ../Hardware/MPU6050/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2020/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/BSP/MPU6050" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/BSP/MPU6050/DMP" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/CONTROL" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/Hardware/MPU6050" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/Hardware" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C" -I"D:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/Debug" -I"C:/ti/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_05_01_00/source" -DD:/mspm0/NUEDC/TraCar_6050HQ/Tra_C/Hardware/MPU6050 -gdwarf-3 -MMD -MP -MF"Hardware/MPU6050/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


