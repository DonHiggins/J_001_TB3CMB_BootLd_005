// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     CanComm.C
//
//   CAN Ccommunications
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "CanComm.h"
#include "CanOpen.h"
#include "stdbool.h"            // needed for bool data types

// from access32.asm
extern Uint32 read32( Uint32 * );
extern void write32( Uint32 *, Uint32 );
extern void setbits32( Uint32 *, Uint32 );
extern void clearbits32( Uint32 *, Uint32 );

/* These 4 lines have been added to support the C callable assembly WorkAround.asm */
/* This is to support the workaround for errata (as explained in SPRZ193) */
extern void CanDoubleRead(Uint32 *StorePtr, volatile Uint32 *RegPtr);
extern void CanDoubleWrite(volatile Uint32 *RegPtr, Uint32 *LoadPtr);
extern void CanDoubleClear(volatile Uint32 *RegPtr, Uint32 *LoadPtr);
extern void CanDoubleSet  (volatile Uint32 *RegPtr, Uint32 *LoadPtr);
/* Note:
As indicated in the 281x device errata (SPRZ193), double-reads/double writes should be
employed for accesses to the mailbox RAM & LAM/MOTO/MOTS areas. Note, however, that the
errata is applicable only if the mailbox RAM & LAM/MOTO/MOTS areas are accessed at the
same time. When eCAN is communicating with another node in non self-test mode,
the workaround must be incorporated.
*/


Uint16 canTestData16_01; // used in testing SDO handling 2099.1,3
Uint16 canTestData16_02;

// used to manage multi-packet SDO up & downloads
//struct SDO_MULTI_SEGMENT {
//   WORD  in_progress;  // in progress = 1, not in progress = 0
//   char  *buf_ptr;     // start of buffer for Tx or Rx
//   SWORD byte_count_so_far;  // next bytes read or written to/from *(buf_ptr + byte_count_so_far)
//   SWORD expected_total_byte_count;
//   WORD  toggle;  // toggle bit from last received message
//   WORD  element_location; // index into MfgrSpcDeviceMemoryMap
//   void  (*process)(WORD id, WORD data);  // pointer to a procedure
//   WORD  max_buff_size; // from MfgrSpcDeviceMemoryMap
//} sdo_multi_segment_control_block = {0,NULL,0,0,0};
//
//#define SDO_MS_CB sdo_multi_segment_control_block

void canC_initComm(void) {
	Uint16 canAddrDipSwitches;
	Uint32 txMsgID;
	Uint32 rxMsgID;

	// At startup, after calling InitECan() which does a default initialization
	// of CAN configuration registers, then we call here to continue register
	// initialization and also to set up any data structures for our CAN
	// communications application.

	InitECan(); // default init of CAN configuration registers, as per DSP281x_ECan.c
	canC_mailboxInitialization();

	// Read dip switch values to set our network address / COB_ID
	canAddrDipSwitches = canC_readCanAddrDipSwitches();
	txMsgID = ((Uint32)(canAddrDipSwitches + 0x580)) << 18;
	rxMsgID = ((Uint32)(canAddrDipSwitches + 0x600)) << 18;

	// mailbox#, MsgID=CobID<<18, MsgByteLength, 0=11-bitID_not_29-bitID
	//	canC_configMbxForTransmit(1, (0x581L << 18), 8, 0);
	//	canC_configMbxForReceive(31, (0x601L << 18), 8, 0);
	//
	// * * * Need to include node #, from DIP switches, in COB ID (encoded in messageID) above
	canC_configMbxForTransmit(1, txMsgID, 8, 0);
	canC_configMbxForReceive(31, rxMsgID, 8, 0);


    // Enable Mailboxes */
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    // ECanaRegs.CANME.all = 0xFFFFFFFF; // enable all mailboxes
    ECanaRegs.CANME.all = 0x80000002;    // enable mailboxes 1 & 31

}
Uint16 canC_readCanAddrDipSwitches(void){
// CAN_Addr_0,1,2,3 = GBIOB4,5,6,7
// but . . . CAN_Addr_0 is the left-most of the 4 Dip switches, so . . .
//		we bit-reverse the result here so that the right-most switch is
//		reported in the LSB of our result.
// also . . . invert 4 result bits so that 0 is up, 1 is down
//      this is compatible with can addr switches on classic test station.
	Uint16 dipSwitch;
	Uint16 rawInput;
	//rawInput = (GpioDataRegs.GPBDAT.all >> 4) & 0xF;
	rawInput = GpioDataRegs.GPBDAT.all;
	dipSwitch = (((rawInput >> 1) & 0x08) // reverse bit order
			   | ((rawInput >> 3) & 0x04)
		       | ((rawInput >> 5) & 0x02)
		       | ((rawInput >> 7) & 0x01));
	return (dipSwitch ^ 0x0F);           // invert bit values
}

void canC_mailboxInitialization(void) {
// from Aimco6

	unsigned short                i;
	volatile far struct MBOX          *wptr;
	volatile far union  CANLAM_REG    *lp;

#if 0
	/* Initialize all bits of 'Master Control Field' to zero */
	// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
	// all bits (including reserved bits) of MSGCTRL must be initialized to zero
	wptr = &ECanaMboxes.MBOX0;
	for( i = 0; i < 32; i++, wptr++ )
		wptr->MSGCTRL.all = 0x00000000L;

	ECanaRegs.CANME.all = 0L;     // Disable all Mailboxes. Required before writing the MSGIDs
#endif

	// Message identifier register: IDE|AME|AAM|ID[28:18]|ID[17:16]|ID[15:0]
	// IDE=0 means standard identifier, IDE=1 means extended identifier
	// AME=1 means acceptance mask enable, AME=0 acc mask disabled, so all bits in the
	//    message must match these ID bits (if AAM==1 then AME must set to 0)
	// AAM=1 means auto answer mode enabled (TX only)
	// - Standard ID resides in the ID[28:18] region
	// - Mailboxs can be written to 16-bits or 32-bits at a time
	// - Write to the MSGID field of TRANSMIT mailboxes MBOX0 - 15

	// Tx mailboxes MSGID <-- 0
	wptr = &ECanaMboxes.MBOX0;
	for( i = 0; i < 16; i++, wptr++ )
		wptr->MSGID.all = 0L;

	// Rx Mailboxes
	// Write to the MSGID field of RECEIVE mailboxes MBOX16 - 31
	// Set AME, acceptance mask is in use
	wptr = &ECanaMboxes.MBOX16;
	for( i = 0; i < 16; i++, wptr++ )
		wptr->MSGID.all = 0x40000000L;

	// Rx Mailboxes
	// Write to the local mask registers (RX only)
	// Receive all messages (all bit don't care)
	lp = &ECanaLAMRegs.LAM16;
	for( i = 0; i < 16; i++, lp++ )
		lp->all = (0x9FFFFFFFL ^ (0x7FFL<<18)); // 0x8003FFFFL, (require match on MsgID / COBID)

	// Tx mailboxes
	// Specify that 8 bytes will be sent
	wptr = &ECanaMboxes.MBOX0;
	for( i = 0; i < 16; i++, wptr++ )
		wptr->MSGCTRL.bit.DLC = 8;

	// Tx Mailboxes
	// No remote frame is requested
	// Since RTR bit is undefined upon reset,it must be initialized to the proper value
	wptr = &ECanaMboxes.MBOX0;
	for( i = 0; i < 16; i++, wptr++ )
		wptr->MSGCTRL.bit.RTR = 0;

	// Tx Mailboxes
	// Set the Transmit priority level of the transmit mailboxes
	// The higher number means higher priority
	wptr = &ECanaMboxes.MBOX0;
	for( i = 0; i < 16; i++, wptr++ )
		wptr->MSGCTRL.bit.TPL = i;

	// Configure Mailboxes 0-15 as Tx, 16-31 as Rx
	ECanaRegs.CANMD.all = 0xFFFF0000L;

	// Mailboxes are left disabled
    // Caller Enables all Mailboxes after we return
}

int canC_configMbxForTransmit( int mbxNumber, Uint32 messageID, int dlc, int ide ){

	volatile struct MBOX *mp; // "far" declaration caused errors calling canDoublexxx()
	Uint32   bitMask = ( 1L << mbxNumber );
	Uint32   msgIDVal;
	Uint32   msgCtrl;

	if( mbxNumber >= 16 )
		return 1;

	setbits32( (Uint32 *) &ECanaRegs.CANAA.all, bitMask ); // Abort-Acknowledge Register (1 bit per mailbox)
	setbits32( (Uint32 *) &ECanaRegs.CANTA.all, bitMask ); // Transmission-Acknowledge Register (1 bit per mailbox)
	write32( (Uint32 *) &ECanaRegs.CANTRR.all, bitMask );    // Clear CANTRS bit to disable the mailbox (write TRR bit and wait for TRS to clear)
	while( (read32( (Uint32 *) &ECanaRegs.CANTRS.all ) & bitMask) != 0 ) // Transmission-Request Set Register (1 bit per mailbox)
   {
		//EALLOW;
		//serviceDog();
		// EDIS;
   }

	CanDoubleClear(&ECanaRegs.CANME.all, &bitMask); // Mailbox-Enable Register (1 bit per mailbox)

	mp = &((&ECanaMboxes.MBOX0)[mbxNumber]); // Point mp at the appropraite MBX
	if (ide != 0) {
		msgIDVal = ((Uint32) messageID)
				    | 0x40000000L   // AME= 1 acceptance mask enable
				    | 0x80000000L ; // Set IDE = 1, 29-Bit header
		CanDoubleWrite(&mp->MSGID.all, &msgIDVal);
	} else {
		msgIDVal = (Uint32) messageID
	    			| 0x40000000L   // AME= 1 acceptance mask enable
	    			| 0x00000000L ; // Set IDE = 0, 11-bit header, AME= 1 acceptance mask enable
		CanDoubleWrite(&mp->MSGID.all, &msgIDVal);
	}
	msgCtrl = ((Uint32) mbxNumber << 8) | ((Uint32) dlc);
	CanDoubleWrite(&mp->MSGCTRL.all, &msgCtrl);

	clearbits32( (Uint32 *) &ECanaRegs.CANMD.all, bitMask );
	setbits32( (Uint32 *) &ECanaRegs.CANME.all, bitMask );

	return 0;
}

int canC_configMbxForReceive( int mbxNumber, Uint32 messageID, int dlc, int ide) {
// from Aimco6

	volatile struct MBOX *mp;
	Uint32 * dwptr;
	Uint32 bitMask = ( 1L << mbxNumber );
	Uint32 lamMask;
	Uint32 msgIDVal;
	Uint32 msgCtrl;

	if( mbxNumber < 16 )
		return 1;

	setbits32( (Uint32 *) &ECanaRegs.CANAA.all, bitMask );	//Abort-Acknowledge Register
	setbits32( (Uint32 *) &ECanaRegs.CANTA.all, bitMask );	//Transmission-Acknowledge Register
	write32( (Uint32 *) &ECanaRegs.CANTRR.all, bitMask );	// Clear CANTRS bit to disable the mailbox
															//(write TRR bit and wait for TRS to clear)
	while( (read32( (Uint32 *) &ECanaRegs.CANTRS.all ) & bitMask) != 0 ) {
		//EALLOW;
		//serviceDog();
		//EDIS;
   }

	CanDoubleClear(&ECanaRegs.CANME.all, &bitMask);	// Mailbox Enable Register, 1=enable, 0=disable

	// In default initialization, we set LAM mask registers to accept all messages.
	// Here we change that so that our receive mailbox selects based on COB_ID.
	lamMask = (0x9FFFFFFFL ^ (0x7FFL<<18)); // 0x8003FFFFL, (require match on MsgID / COBID)
	// lamMask = 0x9FFFFFFFL; // Accept anything
	dwptr  = (Uint32 *)&ECanaLAMRegs.LAM0;
	dwptr += mbxNumber;
	CanDoubleWrite(dwptr,&lamMask);

	mp = &((&ECanaMboxes.MBOX0)[mbxNumber]); // Point mp at the appropraite MBX
	if (ide != 0){
		msgIDVal = ((Uint32) messageID)
				    | 0x40000000L   // AME= 1 acceptance mask enable
				    | 0x80000000L ; // Set IDE = 1, 29-Bit header
		CanDoubleWrite(&mp->MSGID.all, &msgIDVal);
	} else {
		msgIDVal = (Uint32) messageID
	    			| 0x40000000L   // AME= 1 acceptance mask enable
	    			| 0x00000000L ; // Set IDE = 0, 11-bit header, AME= 1 acceptance mask enable
		CanDoubleWrite(&mp->MSGID.all, &msgIDVal);
	}
	msgCtrl = ((Uint32) mbxNumber << 8) | ((Uint32) dlc);
	CanDoubleWrite(&mp->MSGCTRL.all, &msgCtrl);

	clearbits32( (Uint32 *) &ECanaRegs.CANRMP.all, bitMask );

	setbits32( (Uint32 *) &ECanaRegs.CANMD.all, bitMask );        // Receive enable
	setbits32( (Uint32 *) &ECanaRegs.CANME.all, bitMask );

	return 0;
}

void canC_pollRecv(void) {
// Called periodically to see if we have received any CAN messages
// and act on them

	union canRecvDataUnion {
		Uint16 canRecvData16[4];
		Uint32 canRecvData32[2];
	} CRDU;

	union canXmitDataUnion {
		Uint16 canXmitData16[4];
		Uint32 canXmitData32[2];
	} CXDU;

    Uint16 *rcvMsg;
    Uint16 *xmtMsg;
    Uint32 *msgDBL;

    union CANOPENMBOXA *mboxaBitsXmit;

	Uint16   mbxNumber;
	Uint32   bitMask;
	volatile struct MBOX *mp;
    enum CANOPEN_STATUS canOpenStatus;

	mboxaBitsXmit = (union CANOPENMBOXA *)CXDU.canXmitData16;

	//Read all Receive mailboxes 16-31 and see if we got a message
	//mailbox 31 is set up to receive our normal SDO traffic from COB ID 0x60n (n=node #)
	//Durring development, if we receive anything on any other mailbox, we need
	//to evaluate it and see how to handle it.
	//After development, we can probably just close unused mailboxes, don't read them.
    for(mbxNumber=16; mbxNumber<32; mbxNumber++)    // Read & check 16 mailboxes
    {
    	bitMask = (1L << mbxNumber);
     	if( (read32( (Uint32 *) &ECanaRegs.CANRMP.all) & bitMask) != 0 ) {
     		// If so then we got a message

     		//Lets read 4 words from the mailbox
            mp = &((&ECanaMboxes.MBOX0)[mbxNumber]); // Point mp at the appropriate MBX
    	    // Use DoubleReads to get data from mailbox, avoid possible issue due to bus contention
            msgDBL = CRDU.canRecvData32;
            CanDoubleRead((msgDBL+0), &mp->MDL.all);            // Read mailbox twice (hw workaround)
            CanDoubleRead((msgDBL+1), &mp->MDH.all);            // Read from specified mailbox twice

            // don't need MSGID & MSGCTRL, but following 2 lines would read them for us
            //CanDoubleRead((msgDBL+2), &mp->MSGID.all);          // Read from specified mailbox twice
            //CanDoubleRead((msgDBL+3), &mp->MSGCTRL.all);        // Read from specified mailbox twice

            //diagnostic displays contents of receive MBOX
            // diagRs232CanRecvMsg(mbxNumber, CRDU.canRecvData16);

    	    //Also better reset receive flag
    	    setbits32( (Uint32 *) &ECanaRegs.CANRMP.all, bitMask );

            // Lets see if we recognize the request and try to respond to it
    	    rcvMsg = CRDU.canRecvData16;
    	    xmtMsg = CXDU.canXmitData16;
    	    // default starting contents for the response msg is copied from contents of reveived message
    	    *(xmtMsg++) = *(rcvMsg++);
    	    *(xmtMsg++) = *(rcvMsg++);
    	    *(xmtMsg++) = *(rcvMsg++);
    	    *xmtMsg     = *rcvMsg;
    	    rcvMsg = CRDU.canRecvData16;
    	    xmtMsg = CXDU.canXmitData16;

    	    // Here's where we hand it off to CanOpen to see what the sender is requesting,
    	    // and do it. If it returns true, it means we were successful and the xmtMsg buffer
    	    // is ready to return to the sender.
    	    canOpenStatus = canO_HandleCanOpenMessage(rcvMsg, xmtMsg);
            if (canOpenStatus == CANOPEN_NO_ERR){
            	canC_transmitMessage( 1, xmtMsg );
            } else {
            	CXDU.canXmitData16[2] = (Uint16)canOpenStatus;
            	CXDU.canXmitData16[3] = 0x00;

            	// abortDomain()returns 0x80 msg type code

            	mboxaBitsXmit->exp_sdo.CmndSpc = 4;
            	mboxaBitsXmit->exp_sdo.fill0 = 0;
            	mboxaBitsXmit->exp_sdo.bytes_no_data = 0;
            	mboxaBitsXmit->exp_sdo.expedite = 0;
            	mboxaBitsXmit->exp_sdo.size_indctr = 0;

            	canC_transmitMessage( 1, xmtMsg );
            	return;
            }

            //
            // Hueristic look at message codes / command specs
            //   Recv  Reply
            //   0x40  0x43 single-packet SDO upload, return data to PC
            //   0x22  0x60 single-packet SDO download, recv msg includes data from PC
            //         0x80 abortdomain -- test station can't handle recv msg
            //
     	}
    }
}

void canC_transmitMessage( int mbxNumber, Uint16 *msg ) {
// from Aimco6

	volatile far struct MBOX          *mp;
	Uint32   bitMask = (1L << mbxNumber);
	Uint32   *dp;
	Uint16   *sp;

	union canTxDataUnion {
		Uint16 canTxData16[4];
		Uint32 canTxData32[2];
	} CTDU;

	if( (read32( (Uint32 *) &ECanaRegs.CANMD.all ) & bitMask) != 0L )     // Xmit mailbox?
		return;
	if( (read32( (Uint32 *) &ECanaRegs.CANME.all ) & bitMask) == 0L )     // Enabled?
		return;

    // CANTRS: flag indicating if previous transmission is still pending
    // CANTRR: write 1 to reset flag in CANTRS
	if( (read32( (Uint32 *) &ECanaRegs.CANTRS.all ) & bitMask) != 0 ) {
		setbits32( (Uint32 *) &ECanaRegs.CANTRR.all, bitMask );
		while( (read32( (Uint32 *) &ECanaRegs.CANTRS.all ) & bitMask) != 0 ) {
			// EALLOW;
			// serviceDog();
			//EDIS;
		}
	}

    // CANTA: write 1 to reset flag indicating if prev msg was transmitted successfully
    // CANAA: write 1 to reset flag indicating if prev msg was aborted
	if( (read32( (Uint32 *) &ECanaRegs.CANTA.all ) & bitMask) != 0 ) {
		setbits32( (Uint32 *) &ECanaRegs.CANTA.all, bitMask );
 	}
	if( (read32( (Uint32 *) &ECanaRegs.CANAA.all ) & bitMask) != 0 ) {
		setbits32( (Uint32 *) &ECanaRegs.CANAA.all, bitMask );
 	}

	// Set the transmit data
	mp = &((&ECanaMboxes.MBOX0)[mbxNumber]); // Point mp at the appropraite MBX

	// Copy caller's 4-word message data into local union, accessable as either 16 or 32 bit data
	sp = CTDU.canTxData16;
	*sp++ = *msg++;
	*sp++ = *msg++;
	*sp++ = *msg++;
	*sp = *msg;

	// Using 32-bit double-writes to avoid bus-contension issue.
	dp = (Uint32 *) &mp->MDL;
	CanDoubleWrite(dp, &CTDU.canTxData32[0]);
	dp = (Uint32 *) &mp->MDH;
	CanDoubleWrite(dp, &CTDU.canTxData32[1]);

	// Set CANTRS bit to start the transmission
	setbits32( (Uint32 *) &ECanaRegs.CANTRS.all, bitMask );

	while( (read32( (Uint32 *) &ECanaRegs.CANTRS.all ) & bitMask) == 0 ) { // Wait for the message to leave...
		// EALLOW;
		// serviceDog();
		// EDIS;
	}
}

//===========================================================================
// Multi Segment Uploads and Downloads
//===========================================================================

