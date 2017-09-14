// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     LED.C
//
// Turn on/off LED's for DSP (on TB3CMB)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Simple counter to implement timer for DSP CPLS's without timer interrupt
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Uint16 led_simpleTimer0;
Uint16 led_simpleTimer1;
Uint16 led_simpleTimer2;

void led_simpleTimerInit(void){
	led_simpleTimer0 = 0;
	led_simpleTimer1 = 0;
	led_simpleTimer2 = 0;
}

void led_simpleTimer(void){
	// called each time thru the background loop in main()
	// Increment counter variables
	// toggle DSP LED's when we roll over
	if(((led_simpleTimer0++) & 0x0FFF) == 0){
		if(led_simpleTimer1++ == 4){
			led_simpleTimer1 = 0;
			led_simpleTimer2++;	// goal is to increment this every xx sec.
		}
	}
}

void led_simpleHeartbeat(void){
	   // GPIO3B0: 3 off, rightmost LED blinks
	   GpioDataRegs.GPBSET.all  |= 0x0001;
	   GpioDataRegs.GPBSET.all  |= 0x0002;
	   GpioDataRegs.GPBSET.all  |= 0x0004;
	   if ((led_simpleTimer2 & 1) == 1) {
	      GpioDataRegs.GPBCLEAR.all |= 0x0008; // on
	   } else {
		  GpioDataRegs.GPBSET.all |= 0x0008;   // off
	   }
}
void led_leds(Uint16 ledData){
	   // 0x0008 rightmost LED
	   // 0x0001 leftmost LED
	   // set -> led on, clear -> led off
	   GpioDataRegs.GPBSET.all  |= 0x000F; // all off
       GpioDataRegs.GPBCLEAR.all |= ledData; // on
}

void GpioU_defaultInit(void){
    // For starters setup the GPIO as all inputs.

    Uint16 var1;
    Uint16 var2_all_inputs;
    Uint16 var3;

    EALLOW;

    var1= 0x0000;		            // sets GPIO Muxs as I/Os
	GpioMuxRegs.GPAMUX.all=var1;
    GpioMuxRegs.GPBMUX.all=var1;
    GpioMuxRegs.GPDMUX.all=var1;
    GpioMuxRegs.GPFMUX.all=var1;
    GpioMuxRegs.GPEMUX.all=var1;
    GpioMuxRegs.GPGMUX.all=var1;

    var2_all_inputs = 0x0000;                // sets GPIO DIR as all inputs
    GpioMuxRegs.GPADIR.all=var2_all_inputs;  // If GPxDIR.bit = 0, then the pin is configured as an input
    GpioMuxRegs.GPBDIR.all=var2_all_inputs;  // If GPxDIR.bit = 1, then the pin is configured as an output
    GpioMuxRegs.GPDDIR.all=var2_all_inputs;
    GpioMuxRegs.GPEDIR.all=var2_all_inputs;
    GpioMuxRegs.GPFDIR.all=var2_all_inputs;
    GpioMuxRegs.GPGDIR.all=var2_all_inputs;

    var3= 0x0000;		            // sets the Input qualifier values
    GpioMuxRegs.GPAQUAL.all=var3;   // 0x00 No qualification (just SYNC to SYSCLKOUT)
    GpioMuxRegs.GPBQUAL.all=var3;   // eg: no de-bounce
    GpioMuxRegs.GPDQUAL.all=var3;
    GpioMuxRegs.GPEQUAL.all=var3;

    EDIS;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Set all GPIO's On or Off -- in this case, they are all OFF ("clear")
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GpioU_initGpiosAllZero(void)
{
   // Set pins to a known initial state
   // Set all GPIO to 0, "clear"

//   GpioDataRegs.GPASET.all    =0xAAAA;
	 GpioDataRegs.GPACLEAR.all  =0xFFFF;

//	 GpioDataRegs.GPBSET.all    =0x0000;
     GpioDataRegs.GPBCLEAR.all  =0xFFFF;

//   GpioDataRegs.GPDSET.all    =0x0022;
     GpioDataRegs.GPDCLEAR.all  =0x0063;    // Four I/Os only

//   GpioDataRegs.GPESET.all    =0x0002;
     GpioDataRegs.GPECLEAR.all  =0x0007;    // ThreeI/Os only

//   GpioDataRegs.GPFSET.all    =0x0000;
     GpioDataRegs.GPFCLEAR.all  =0xFFFF;

//   GpioDataRegs.GPGSET.all    =0x0020;
     GpioDataRegs.GPGCLEAR.all  =0x0030;    // Two  I/Os only
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  LEDs
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GpioU_DspLedsInit(void){
    // Setup the GPIO to use IO pins B0, B1, B2, & B3 as outputs
    Uint16 var2_LEDs;
    var2_LEDs       = 0x000F;       // 0:3 are outputs for LEDs

    EALLOW;
    GpioMuxRegs.GPBDIR.all |= var2_LEDs;
    EDIS;

}
