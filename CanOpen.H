// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     CanOpen.H
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef CANOPENx_H
#define CANOPENx_H

#include "stddef.h"             // defnes NULL
#include "stdbool.h"            // needed for bool data types


// Used in CAN_COMMAND descriptions to indicate what type (or size) of data
// should be used in the reply to a request from the PC.
enum REPLY_DATA_TYPE {
	TYP_BOOLEAN        =  1,
	TYP_INT8           =  2,
	TYP_INT16          =  3,
	TYP_INT32          =  4,
	TYP_UINT8          =  5,
	TYP_UINT16         =  6,
	TYP_UINT32         =  7,
	TYP_FLOAT          =  8,
	TYP_VIS_STRING     =  9,
	TYP_OCT_STRING     = 10,
	TYP_DATE           = 11,
	TYP_TIME_OF_DAY    = 12,
	TYP_TIME_DIFF      = 13,
	TYP_BIT_STRING     = 14,
	TYP_DOMAIN         = 15,
	TYP_NAH            = 64
};

enum CANOPEN_STATUS {
	CANOPEN_NO_ERR             =  0,
	CANOPEN_INDEX_ERR          =  1,
	CANOPEN_SUBINDEX_ERR       =  2,
	CANOPEN_CMND_SPC_ERR       =  3,
	CANOPEN_NULL_FUNC_PTR_ERR  =  4,    //index.subindex table entry has NULL for ptr-to-function
	CANOPEN_TEST_ERR           =  5,    //Used internally, only for unit testing.
	CANOPEN_DATA_TYPE_ERR      =  6,    //we received 1st packet in multi-packet but index.subindex table entry != TYP_OCT_STRING
	CANOPEN_MULTI_SEG_000_ERR  =  7,    //1st packet in multi-packet xfer, requested total byte-count > buffer size
	CANOPEN_MULTI_SEG_001_ERR  =  8,    //2nd, 3rd, etc packet received but multi-segment transfer not in progress
	CANOPEN_MULTI_SEG_002_ERR  =  9,    //wrong toggle bit received in 2nd, 3rd, etc packet of multi-segment transfer
	CANOPEN_MULTI_SEG_003_ERR  =  10,   //# bytes of data in multi-segment packet than in available buffer or expected total message
	CANOPEN_MULTI_SEG_004_ERR  =  11,   //# bytes total in multi-segment transfer != # stated in 1st packet
	CANOPEN_DFX_SEND_001_ERR   =  12,   //DummyFileXfer SEND: unable to write to MultiPacketBuf
	CANOPEN_DFX_SEND_002_ERR   =  13,   //DummyFileXfer SEND: user requests data past end of what is in buffer
	CANOPEN_MCS_FILE_RECV_001_ERR = 14, //error details reported in mcsFileRecvStatus
	CANOPEN_BAD_FLASH_ERASE_TOKEN = 15, //PC sent Flash_Erase_Token that does not match what we sent them
	CANOPEN_BAD_FLASH_STATUS      = 16, //flash status reg indicates not ready for desired operation
	CANOPEN_MCS_SEND_001_ERR   =  17,   //MCS File SEND: unable to write to MultiPacketBuf
	CANOPEN_BAD_EEPROM_TOKEN   =  18,   //PC sent I2CEEProm Programming Token that does not match what we sent them
	CANOPEN_I2CEEPROM_READ_ERR =  19,   //Unsuccessfull cal to i2cee_read()
	CANOPEN_I2CEEPROM_NO_SEL   =  20,   //PC must select an I2CEEPROM before reading or programming
	CANOPEN_I2CEE_WRITE_FAIL   =  21,   //Possibly we are trying to write to the EEProm before end of previous update cycle.
	CANOPEN_MULTI_SEG_005_ERR  =  22,	//PC requests multi-segment transfer but multi-seg buffer is already busy.
	CANOPEN_MULTI_SEG_006_ERR  =  23,	//Problem with 32-byte multi-seg xfer from PC.
	CANOPEN_SCI2_RX_BUF_EMPTY_ERR = 24, // they are asking for Rx data, but Rx buff is empty
	CANOPEN_SCI2_RX_002_ERR	   =  25	// we can't copy to MultiPacketBuf, maybe it is is in use
};

struct MULTI_PACKET_BUF
	{
		Uint16 max_char_in_buf; // initialize this to MAX allowable # of chars in buff
	    Uint16 count_of_bytes_in_buf;
		char buff[(128+7)];	// spec at 128 max, + 7 extra words so overfilling won't crash anything else
	};

Uint16 copyDataToMultiPacketBuf(char* fromPtr, Uint16 count);
Uint16 copyPacked32BytesToMultiPacketBuf(Uint16* fromPtr);
Uint16 copy32BytesFromMultiPacketBufToPackedBuf(Uint16* toPtr);


//Tables to help parse incoming CAN requests.
// CAN_INDEX -- we index directly into this table using the CAN index from the incoming
//    command.  It gives us an address in a CAN_COMMAND structure, and it tells us
//    how many CAN requests are invoked as sub-indices of this CAN index.
// CAN_COMMAND -- for each CAN index and each sub-index we have several pieces of
//    information useful in responding to the command, such as function pointers to
//    call, depending on whether it is a read or a write operation, an indicator of
//    what type/size of data is expected in the response, and a pointer to a data field
//    that may be important to the response.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     CAN_OPEN STRUCTURE DEFINITIONS for Multi-Packet transfers
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Field Definitions for Rx Mailbox in 2nd, 3rd, etc msgs in multi-packet
//  Upload or Download  SDO Segment Protocol.

struct MULTI_PACKET_PROTOCOL {
   struct {
      unsigned int final_packet	: 1;
      unsigned int n_unused_bytes	: 3;
      unsigned int toggle 		: 1;
      unsigned int CmdSpc 		: 3;
      unsigned int data_byte_1	: 8;
   } MboxA;
   struct {
      unsigned int data_byte_2	: 8;
      unsigned int data_byte_3	: 8;
   } MboxB;
   struct {
      unsigned int data_byte_4	: 8;
      unsigned int data_byte_5	: 8;
   } MboxC;
   struct {
      unsigned int data_byte_6	: 8;
      unsigned int data_byte_7	: 8;
   } MboxD;
};



//===========================================================================
// Tables with parameters for each CanOpen request index.subindex.
// Tables to help parse incoming CAN requests.
//===========================================================================

struct CAN_COMMAND              // for each Index and each sub-index
   {
   void  *datapointer;
   enum REPLY_DATA_TYPE replyDataType;
   enum CANOPEN_STATUS (*sendProcess)(struct CAN_COMMAND* can_command, Uint16* data);
   enum CANOPEN_STATUS (*recvProcess)(struct CAN_COMMAND* can_command, Uint16* data);
   };

struct CAN_INDEX                // for each Index and each sub-index
   {
	struct CAN_COMMAND* canCommand;
	Uint16 max_subIndex;
   };


enum CANOPEN_STATUS canO_HandleCanOpenMessage(Uint16 *rcvMsg, Uint16 *xmtMsg);
enum CANOPEN_STATUS canO_multiPktRecvFirst(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg);
enum CANOPEN_STATUS canO_multiPktRecv2nd3rdEtc(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg);
enum CANOPEN_STATUS canO_multiPktSendFirst(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg);
enum CANOPEN_STATUS canO_multiPktSend2nd3rdEtc(struct CAN_COMMAND *can_command, Uint16 *rcvMsg, Uint16 *xmtMsg);
enum CANOPEN_STATUS canO_recv32Bits(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_send32Bits(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_recv16Bits(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_send16Bits(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_givesErrForTesting(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_sendMaxSubIndex(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_doReset(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS canO_doJumpToMain(struct CAN_COMMAND* can_command, Uint16* data);


union CANOPEN16_32 {
	Uint32 		all;
	struct TWO_WORDS {
		Uint16 lsw;
		Uint16 msw;
	} words;
};

struct SDO_MULTI_SEGMENT {
   Uint16  in_progress;  // in progress = 1, not in progress = 0
   char  *buf_ptr;     // start of buffer for Tx or Rx
   Uint16 byte_count_so_far;  // next bytes read or written to/from *(buf_ptr + byte_count_so_far)
   Uint16 expected_total_byte_count;
   Uint16  toggle;  // toggle bit from last received message
   struct CAN_COMMAND*  can_command_ptr; // Pointer to parameters for index.subindex
   enum CANOPEN_STATUS (*process)(struct CAN_COMMAND* can_command, Uint16* data);  // pointer to a procedure
   Uint16  max_buff_size; // from MfgrSpcDeviceMemoryMap
};


#endif
