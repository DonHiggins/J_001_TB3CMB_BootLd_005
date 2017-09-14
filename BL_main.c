// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     BL_main.c
//
// Bootloader main() entry point
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "FlashApi.h"
#include "LED.H"
#include "CanComm.H"
#include "CanOpen.h"
#include "MCS.H"
#include "Xintf.H"
#include "stdbool.h"            // needed for bool data types

extern Uint16 Flash28_API_LoadStart;
extern Uint16 Flash28_API_LoadEnd;
extern Uint16 Flash28_API_RunStart;
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

void InitSysCtrl(void);		// found in the DSP281x_SysCtrl.c file
void bl_jumpToMain(void);
void bl_doReset(void);


// used to manage download of whole MCS <Motorola format object> file
extern struct MCS_DOWNLOAD_CONTROL_BLOCK mcs_download_control_block;
#define MCS_CB mcs_download_control_block

Uint16 idle_loop_count;
Uint16 do_reset;
Uint16 do_jump_to_main;
Uint16 vectored_from_main;

void main(void) {
	// Disable CPU interrupts
	   DINT;

	// Arrived here from _c_int00 initialization
	if (GpioMuxRegs.GPBDIR.all == 0x0000) {
		// Got here due to a power-up reset or watchdog reset
		vectored_from_main = 0;
	} else {
		// GPIOB is set up for LED outputs,
		// This means we got here from the main program, via the _vector_from_main_prog
		vectored_from_main = 1;
	}

	// Step 0. Zero out page of RAM so we can observe loading from Flash to RAM
	Uint16* zPtr;
	Uint16  j;
	zPtr = &Flash28_API_RunStart;
	for (j=0;j<0x1000;j++){
		*(zPtr++) = 0;
	}

	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP281x_SysCtrl.c file.
	   InitSysCtrl();

	// Initialize GPIO for benefit of LEDs
	   GpioU_defaultInit(); // in LED module
	   GpioU_initGpiosAllZero();
	   GpioU_DspLedsInit();

	// we store downloaded program data in our external RAM, on the external bus
	   xintf_InitXintf(); // External Bus Interface initialization.

	   canC_initComm();  // further setup in the CanComm.c
	   mcs_init_mcs_cb();

	// Step 7. IDLE loop. Just sit and loop forever :
	   idle_loop_count = 0;
	   do_reset = 0;

	   if(vectored_from_main == 1) {
		   // then stick around in the bootloader
		   do_jump_to_main = 0;
	   } else {
		   // didn't get here from main program, must have been from
		   // power-on reset or watchdog reset, so proceed to main program
		   do_jump_to_main = 1;
	   }

	   led_simpleTimerInit();

	// MAIN LOOP
	   for(;;){
		   idle_loop_count++; // just a convenient place for breakpoint, not functional

		   led_simpleTimer(); // increment counter, used to time LEDs
		   led_simpleHeartbeat();

		   canC_pollRecv(); // see if we have received anything on CAN
		   if (mcs_readyToProgram()) {



			   if(MCS_CB.flash_sectors_to_program != 0) {
				  // for safety, don't do this if we haven't loaded an MCS file.

				   led_leds(0x0000); // all off
				   // Initialization from Flash API runs from Flash
				   FLA_InitPlccr();
				   FLA_UnlockCsm();
				   FLA_CopyToRam();
				   FLA_InitScaleFactor();
				   FLA_InitCallBackPtr();

				   FLA_EraseAndReProgram(); // no return from this, runs in RAM
			   }
		   }

		 bl_doReset();		// if requested by CAN command, execute a watchdog reset

		 bl_jumpToMain();	// if requested by CAN command, branch thru vector to main program
	  }
}

void bl_jumpToMain(void){
	void (*mainProgStartVector)(void);
	Uint16 *mainProgVerificationKey;
	Uint16 key1;
	Uint16 key2;

	if (do_jump_to_main != 0) {
	// Upon receiving CAN 0x2036.4, we set do_jump_to_main to a non-zero
	//   value then we decrement it each time through the main loop and
	//   jump to the main program via a vector after it goes to 0.  Hopefully this
	//   gives us enough time to reply to the PC before we jump.
	   do_jump_to_main--;
	   if (do_jump_to_main == 0) {
			// check key fields at 0x3D8002,3 to see if a main program
			// is installed in Flash
			mainProgVerificationKey = (Uint16*)0x003D8002;
			key1 = *(mainProgVerificationKey++);
			key2 = *mainProgVerificationKey;
			if ((key1 == 0x2001) && (key2 == 0x1776)) {
			   mainProgStartVector = (void(*)(void))0x003D8000; // vector to launch main program
			   mainProgStartVector();
			  }
	      }
	 }
}

void bl_doReset(void){
	if (do_reset != 0) {
	// Upon receiving CAN 0x2036.3, we set do_reset to a non-zero
	//   value then we decrement it each time through the main loop and
	//   RESET the processor after it goes to 0.  Hopefully this
	//   gives us enough time to reply to the PC before we reset.
	   do_reset--;
	   if (do_reset == 0) {
	         *WDKEY = 666;     // Bad Watchdog key value causes a RESET,
	   }                    	//   and vectors to codestart
	}

}

