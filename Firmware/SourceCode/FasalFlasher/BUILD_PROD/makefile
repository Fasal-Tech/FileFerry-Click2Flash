################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User_Files/xModem/subdir.mk
-include User_Files/AppStorage/AppSDFS/subdir.mk
-include User_Files/AppStorage/AppFlashFS/W25Qxx/subdir.mk
-include User_Files/AppStorage/AppFlashFS/LittleFS/subdir.mk
-include User_Files/AppStorage/AppFlashFS/subdir.mk
-include User_Files/AppStorage/subdir.mk
-include User_Files/AppFasal/subdir.mk
-include User_Files/AppCommon/TriColorLED/subdir.mk
-include User_Files/AppCommon/PushButton/subdir.mk
-include User_Files/AppCommon/Console/subdir.mk
-include User_Files/AppCommon/ConfigSetting/subdir.mk
-include User_Files/AppCommon/AppUtility/SoftTimer/subdir.mk
-include User_Files/AppCommon/AppUtility/AppProfiler/subdir.mk
-include User_Files/AppCommon/AppConfiguration/subdir.mk
-include User_Files/AppCommon/subdir.mk
-include Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include Middlewares/Third_Party/FatFs/src/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := FasalFlasher
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
FasalFlasher.elf \

MAP_FILES += \
FasalFlasher.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FasalFlasher.list \

OBJCOPY_SREC += \
FasalFlasher.srec \


# All Target
all: main-build

# Main-build Target
main-build: FasalFlasher.elf secondary-outputs

# Tool invocations
FasalFlasher.elf FasalFlasher.map: $(OBJS) $(USER_OBJS) D:\Workspace\fasalflasher_firmware\SourceCode\FasalFlasher\STM32F103RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "FasalFlasher.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\Workspace\fasalflasher_firmware\SourceCode\FasalFlasher\STM32F103RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FasalFlasher.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FasalFlasher.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FasalFlasher.list"
	@echo 'Finished building: $@'
	@echo ' '

FasalFlasher.srec: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O srec $(EXECUTABLES) "FasalFlasher.srec"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) FasalFlasher.elf FasalFlasher.list FasalFlasher.map FasalFlasher.srec default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_SREC)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
