################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
BTControl/%.o: ../BTControl/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2020/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/BSP/MPU6050" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/BSP/MPU6050/DMP" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/CONTROL" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/HARDWARE" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/BTControl" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/Debug" -I"C:/ti/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_05_01_00/source" -I"C:/ti/mspm0_sdk_2_05_01_00/examples/nortos/LP_MSPM0G3507/driverlib/Tracking_Car/Tra_C/Hardware" -gdwarf-3 -MMD -MP -MF"BTControl/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


