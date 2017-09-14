// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     FlashApi.c
//
//
//	Interface to the TI Flash API
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File

#include "Flash281x_API_Library.h"
#include "Flash281x_API_Config.h"

#include "FlashApi.h"
#include "CanOpen.h"
#include "MCS.H"

/*--- Struct defined in MCS.C, monitor progress of MCS download */
extern struct MCS_DOWNLOAD_CONTROL_BLOCK mcs_download_control_block;
#define MCS_CB mcs_download_control_block

extern Uint16 ExtRamStart;

/*--- Global variables used to interface to the flash routines */
FLASH_ST EraseStatus;
FLASH_ST ProgStatus;
FLASH_ST VerifyStatus;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//  ROUTINES FROM FLASH_API EXAMPLE that   R U N   I N   F L A S H
//   but call Flash_API routines running in RAM
//   These are used by TB3CMB_BootLd for setup before erase and program.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void FLA_InitPlccr(void){
	/*------------------------------------------------------------------
	 Initalize the PLLCR value before calling any of the F2810, F2811
	 or F281x Flash API functions.

	     Check to see if the PLL needs to changed
	     PLLCR_VALUE is defined in Example_Flash281x_API.h
	     1) Make the change
	     2) Wait for the DSP to switch to the PLL clock
	        This wait is performed to ensure that the flash API functions
	        will be executed at the correct frequency.
	     3) While waiting, feed the watchdog so it will not reset.
	------------------------------------------------------------------*/
		Uint16 i;

	    if(*PLLCR != PLLCR_VALUE)
	    {
	       EALLOW;
	       *PLLCR = PLLCR_VALUE;

	       // Wait for PLL to lock
	       // Each time through this loop takes ~14 cycles
	       // PLL Lock time is 131072 Cycles
	       for(i= 0; i< 131072/14; i++){
	           *WDKEY = 0x0055;
	           *WDKEY = 0x00AA;
	       }
	       EDIS;
	    }
}

Uint16 FLA_CsmUnlock() {
//------------------------------------------------------------------
//   Unlock the code security module (CSM)
//   Parameters:
//   Return Value:
//
//            STATUS_SUCCESS         CSM is unlocked
//            STATUS_FAIL_UNLOCK     CSM did not unlock
//------------------------------------------------------------------
    volatile Uint16 temp;

    // Load the key registers with the current password
    // These are defined in Example_Flash281x_CsmKeys.asm

    EALLOW;
    *KEY0 = PRG_key0;
    *KEY1 = PRG_key1;
    *KEY2 = PRG_key2;
    *KEY3 = PRG_key3;
    *KEY4 = PRG_key4;
    *KEY5 = PRG_key5;
    *KEY6 = PRG_key6;
    *KEY7 = PRG_key7;
    EDIS;

    // Perform a dummy read of the password locations
    // if they match the key values, the CSM will unlock

    temp = *PWL0;
    temp = *PWL1;
    temp = *PWL2;
    temp = *PWL3;
    temp = *PWL4;
    temp = *PWL5;
    temp = *PWL6;
    temp = *PWL7;

    // If the CSM unlocked, return succes, otherwise return
    // failure.
    if ( (*CSMSCR & 0x0001) == 0) return STATUS_SUCCESS;
    else return STATUS_FAIL_CSM_LOCKED;
}


void FLA_UnlockCsm(void){
//-----------------------------------------------------------------
//	 Unlock the CSM.
//	    If the API functions are going to run in unsecured RAM
//	    then the CSM must be unlocked in order for the flash
//	    API functions to access the flash.
//
//	    If the flash API functions are executed from secure memory
//	    (L0/L1) then this step is not required.
//-----------------------------------------------------------------
	   Uint16 Status;

	   Status = FLA_CsmUnlock();
	   if(Status != STATUS_SUCCESS)
	   {
		   FLA_HaltOnError(Status);
	   }
}

void FLA_MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr) {
//------------------------------------------------------------------
// Used by void FLA_CopyToRam(void) to copy code to RAM
//------------------------------------------------------------------
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

void FLA_CopyToRam(void){
//------------------------------------------------------------------
//	    Copy API Functions into SARAM
//
//	    The flash API functions MUST be run out of internal
//	    zero-waitstate SARAM memory.  This is required for
//	    the algos to execute at the proper CPU frequency.
//	    If the algos are already in SARAM then this step
//	    can be skipped.
//	    DO NOT run the algos from Flash
//	    DO NOT run the algos from external memory
//------------------------------------------------------------------

	    // Copy the Flash API functions to SARAM
	    FLA_MemCopy(&Flash28_API_LoadStart, &Flash28_API_LoadEnd, &Flash28_API_RunStart);

	    // We must also copy required user interface functions to RAM.
	    FLA_MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
}

void FLA_InitScaleFactor(void){
//------------------------------------------------------------------
//  Flash_CPUScaleFactor is a 32-bit global variable that the flash
//  API functions use to scale software delays. This scale factor
//  must be initalized to SCALE_FACTOR by the user's code prior
//  to calling any of the Flash API functions. This initalization
//  is VITAL to the proper operation of the flash API functions.
//
//   SCALE_FACTOR is defined in Example_Flash281x_API.h as
//    #define SCALE_FACTOR  1048576.0L*( (200L/CPU_RATE) )
//
//	This value is calculated during the compile based on the CPU
//  rate, in nanoseconds, at which the algorithums will be run.
//------------------------------------------------------------------

	   Flash_CPUScaleFactor = SCALE_FACTOR;

}

void FLA_InitCallBackPtr(void){
//------------------------------------------------------------------
//   Flash_CallbackPtr is a pointer to a function.  The API uses
//   this pointer to invoke a callback function during the API operations.
//   If this function is not going to be used, set the pointer to NULL
//   NULL is defined in <stdio.h>.
//------------------------------------------------------------------*/

   //Flash_CallbackPtr = &MyCallbackFunction;
   Flash_CallbackPtr = 0; // NULL;

}

void FLA_ToggleTest(void){

	//  Flash_ToggleTest runs in RAM

	//  GPIOF4_SCITXDA_MASK identifies GPIOF4 which serves as SCI_A TXD
	//  Flash_ToggleTest(GPFMUX,GPFTOGGLE,GPIOF4_SCITXDA_MASK);

	//  GPIOB0_MASK identifies GPIOB0 which happens to be left-most DSP LED on TB3CMB
	    Flash_ToggleTest(GPBMUX,GPBTOGGLE,GPIOB0_MASK);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//  ROUTINES MODELED AFTER FLASH_API EXAMPLE that   R U N   I N   R A M
//   These are used by TB3CMB_BootLd for erase and program.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#pragma CODE_SECTION(FLA_EraseAndReProgram,"ramfuncs");
void FLA_EraseAndReProgram(void){
	Uint16 Status;
	Uint32 Length;         // Number of 16-bit values to be programmed
	Uint16 *Prog_Buffer;
	union FLASH16_32 Flash_ptr;

	FLA_leds(0x0001); // leftmost LED ON, *___

	Status = Flash_Erase((MCS_CB.flash_sectors_to_program),&EraseStatus);
	if(Status != STATUS_SUCCESS) {
	       FLA_HaltOnError(Status);
	}

	FLA_leds(0x0002); // 2nd from leftmost LED ON, _*__ from left

 	Prog_Buffer = (Uint16 *)(&ExtRamStart);  //0x10,0000 - symbolic address from .cmd file
	Flash_ptr.all = MCS_CB.new_program_start_addr;
	Length = MCS_CB.new_program_buff_count;
    Status = Flash_Program((Uint16*)Flash_ptr.all,Prog_Buffer,Length,&ProgStatus);
    if(Status != STATUS_SUCCESS)
    {
        FLA_HaltOnError(Status);
    }

    while (1){
    // Use Watchdog to Reset DSP
      FLA_leds(0x0004); // 2nd from rightmost LED ON, __*_ from left
      EALLOW;
      *WDCR = 0x0028; // enable the watchdog
       // Anything other than writing 0x55 immediately
       // followed by 0xAA causes a watchdog reset
      *WDKEY = 0x0666;
      EDIS;
    }

    // Stop here in case there is a delay before reset hits
    //FLA_Example_Done(); // but we seem to be stuck at the ESTOP0 and not do RESET
 }

#pragma CODE_SECTION(FLA_HaltOnError,"ramfuncs");
void FLA_HaltOnError(Uint16 Status){
//------------------------------------------------------------------
//  For this example, if an error is found just stop here
//-----------------------------------------------------------------*/

//  Error code will be in the AL register.
	FLA_leds(0x0005); // *_*_ from left
    asm("    ESTOP0");
    asm("    SB 0, UNC");
}

/*------------------------------------------------------------------
  For this example, once we are done just stop here
-----------------------------------------------------------------*/
#pragma CODE_SECTION(FLA_Example_Done,"ramfuncs");
void FLA_Example_Done(void){
	FLA_leds(0x000A); // _*_* from left
    asm("    ESTOP0");
    asm("    SB 0, UNC");
}
#pragma CODE_SECTION(FLA_leds,"ramfuncs");
void FLA_leds(Uint16 ledData){
	   // 0x0008 rightmost LED
	   // 0x0001 leftmost LED
	   // set -> led on, clear -> led off
	   GpioDataRegs.GPBSET.all  |= 0x000F; // all off
       GpioDataRegs.GPBCLEAR.all |= ledData; // on
}

