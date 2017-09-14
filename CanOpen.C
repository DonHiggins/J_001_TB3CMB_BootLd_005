// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     CanOpen.C
//
//   CAN Given that we just received an 8-byte CAN packet
//       interpret it according to CAN Open, and perform some sort of Test Station
//       action as directed by the packet.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "CanOpen.h"
#include "CanComm.h"
#include "MCS.h"
#include "TimeStamp.h"

extern struct MULTI_PACKET_BUF multi_packet_buf;

extern Uint16 bl_main_can_cmd;
extern Uint16 do_reset;
extern Uint16 do_jump_to_main;

Uint16 canTestData16[2];
Uint16 canTestData0x98765432[] = {0x5432,0x9876}; // {LS word, MS word}


//===========================================================================
// Tables with parameters for each CanOpen request index.subindex.
//
//===========================================================================
//Tables to help parse incoming CAN requests.
//
// CAN_INDEX (array of structs) aka: can_index[]
//
//    When we receive a CAN message (SDO) from the PC, canC_pollRecv() calls
//    the canO_HandleCanOpenMessage() routine which uses the CANOpen INDEX value
//    from the message to index into the CAN_INDEX array to get 2 pieces of information:
//        1. the address of a CAN_COMMAND structure (below) which holds more info
//           that we will use to service the CAN message, and
//        2. value of the maximum legal SUBINDEX for this INDEX.  We use this to
//           validate the CANOpen SUBINDEX from the incoming CAN message.
//
//     Program Maintenance on the CAN_INDEX (array of structs):
//        1. We have a #define for MAX_CAN_INDEX, so if you need to add new elements
//           to the CAN_INDEX array, you should increase MAX_CAN_INDEX accordingly.
//        2. To add service for a message with a new CANOpen Index value, you have to
//           create a new CAN_COMMAND value (below) and add a new element to the
//           CAN_INDEX array.
//        3. If there is a CANOpen INDEX already serviced by this program and you want
//           to add service for a new CANOpen SUBINDEX value for that INDEX, you have
//           to add an element to the CAN_COMMAND array for that CANOpen INDEX, and
//           you have to adjust the CAN_INDEX element so it gives the new value for the
//           maximum legal SUBINDEX for this INDEX.
//
//
// CAN_COMMAND -- for each CAN index and each sub-index we have several pieces of
//    information useful in responding to the command, such as function pointers to
//    call, depending on whether it is a read or a write operation, an indicator of
//    what type/size of data is expected in the response, and a pointer to a data field
//    that may be important to the response.
//
// CAN_COMMAND (for each CANOpen INDEX an array of structs) aka: index_2000[], index_2001[], etc.
//    When we receive a CAN message (SDO) the canO_HandleCanOpenMessage() routine uses the
//    CANOpen INDEX value from the message and the CAN_INDEX table (above) to get a pointer to
//    the approporiate CAN_COMMAND array for that message.  Then it uses the CANOpen SUBINDEX
//    from the message to index into the CAN_COMMAND array, and locate parameters that may (or
//    may not) be used to service the CAN message.  The element in the CAN_COMMAND array (a struct)
//    contains function pointers.  The functions are called to perform any service required for
//    the CAN message.  The functions receive all the information in the CAN_COMMAND element, and
//    it is up to each function to use whatever data it needs and ignore the rest.
//
//    *datapointer --
//       In the simplest case, we have CAN messages that cause us to read or write a 16 or 32-bit
//       data value into memory and we read or write at this address.  In some cases the read
//       or write directly address the CPLD or FPGA's on the external bus -- to the DSP it looks
//       just like a RAM access, but the CPLD and FPGA's may be performing complicated functions
//       for us.
//       In some cases datapointer points to an element in an array, and we subtract it from the
//       base of the array to get the array index.
//       In some cases datapointer may not be used at all by the functions servicing the CAN message,
//       but in our haste we put in a dummy value like "canTestData16", rather than a more elegant
//       indicator for an unused value, so there may be opportunities to clean this up a little.
//    replyDataType --
//       As far A I know, we ignore this value unless it is TYP_OCT_STRING, in which case it
//       signifies a "non-expedited SDO" multi-packet data transfer.  This is pretty much
//       dictated by compatibility with the Classic Test Station and the PC software that
//       communicates with it.
//    *sendProcess --
//       The canO_HandleCanOpenMessage() calls this function for messages where the test station
//       "sends" data to satisfy a "read" request from the PC.
//       To facilitate being called from pointers in CAN_COMMAND, all send and recv processes
//       are defined with identical calling parameters and return type.  For example check out
//          enum CANOPEN_STATUS canO_recv16Bits(struct CAN_COMMAND *can_command,Uint16 *data) {};
//    *recvProcess --
//       The canO_HandleCanOpenMessage() calls this function for messages where the test station
//       "receives" data from the PC which sent a "write" request.


struct CAN_COMMAND index_2000[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2001[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2002[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2003[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2004[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2005[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2006[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2007[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2008[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2009[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_200F[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2010[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2011[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2012[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2013[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2014[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2015[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2016[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2017[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2018[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2019[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_201F[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2020[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2021[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2022[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2023[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2024[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2025[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2026[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2027[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2028[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2029[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_202F[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2030[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2031[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

// 2032  Classic compatible date and time stamps
struct CAN_COMMAND index_2032[] =  {	{NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL},
{NULL, TYP_UINT32, &ts_sendDateStampClassic,  NULL        }, //2032.01
{NULL, TYP_UINT32, &ts_sendTimeStampClassic,  NULL        }};//2032.02

struct CAN_COMMAND index_2033[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2034[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2035[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

//  1) This returns a valid ack to any "Initiate SDO Download	(From PC)"
//  2) ACK's "Initiate SDO Upload"
//  3) Handle following CAN requests
//      2036.1 enter Boot Loader Mode
//      2036.2 Boot Loader: Download new code
//      2036.3 Boot Loader: Reset DSP
//      2032.1 Upload Datestamp (to PC)
//      2032.2 Upload Timestamp (to PC)

struct CAN_COMMAND index_2036[] = {	{NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL},                              //2049.00
	// (void  *)data    	          Uint16      send_funct               recv_funct
	//-------------------   ----------- ------------------------  -----------------
	{NULL,        	 		TYP_UINT32, NULL,  					NULL    			    }, //2036.01
	{&multi_packet_buf, TYP_OCT_STRING, NULL,  					mcs_recv                }, //2036.02
	{NULL,                  TYP_UINT32, &canO_send32Bits,       &canO_doReset           }, //2036.03
	{NULL,                  TYP_UINT32, NULL,                   &canO_doJumpToMain      }, //2036.04
	{NULL, 					TYP_UINT32, &mcs_sendMcsStatus,       NULL                  }, //2036.05
	{NULL, 					TYP_UINT32, NULL			,         NULL                  }, //2036.06
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.07
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.08
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.09
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0A
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0B
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0C
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0D
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0E
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.0F
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.10
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.11
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.12
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }, //2036.13
	{NULL, 					TYP_UINT32, &mcs_sendMcsCb  ,         NULL                  }};//2036.14

/* Boot Loader 2036 */
//{NULL                             , /* 0x0003, */ TYP_INT8  ,  &read_record_info    ,0                       },  // 2036.0
//{NULL                             , /* 0x0003, */ TYP_UINT16,  0                    ,&set_goto_NTB_Bootloader},  // 2036.1 enter Boot Loader Mode
//{NULL /* &bootLoadBuffer.buff */  , /* 0x0003, */ TYP_OCT_STRING,  0                ,0                       },  // 2036.2 Boot Loader: Download new code
//{NULL                             , /* 0x0003, */ TYP_UINT16,  0                    ,&resetProcessor         },  // 2036.3 reset & start from 0x0000


struct CAN_COMMAND index_2037[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2038[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2039[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_203F[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2040[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2041[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2042[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2043[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2044[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2045[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2046[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2047[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2048[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

// for Testing Simple CAN operations
struct CAN_COMMAND index_2049[] = {	{NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL},                              //2049.00
	// (void  *)data    	          Uint16      send_funct               recv_funct
	//-------------------   ----------- ------------------------  -----------------
	{canTestData16,         TYP_UINT32, &canO_send32Bits,         &canO_recv32Bits        }, //2049.01
	{canTestData0x98765432, TYP_UINT32, &canO_send32Bits,         NULL                    }, //2049.02
	{canTestData16,         TYP_UINT32, &canO_givesErrForTesting, &canO_givesErrForTesting}};//2049.03

struct CAN_COMMAND index_204A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};                            //204A.00
struct CAN_COMMAND index_204B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_204C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_204D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_204E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

// DSP Firmware Rev & Timestamp
struct CAN_COMMAND index_204F[] = {{NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL},
// (void  *)data    Uint16     send_funct   recv_funct
//---------------  -------   -----------    ----------  -----------------
{(void*)(&timeStamp_t1),  TYP_UINT32, &canO_send16Bits, NULL  }, //204F.1
{(void*)(&timeStamp_t2),  TYP_UINT32, &canO_send16Bits, NULL  }, //204F.2
{(void*)(&timeStamp_t3),  TYP_UINT32, &canO_send16Bits, NULL  }, //204F.3
{(void*)(&revision_rv1),  TYP_UINT32, &canO_send16Bits, NULL  }, //204F.4
{(void*)(&revision_rv2),  TYP_UINT32, &canO_send16Bits, NULL }}; //204F.5

struct CAN_COMMAND index_2050[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2051[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2052[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2053[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2054[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2055[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2056[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

struct CAN_COMMAND index_2057[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2058[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2059[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205A[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205B[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205C[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205D[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205E[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_205F[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};
struct CAN_COMMAND index_2060[] = {NULL,TYP_INT8,&canO_sendMaxSubIndex,NULL};

#define MAX_CAN_COMMAND_INDEX 0x2060;

struct CAN_INDEX can_index[] =
{		{index_2000, 0},
		{index_2001, 0},
		{index_2002, 0},
		{index_2003, 0},
		{index_2004, 0},
		{index_2005, 0},
		{index_2006, 0},
		{index_2007, 0},
		{index_2008, 0},
		{index_2009, 0},
		{index_200A, 0},
		{index_200B, 0},
		{index_200C, 0},
		{index_200D, 0},
		{index_200E, 0},
		{index_200F, 0},
		{index_2010, 0},
		{index_2011, 0},
		{index_2012, 0},
		{index_2013, 0},
		{index_2014, 0},
		{index_2015, 0},
		{index_2016, 0},
		{index_2017, 0},
		{index_2018, 0},
		{index_2019, 0},
		{index_201A, 0},
		{index_201B, 0},
		{index_201C, 0},
		{index_201D, 0},
		{index_201E, 0},
		{index_201F, 0},
		{index_2020, 0},
		{index_2021, 0},
		{index_2022, 0},
		{index_2023, 0},
		{index_2024, 0},
		{index_2025, 0},
		{index_2026, 0},
		{index_2027, 0},
		{index_2028, 0},
		{index_2029, 0},
		{index_202A, 0},
		{index_202B, 0},
		{index_202C, 0},
		{index_202D, 0},
		{index_202E, 0},
		{index_202F, 0},
		{index_2030, 0},
		{index_2031, 0},
		{index_2032, 2},
		{index_2033, 0},
		{index_2034, 0},
		{index_2035, 0},
		{index_2036, 0x14},
		{index_2037, 0},
		{index_2038, 0},
		{index_2039, 0},
		{index_203A, 0},
		{index_203B, 0},
		{index_203C, 0},
		{index_203D, 0},
		{index_203E, 0},
		{index_203F, 0},
		{index_2040, 0},
		{index_2041, 0},
		{index_2042, 0},
		{index_2043, 0},
		{index_2044, 0},
		{index_2045, 0},
		{index_2046, 0},
		{index_2047, 0},
		{index_2048, 0},
		{index_2049, 3},
		{index_204A, 5},
		{index_204B, 1},
		{index_204C, 0},
		{index_204D, 0},
		{index_204E, 0},
		{index_204F, 5},
		{index_2050, 0},
		{index_2051, 0},
		{index_2052, 0},
		{index_2053, 0},
		{index_2054, 0},
		{index_2055, 0},
		{index_2056, 0},
		{index_2057, 0},
		{index_2058, 0},
		{index_2059, 0},
		{index_205A, 0},
		{index_205B, 0},
		{index_205C, 0},
		{index_205D, 0},
		{index_205E, 0},
		{index_2060, 0}
};

#define MIN_CAN_INDEX 0x2000
#define MAX_CAN_INDEX 0x2060

//===========================================================================
// Structure used to manage multi-packet uploads and downloads.
//
//===========================================================================
// used to manage multi-packet SDO up & downloads
struct SDO_MULTI_SEGMENT sdo_multi_segment_control_block = {0,NULL,0,0,0};
#define SDO_MS_CB sdo_multi_segment_control_block

struct MULTI_PACKET_BUF multi_packet_buf = {128,0,0}; //  buffer size = 128, count of chars = 0, 1st char = 0

void init_SDO_MS_CB(void) {
	// set initial values for sdo_multi_segment_control_block
	SDO_MS_CB.in_progress = 0;
	SDO_MS_CB.buf_ptr = NULL;
	SDO_MS_CB.byte_count_so_far = 0;
	SDO_MS_CB.expected_total_byte_count = 0;
}

Uint16 copyDataToMultiPacketBuf(char* fromPtr, Uint16 count){
	//SAFE utility to copy characters from *fromPtr into multi_packet_buf.buf
	// "count" specifies desired # of characters to copy
    // Return value is actual # of chars
	Uint16 actualCount = 0;
	Uint16 i;

	// return 0 if a CAN multi-segment operation is in progress
	if (sdo_multi_segment_control_block.in_progress != 0){
		return actualCount;
	}
	if (count > multi_packet_buf.max_char_in_buf) {
		actualCount = multi_packet_buf.max_char_in_buf;
	} else {
		actualCount = count;
	}

	for (i=0;i<actualCount;i++){
		multi_packet_buf.buff[i] = *(fromPtr++);
	}
	multi_packet_buf.count_of_bytes_in_buf = actualCount;

	return actualCount;
}

Uint16 copyPacked32BytesToMultiPacketBuf(Uint16* fromPtr){
	// Utility to copy 32 bytes of data from *fromPtr into multi_packet_buf
	// Serves i2cee_32BytesToPC() function.
	// *fromPtr" points to 16 words of packed data -- 2-bytes per word.
	// multi_packet_buf is (CCS)  char with 1 byte per 16-byte word.
	// Returns # of bytes copied: 32 is OK, 0 means the multi_packet_buf is not available

	Uint16 i;

	// return 0 if a CAN multi-segment operation is in progress
	if (sdo_multi_segment_control_block.in_progress != 0){
		return 0;
	}

	for (i=0;i<32; ){
		multi_packet_buf.buff[i++] = (*(fromPtr) >> 8) & 0x00FF;
		multi_packet_buf.buff[i++] = *(fromPtr++) & 0x00FF;
	}
	multi_packet_buf.count_of_bytes_in_buf = 32;

    return 32;
}

Uint16 copy32BytesFromMultiPacketBufToPackedBuf(Uint16* toPtr){
	// Utility to copy 32 bytes of data from multi_packet_buf *fromPtr into
	// Serves i2cee_32BytesFromPC() function.
	// *toPtr" points to 16 words of packed data buffer -- 2-bytes per word.
	// multi_packet_buf is (CCS)  char with 1 byte per 16-byte word.
	// Returns # of bytes copied: 32 is OK, 0 indicates some sort of problem.

	Uint16 i;
	Uint16 j;

	// return 0 if we don't have 32 bytes in multi-packet buffer
	if (multi_packet_buf.count_of_bytes_in_buf != 32){
		return 0;
	}

	j = 0;
	for (i=0;i<16; i++){
		*(toPtr) = (multi_packet_buf.buff[j++] << 8) & 0xFF00;
		*(toPtr++) |= (multi_packet_buf.buff[j++] & 0x00FF);
	}
    return 32;
}

//===========================================================================
// Entry point from canC_pollRecv()after detecting receipt of a CAN packet
// Here we start applying CanOpen protocol to it.
//===========================================================================

enum CANOPEN_STATUS canO_HandleCanOpenMessage(Uint16 *rcvMsg, Uint16 *xmtMsg){
	// CanComm just received an 8-byte packet and handed it off to us.
	// Here we interpret it as a CanOpen message and see what the sender is requesting,
	// and do it.
	// If we are successful we return status CANOPEN_NO_ERR, and the xmtMsg buffer
	// is ready to transmit to the sender.
	// Otherwise we return a non-zero error status.

	//Uint16 msgType;
    Uint16 index;
    Uint16 subIndex;
    Uint16 cmndSpc;
    enum REPLY_DATA_TYPE replyDataType;

    struct CAN_COMMAND *can_command;

    enum CANOPEN_STATUS (*functPtr)(struct CAN_COMMAND* can_command, Uint16* data);
    enum CANOPEN_STATUS canOpenStatus;

    union CANOPENMBOXA *mboxaBitsRecv = (union CANOPENMBOXA *)rcvMsg;
    union CANOPENMBOXA *mboxaBitsXmit = (union CANOPENMBOXA *)xmtMsg;

    cmndSpc = mboxaBitsRecv->exp_sdo.CmndSpc;

    if ((SDO_MS_CB.in_progress == 1)) {
    	// Multi-packet operation in progress, see if this is SEND or RECEIVE
    	if (cmndSpc == 0) {          // RECV -- 2nd, 3rd, etc in multi-packet from PC, (download)
    		canOpenStatus = canO_multiPktRecv2nd3rdEtc(SDO_MS_CB.can_command_ptr, rcvMsg, xmtMsg);
		    return canOpenStatus;
    	}else if (cmndSpc == 3) {    // SEND -- 2nd, 3rd, etc in multi-packet to PC, (upload)
    		canOpenStatus = canO_multiPktSend2nd3rdEtc(SDO_MS_CB.can_command_ptr, rcvMsg, xmtMsg);
		    return canOpenStatus;
    	}else {
    		init_SDO_MS_CB(); // assume an earlier multi-segment transmission was interrupted
		    return CANOPEN_CMND_SPC_ERR;
    	}
    }

    index = ((*rcvMsg>>8) & 0x00FF)|((*(rcvMsg+1)<<8) & 0xFF00);
    subIndex = ((*(rcvMsg+1)>>8) & 0x00FF);

    // Check to see if this is a legal index.subindex
    if ((index < MIN_CAN_INDEX) || (index > MAX_CAN_INDEX)) {
    	return CANOPEN_INDEX_ERR;
    }
    if (subIndex > can_index[index - 0x2000].max_subIndex) {
    	return CANOPEN_SUBINDEX_ERR;
    }

    // calculate pointer to CAN_COMMAND struct for index.subindex
    can_command = &((can_index[index - 0x2000].canCommand)[subIndex]);

    // From bit fields in MboxA, see if we SEND or RECV data in this message
    if (cmndSpc == 1) {          // RECV -- single packet from PC, (download)
    	                         //         or start of multi-packet from PC
        // locate function pointer for RECV for this index.subindex;
    	functPtr = (can_index[index - 0x2000].canCommand)[subIndex].recvProcess;
    	if (functPtr == NULL) return CANOPEN_NULL_FUNC_PTR_ERR;

    	// if expedite bit == 0, then this is the first packet in a multi-packet download
    	if (mboxaBitsRecv->exp_sdo.expedite == 0) {
    		canOpenStatus = canO_multiPktRecvFirst(can_command, rcvMsg, xmtMsg);
    		return canOpenStatus;
    	}

        // call the RECV function for this index.subindex
    	canOpenStatus = functPtr(can_command,rcvMsg);
        if (canOpenStatus != CANOPEN_NO_ERR) return canOpenStatus;

    	// mboxaBitsXmit->all = (mboxaBitsXmit->all & 0xFF00) | 0x60;

    	mboxaBitsXmit->exp_sdo.CmndSpc = 3;
    	mboxaBitsXmit->exp_sdo.fill0 = 0;
    	mboxaBitsXmit->exp_sdo.bytes_no_data = 0;
    	mboxaBitsXmit->exp_sdo.expedite = 0;
    	mboxaBitsXmit->exp_sdo.size_indctr = 0;


    } else if (cmndSpc == 2) {    // SEND -- single packet with data to PC {upload}
        // locate function pointer for RECV for this index.subindex;
    	functPtr = (can_index[index - 0x2000].canCommand)[subIndex].sendProcess;
    	if (functPtr == NULL) return CANOPEN_NULL_FUNC_PTR_ERR;
        // call the SEND function for this index.subindex
    	canOpenStatus = functPtr(can_command,xmtMsg);
        if (canOpenStatus != CANOPEN_NO_ERR) return canOpenStatus;

    	// As a COMPATIBILITY issue, with DSP2407 CAN_Main.c, we do not look
    	// at the expedite bit when cmndSpc == 2, instead we look at the
    	// replyDataType in the CAN_COMMAND struct, and we treat the packet as
    	// first packet in a non-expedited exchange
    	// if replyDataType is TYP_OCT_STRING,
    	// otherwise we treat it as an expedited packet
    	replyDataType = (can_index[index - 0x2000].canCommand)[subIndex].replyDataType;
    	if (replyDataType == TYP_OCT_STRING) {
    		canOpenStatus = canO_multiPktSendFirst(can_command, rcvMsg, xmtMsg);
    		return canOpenStatus;
    	}

        if (canOpenStatus != CANOPEN_NO_ERR) return canOpenStatus;

    	//mboxaBitsXmit->all = (mboxaBitsXmit->all & 0xFF00) | 0x43;

    	mboxaBitsXmit->exp_sdo.CmndSpc = 2;
    	mboxaBitsXmit->exp_sdo.fill0 = 0;
    	mboxaBitsXmit->exp_sdo.bytes_no_data = 0;
    	mboxaBitsXmit->exp_sdo.expedite = 1;
    	mboxaBitsXmit->exp_sdo.size_indctr = 1;

    } else {                     // we don't recognize this (yet)
    	return CANOPEN_CMND_SPC_ERR;
    }

    return CANOPEN_NO_ERR;
}
//===========================================================================
// These functions handle first and subsequent packets in multi-packet
// uploads and downloads.
//===========================================================================

// A little research from DSP2407 CAN_Main.c . . .
//
// Incoming packet has CmdSpc = 1 (eg. 0x20, since CmdSpc is 3 MS bits)
//     this is a request for us to Recv data from the host
//     if expedite bit = 0, then this is the first packet in a Multi-Packet receive
//     2nd, 3rd, etc packets come with CmdSpc = 0
//
// Incoming packet has CmdSpc = 2 (eg. 0x40, since CmdSpc is 3 MS bits)
//     this is a request for us to Send data back to the host
//     if data Type in CAN_COMMAND struct is TYPE_OCT_STR then this is
//        the first packet in a Multi-Packet send)
//     2nd, 3rd, etc packets come with CmdSpc = 3 (0x60)
//     NOTE: DSP2407 CAN_Main.c, does not look at the expedite bit in incoming
//     packets, and treats them as expedited (single packet) unless the
//     replyDataType in CAN_COMMAND struct is TYPE_OCT_STRING.  We must be compatible.
//
// Also be aware that CAN_Main multi-packet code may have been written using "upload" and "download"
// opposite of how those terms are used in CANopen documentation.  As you read it, just check to
// see if bytes are going from the packet into the buffer, or the other way round. -- DH

enum CANOPEN_STATUS canO_multiPktRecvFirst(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg){
	// Call here when we receive the first packet in a multi-packet upload (receive)

    union CANOPENMBOXA *mboxaBitsRecv = (union CANOPENMBOXA *)rcvMsg;
    union CANOPENMBOXA *mboxaBitsXmit = (union CANOPENMBOXA *)xmtMsg;
	struct MULTI_PACKET_BUF* mpb;

    // Type should be TYP_OCT_STRING
	if (can_command->replyDataType != TYP_OCT_STRING) {
		return CANOPEN_DATA_TYPE_ERR;
	}

	mpb = (struct MULTI_PACKET_BUF*)(can_command->datapointer);
	SDO_MS_CB.buf_ptr = (mpb->buff);
	SDO_MS_CB.max_buff_size = mpb->max_char_in_buf;
	mpb->count_of_bytes_in_buf = 0;

	// See if the sender provided a byte count
	// If sender did provide a byte count, then it is in MboxC, *(rcvMsg+2)
	SDO_MS_CB.expected_total_byte_count = *(rcvMsg+2);
	// note: according to CAN Open protocol, the byte count can go up to 2^^32, but
	//  we don't handle anything that large, so we just look at the 2 ls bytes.
	// Then, if they leave the S-bit 0, and don't specify a byte count we
	//  record it as 0 here and take the byte-count from the # actually sent.
	if (mboxaBitsRecv->exp_sdo.size_indctr == 0) {
	      SDO_MS_CB.expected_total_byte_count = 0;
	}
	if (  (SDO_MS_CB.max_buff_size > 1) // 1 is a special flag value for testing
	   && (SDO_MS_CB.expected_total_byte_count > SDO_MS_CB.max_buff_size) )
	{
	      return CANOPEN_MULTI_SEG_000_ERR;
	}
	SDO_MS_CB.in_progress = 1;
	SDO_MS_CB.can_command_ptr = can_command;
	SDO_MS_CB.process = can_command->recvProcess;;
	SDO_MS_CB.byte_count_so_far = 0;
	SDO_MS_CB.toggle = 1;  // next message we expect toggle bit to be 0

	mboxaBitsXmit->exp_sdo.CmndSpc = 3;
	mboxaBitsXmit->exp_sdo.fill0 = 0;
	mboxaBitsXmit->exp_sdo.bytes_no_data = 0;
	mboxaBitsXmit->exp_sdo.expedite = 0;
	mboxaBitsXmit->exp_sdo.size_indctr = 0;

    return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_multiPktRecv2nd3rdEtc(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg){
	// Call here when we receive the 2nd, 3rd, etc. packet in a multi-packet upload (receive)
	// Design Decision: Here we use a char type receive buffer -- 1 byte of data per 16-bit memory word.
	//                  We assume the data format for our file transfers will be some sort of readable ascii.
	//                  Later when we need to work with the file contents as 16-bit binary, we will
	//                  do that conversion separately.
	//                  This will simplify debugging and maintenance.
	Uint16 i; // # bytes remaining in message
	Uint16 j; // # bytes in this packet
	char *c;  // used to point into data buffer
	Uint16 *ptr;
	struct MULTI_PACKET_BUF* mpb;

    union CANOPENMBOXA *mboxaBitsRecv = (union CANOPENMBOXA *)rcvMsg;
    union CANOPENMBOXA *mboxaBitsXmit = (union CANOPENMBOXA *)xmtMsg;
    struct MULTI_PACKET_PROTOCOL *mpPacketRecv = (struct MULTI_PACKET_PROTOCOL *)rcvMsg;
    struct MULTI_PACKET_PROTOCOL *mpPacketXmit = (struct MULTI_PACKET_PROTOCOL *)xmtMsg;

	mpb = (struct MULTI_PACKET_BUF*)(can_command->datapointer);

	if (SDO_MS_CB.in_progress == 0) return CANOPEN_MULTI_SEG_001_ERR;

	if(mboxaBitsRecv->non_exp_sdo.toggle == SDO_MS_CB.toggle) {
	   // toggle bit should toggle in each sequential message, and
	   // should differ from copy of previous msg toggle bit saved in SDO_MS_CB
	   SDO_MS_CB.in_progress = 0; // shut down the multi-segment transfer in process
	   return CANOPEN_MULTI_SEG_002_ERR;
	} else {
	   SDO_MS_CB.toggle = mboxaBitsRecv->non_exp_sdo.toggle;  // save toggle bit from current message
	}

	// set a value for local variable, i, "# bytes remaining in message"
	if (SDO_MS_CB.expected_total_byte_count == 0) {
	   // 1st message didn't include a byte count, so we check against
	   // max buffer size rather than expected total byte count
	   i = SDO_MS_CB.max_buff_size -  SDO_MS_CB.byte_count_so_far;
	   if (SDO_MS_CB.max_buff_size == 1) {  // 1 is a special flag value for testing
	       i = 8; // big enough so test can continue
	   }
	} else {
	   // 1st message did include a byte count, so we check against that
	   i = SDO_MS_CB.expected_total_byte_count -  SDO_MS_CB.byte_count_so_far;
	}

	// set a value for local variable, 6, "# bytes of data in this packet"
	j = 7 - mboxaBitsRecv->non_exp_sdo.n_unused_bytes;

    // insure # bytes of data in this packet is <= # bytes remaining in the total message
	if (i < j) {
	   // more data bytes in packet than bytes remaining in the message
		  SDO_MS_CB.in_progress = 0; // shut down the multi-segment transfer in process
	      return CANOPEN_MULTI_SEG_003_ERR;
	   }

	// go ahead and append all 7 data bytes   from packet to
	//    data buffer.  Note: buffers are declared with 7 extra words
	//  to avoid problems in the case of overfill
	c =  SDO_MS_CB.buf_ptr + SDO_MS_CB.byte_count_so_far;
	if (SDO_MS_CB.max_buff_size == 1) {  // 1 is a special flag value for testing
	   c = SDO_MS_CB.buf_ptr; // continuously write over first 7 bytes in buffer
	}
	*(c++) = mpPacketRecv->MboxA.data_byte_1;
	*(c++) = mpPacketRecv->MboxB.data_byte_2;
	*(c++) = mpPacketRecv->MboxB.data_byte_3;
	*(c++) = mpPacketRecv->MboxC.data_byte_4;
	*(c++) = mpPacketRecv->MboxC.data_byte_5;
	*(c++) = mpPacketRecv->MboxD.data_byte_6;
	*(c++) = mpPacketRecv->MboxD.data_byte_7;
	SDO_MS_CB.byte_count_so_far += j;
	mpb->count_of_bytes_in_buf += j;


   if (mboxaBitsRecv->non_exp_sdo.final_packet) {
      SDO_MS_CB.in_progress = 0;
      // our internal convention is that the word preceeding the buffer
      // receives the count of bytes downloaded, see struct MULTI_PACKET_BUF
      *(SDO_MS_CB.buf_ptr - 1) = SDO_MS_CB.byte_count_so_far;

      if (SDO_MS_CB.expected_total_byte_count != 0) {
         // 1st message did include a byte count, so we check it against what we got
         if (SDO_MS_CB.expected_total_byte_count != SDO_MS_CB.byte_count_so_far) {
   	        return CANOPEN_MULTI_SEG_004_ERR;
	     }
      }

      if (SDO_MS_CB.process != NULL) {
         // optional procedure to call at end of download
    	 ptr = (Uint16*)SDO_MS_CB.buf_ptr;
    	 // calling params: CAN_COMMAND struct for index.subindex, Uint16* pointer to received byte stream
         SDO_MS_CB.process(SDO_MS_CB.can_command_ptr,ptr);
      }
   }

   mboxaBitsXmit->all = 0;
   mboxaBitsXmit->non_exp_sdo.CmdSpc = 1;
   mboxaBitsXmit->non_exp_sdo.toggle = mboxaBitsRecv->non_exp_sdo.toggle;

   mpPacketXmit->MboxA.data_byte_1 = 0;
   mpPacketXmit->MboxB.data_byte_2 = 0;
   mpPacketXmit->MboxB.data_byte_3 = 0;
   mpPacketXmit->MboxC.data_byte_4 = 0;
   mpPacketXmit->MboxC.data_byte_5 = 0;
   mpPacketXmit->MboxD.data_byte_6 = 0;
   mpPacketXmit->MboxD.data_byte_7 = 0;

   return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_multiPktSendFirst(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg){
	// Call here when we receive the first packet in a multi-packet download (send)

    union CANOPENMBOXA *mboxaBitsXmit = (union CANOPENMBOXA *)xmtMsg;
    struct MULTI_PACKET_BUF *mpb;

    // Type should be TYP_OCT_STRING
	if (can_command->replyDataType != TYP_OCT_STRING) {
		return CANOPEN_DATA_TYPE_ERR;
	}

	mpb = (struct MULTI_PACKET_BUF *)(can_command->datapointer);
	SDO_MS_CB.buf_ptr = (mpb->buff);
	SDO_MS_CB.expected_total_byte_count = mpb->count_of_bytes_in_buf;

	SDO_MS_CB.in_progress = 1;
	SDO_MS_CB.can_command_ptr = can_command;
	SDO_MS_CB.process = can_command->sendProcess;;
	SDO_MS_CB.byte_count_so_far = 0;
	SDO_MS_CB.toggle = 1;  // next message we expect toggle bit to be 0

	mboxaBitsXmit->exp_sdo.CmndSpc = 2;
	mboxaBitsXmit->exp_sdo.fill0 = 0;
	mboxaBitsXmit->exp_sdo.bytes_no_data = 0;
	mboxaBitsXmit->exp_sdo.expedite = 0;
	mboxaBitsXmit->exp_sdo.size_indctr = 0;

    return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_multiPktSend2nd3rdEtc(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg){
	// Call here when we receive rewuests for the 2nd, 3rd, etc. packet in a multi-packet download (send)
	// Design Decision: Here we use a char type receive buffer -- 1 byte of data per 16-bit memory word.
	//                  We assume the data format for our file transfers will be some sort of readable ascii.
	//                  Later when we need to work with the file contents as 16-bit binary, we will
	//                  do that conversion separately.
	//                  This will simplify debugging and maintenance.
	Uint16 i; // # bytes remaining in message
	Uint16 j; // # bytes in this packet
	char *c;  // used to point into data buffer

    union CANOPENMBOXA *mboxaBitsRecv = (union CANOPENMBOXA *)rcvMsg;
    union CANOPENMBOXA *mboxaBitsXmit = (union CANOPENMBOXA *)xmtMsg;
    struct MULTI_PACKET_PROTOCOL *mpPacketXmit = (struct MULTI_PACKET_PROTOCOL *)xmtMsg;
    struct MULTI_PACKET_BUF *mpb;

	if (SDO_MS_CB.in_progress == 0) return CANOPEN_MULTI_SEG_001_ERR;

	if(mboxaBitsRecv->non_exp_sdo.toggle == SDO_MS_CB.toggle) {
	   // toggle bit should toggle in each sequential message, and
	   // should differ from copy of previous msg toggle bit saved in SDO_MS_CB
	   SDO_MS_CB.in_progress = 0; // shut down the multi-segment transfer in process
	   return CANOPEN_MULTI_SEG_002_ERR;
	} else {
	   SDO_MS_CB.toggle = mboxaBitsRecv->non_exp_sdo.toggle;  // save toggle bit from current message
	}

	// i = bytes remaining in message; j =	bytes in this packet
	i = SDO_MS_CB.expected_total_byte_count -  SDO_MS_CB.byte_count_so_far; // # bytes remaining in message
	j = i;
	if (i > 7)
	{
	   j = 7; // # bytes in this packet
	}

	mboxaBitsXmit->all = 0; // all 16 bits of MboxA, including data_byte_1 & CmdSpc, Toggle, etc

	mpb = (struct MULTI_PACKET_BUF *)(can_command->datapointer); //
	c = (mpb->buff) + SDO_MS_CB.byte_count_so_far; // chr* pointer into multi-packet buf
	// go ahead and append all 7 next data bytes from data buf
	// to packet.  Note: garbage bytes are no problem at end of last packet
	mpPacketXmit->MboxA.data_byte_1 = *(c++);
	mpPacketXmit->MboxB.data_byte_2 = *(c++);
	mpPacketXmit->MboxB.data_byte_3 = *(c++);
	mpPacketXmit->MboxC.data_byte_4 = *(c++);
	mpPacketXmit->MboxC.data_byte_5 = *(c++);
	mpPacketXmit->MboxD.data_byte_6 = *(c++);
	mpPacketXmit->MboxD.data_byte_7 = *(c++);
	SDO_MS_CB.byte_count_so_far += j;

	mboxaBitsXmit->non_exp_sdo.CmdSpc = 3;
	mboxaBitsXmit->non_exp_sdo.toggle = mboxaBitsRecv->non_exp_sdo.toggle;

	if (SDO_MS_CB.byte_count_so_far >= SDO_MS_CB.expected_total_byte_count)
	{
	   // final packet
	   mboxaBitsXmit->non_exp_sdo.final_packet = 1;
	   mboxaBitsXmit->non_exp_sdo.n_unused_bytes = 7 - j;
	   SDO_MS_CB.in_progress = 0;
//
//  - - - We actually call the process at the beginning of the multi-packet SEND - - -
//	   if (SDO_MS_CB.process != NULL)
//	   {
//	      // optional procedure to call at end of download
//	      ptr = (Uint16*)SDO_MS_CB.buf_ptr;
//	      // calling params: CAN_COMMAND struct for index.subindex, Uint16* pointer to received byte stream
//	      SDO_MS_CB.process(SDO_MS_CB.can_command_ptr,ptr);
//	   }
	}

   return CANOPEN_NO_ERR;
}


//===========================================================================
// Send and Recv Functions, activated via function pointers in CAN_COMMAND table,
// acting on data for specific CanOpen index.subindex.
//===========================================================================

enum CANOPEN_STATUS canO_recv32Bits(struct CAN_COMMAND *can_command,Uint16 *data) {
	// We received data from host, store in memory at dest = (Uint16*)can_command->datapointer
	// *data is MboxA of received Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 *dest;
	dest = (Uint16*)can_command->datapointer;
	*dest++ = *(data+2); // MboxC
	*dest = *(data+3);   // MboxD
	return CANOPEN_NO_ERR;
}
enum CANOPEN_STATUS canO_send32Bits(struct CAN_COMMAND* can_command, Uint16* data){
	// Host asks us to send data from memory location src = (Uint16*)can_command->datapointer
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 *src;
	src = (Uint16*)can_command->datapointer;
	*(data+2) = *src++; //MboxC
	*(data+3) = *src;   //MboxD
	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_recv16Bits(struct CAN_COMMAND *can_command,Uint16 *data) {
	// We received data from host, store in memory at dest = (Uint16*)can_command->datapointer
	// *data is MboxA of received Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 *dest;
	dest = (Uint16*)can_command->datapointer;
	*dest = *(data+2); // MboxC
	// MboxD ignored
	return CANOPEN_NO_ERR;
}
enum CANOPEN_STATUS canO_send16Bits(struct CAN_COMMAND* can_command, Uint16* data){
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 *src;
	src = (Uint16*)can_command->datapointer;
	*(data+2) = *src; //MboxC
	*(data+3) = 0;   //MboxD
	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_givesErrForTesting(struct CAN_COMMAND* can_command, Uint16* data){
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
    // This function always returns an error message -- for testing the handling of error messages
	return CANOPEN_TEST_ERR;
}

enum CANOPEN_STATUS canO_sendMaxSubIndex(struct CAN_COMMAND* can_command, Uint16* data){
	// response to subindex 0x00 for each index, returns # of legal subindeces.
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 canIndex;
	Uint16 max_subIndex;

	// Start by pulling the CAN index value out of the CAN Mailbox

	//  canIndex = ((*(data+1)) << 8) & 0xFF00; // this gives you the MS 16 bits of the CAN index
	canIndex = (((*(data+0)) >> 8) & 0x00FF);  // but we only use the LS 16 bits

     // read max_subIndex value from table
	max_subIndex = can_index[(canIndex & 0xFF)].max_subIndex;

	*(data+2) = max_subIndex; //MboxC
	*(data+3) = 0;   //MboxD
	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_doReset(struct CAN_COMMAND* can_command, Uint16* data){
	// Request to do a watchdog reset.  Here we store a non-zero value into
	// the variable "do_reset." It gets counted down each time thru the main loop
	// and when it hits 0, we execure a watchdog reset.
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex

	do_reset = 100;

	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS canO_doJumpToMain(struct CAN_COMMAND* can_command, Uint16* data){
	// Request to jump to main program via vector.  Here we store a non-zero value into
	// the variable "do_jump_to_main." It gets counted down each time thru the main loop
	// and when it hits 0, we go ahead and do the jump.
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex

	do_jump_to_main = 100;

	return CANOPEN_NO_ERR;
}
