// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     TimeStamp.C
//
//     N O T E : This module is automatically edited to insert date and time
//                information when project is built by CCSv5
//               Automatic edit changes the ts_dateStamp and ts_timeStamp values.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

#include "stdbool.h"            // needed for bool data types
#include <string.h>             // needed for strcpy() function
#include "CanOpen.H"

//#include "Rs232Out.h"
//#include "StrUtil.H"

// Automatic edit changes the version, ts_dateStamp and ts_timeStamp values.
// RS232 diagnostic command "C1003Cr" reads ascii version and timestamp info.
const char *version = "V02.01.13\0";
const char *ts_dateStamp = "09/14/2017\0";
const char *ts_timeStamp = "09:58\0";

// Automatic edit changes this version and timestamp, in compatible format to FPGA timestamps
// we have a CAN command to read timestamp and version information
const Uint16 timeStamp_t1 = 0x1709; // BCD YYMM (0x1504 => 2015 April)
const Uint16 timeStamp_t2 = 0x1409; // BCD DDHr (0x2315 => 23rd day of month, 3 pm)
const Uint16 timeStamp_t3 = 0x58A5; // BCD MnA5 (0x25A5 => 25 min past the hour, A5 is a constant)
const Uint16 revision_rv1 = 0x0201; // 2 binary bytes A.B
const Uint16 revision_rv2 = 0x0D0F; // 2 binary bytes C.0 (version is A.B.C, 0 indicates DSP)

// Display the build date-timestamp on RS232
//void ts_displayBuildDateTimestamp(void)
//{
 //  char msgOut[64];
//  char *ptr;

//   ptr = strU_strcpy(msgOut," TimeStamp: ");
//   ptr = strU_strcpy(ptr,ts_dateStamp);
//   ptr = strU_strcpy(ptr," ");
//   ptr = strU_strcpy(ptr,ts_timeStamp);
//   ptr = strU_strcpy(ptr,"\n\r");
//   ptr = strU_strcpy(ptr," Version  : ");
//   ptr = strU_strcpy(ptr,version);
//   ptr = strU_strcpy(ptr,"\n\r\n\r");
//   r232Out_outChars(msgOut, (Uint16)(ptr - msgOut));
//}

enum CANOPEN_STATUS ts_sendTimeStampClassic(struct CAN_COMMAND* can_command, Uint16* data){
	// Return 32-bit DSP Firmware TimeStamp, formatted as in classic test station
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex

	// MboxC = HHMM in BCD
	// MboxD = 00A5 in BCD


	*(data+2) = ((timeStamp_t2 << 8) & 0xFF00) | ((timeStamp_t3 >> 8) & 0x00FF);  //MboxC
	*(data+3) = (timeStamp_t3 & 0x00FF);   //MboxD
	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS ts_sendDateStampClassic(struct CAN_COMMAND* can_command, Uint16* data){
	// Return 32-bit DSP Firmware DateStamp, formatted as in classic test station
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex

	// MboxC = YYYY in BCD
	// MboxD = MMDD in BCD
	*(data+2) = ((timeStamp_t1 << 8) & 0xFF00) | ((timeStamp_t2 >> 8) & 0x00FF);  //MboxC,
	*(data+3) = 0x2000 | ((timeStamp_t1 >> 8) & 0x00FF); 					//MboxD
	return CANOPEN_NO_ERR;
}
