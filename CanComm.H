// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     CanComm.H
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef CANCOMMx_H
#define CANCOMMx_H

#include "stdbool.h"            // needed for bool data types

void canC_initComm(void);
Uint16 canC_readCanAddrDipSwitches(void);
void canC_pollRecv(void);
bool canC_RecognizedIndexSubindex(Uint16 index, Uint16 subIndex, Uint16 msgType, Uint16 *rcvMsg, Uint16 *xmtMsg);

void canC_mailboxInitialization(void);
void canC_transmitMessage( int mbxNumber, Uint16 *msg );
int canC_configMbxForTransmit( int mbxNumber, Uint32 messageID, int dlc, int ide );
int canC_configMbxForReceive( int mbxNumber, Uint32 messageID, int dlc, int ide);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     CAN_OPEN STRUCTURE DEFINITIONS for MailboxA, first 16-bit message data word
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct  MBOXA_SDO_EXPEDITED_BITS {     // bits  description
   Uint16 size_indctr    :  1;  // LS bit of LS byte
   Uint16 expedite       :  1;
   Uint16 bytes_no_data  :  2;
   Uint16 fill0          :  1;
   Uint16 CmndSpc        :  3; // MS 3 bits of LS byte
   Uint16 LSBIndex       :  8; // MS byte
};

struct  MBOXA_SDO_NONEXPEDITED_BITS {     // bits  description
   Uint16 final_packet		: 1; // LS bit of LS byte
   Uint16 n_unused_bytes	: 3;
   Uint16 toggle 			: 1;
   Uint16 CmdSpc 			: 3; // MS 3 bits of LS byte
   Uint16 data_byte_1		: 8; // MS byte
};

union CANOPENMBOXA {
   Uint16                all;
   struct MBOXA_SDO_EXPEDITED_BITS  exp_sdo;		//single packet SDO
   struct MBOXA_SDO_NONEXPEDITED_BITS  non_exp_sdo; //multi-packet SDO
};



#endif
