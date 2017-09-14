################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
BL_main.obj: ../BL_main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="BL_main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

BootLdEntry.obj: ../BootLdEntry.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="BootLdEntry.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanComm.obj: ../CanComm.C $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="CanComm.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

CanOpen.obj: ../CanOpen.C $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="CanOpen.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSP281x_CSMPasswords.obj: ../DSP281x_CSMPasswords.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP281x_CSMPasswords.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSP281x_CodeStartBranch.obj: ../DSP281x_CodeStartBranch.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP281x_CodeStartBranch.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSP281x_ECan.obj: ../DSP281x_ECan.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP281x_ECan.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSP281x_GlobalVariableDefs.obj: ../DSP281x_GlobalVariableDefs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP281x_GlobalVariableDefs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSP281x_SysCtrl.obj: ../DSP281x_SysCtrl.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="DSP281x_SysCtrl.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Example_Flash281x_CsmKeys.obj: ../Example_Flash281x_CsmKeys.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="Example_Flash281x_CsmKeys.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

FlashApi.obj: ../FlashApi.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="FlashApi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LED.obj: ../LED.C $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="LED.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

MCS.obj: ../MCS.C $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="MCS.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

TimeStamp.obj: ../TimeStamp.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="TimeStamp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

WorkAround.obj: ../WorkAround.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="WorkAround.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Xintf.obj: ../Xintf.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="Xintf.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

access32.obj: ../access32.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c2000_6.1.0/bin/cl2000" -v28 -ml -mt -g --include_path="C:/ti/ccsv5/tools/compiler/c2000_6.1.0/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_headers/include" --include_path="C:/TI_SPRCO97_C281x_Header_n_Peripherals_Ex/tidcs/c28/DSP281x/v120/DSP281x_common/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="access32.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


