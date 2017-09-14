// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     MCS.C
//
// Parse received MCS records
// Store data to be programmed into Flash
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "CanOpen.h"
#include "MCS.h"
#include "Flash281x_API_Library.h"
#include "stdbool.h"            // needed for bool data types

extern struct SDO_MULTI_SEGMENT sdo_multi_segment_control_block;
#define SDO_MS_CB sdo_multi_segment_control_block

extern struct MULTI_PACKET_BUF multi_packet_buf;

// used to manage download of whole MCS <Motorola format object> file
struct MCS_DOWNLOAD_CONTROL_BLOCK mcs_download_control_block;
#define MCS_CB mcs_download_control_block

#define NEW_PROG_BUF_SIZE 0x40000
Uint16 *new_program_buff;

extern Uint16 ExtRamStart;
extern Uint16 ExtRamSize;
union EXTRAM16_32 extRamSize;
bool mcsReceivedFileReadyToProgram;


//***************************************************************************************************
// - - - - - M C S Routines - - - - - - - - - - - - - - - - - - -
// decoding Motorola object format records from .MCS file
//***************************************************************************************************
void mcs_init_mcs_cb(void){
	MCS_CB.new_program_buff_count = 0;
	MCS_CB.new_program_start_addr = 0;
	MCS_CB.load_addr_msw = 0;
	MCS_CB.flash_sectors_to_program = 0;
	MCS_CB.MCS_records = 0;
	MCS_CB.MCS_data_records = 0;
	MCS_CB.MCS_format_errors = 0;
	MCS_CB.first_format_err_rec = 0;
	MCS_CB.MCS_checksum_erors = 0;
	MCS_CB.first_checksum_err_rec = 0;
	MCS_CB.MCS_non_contiguous = 0;
	MCS_CB.first_non_contig_err_rec = 0;
	MCS_CB.MCS_running_checksum = 0;
	MCS_CB.MCS_checksum_validation = 0;

	// we store downloaded program data into our external RAM chip
	new_program_buff = &ExtRamStart;  //0x10,0000 - symbolic address from .cmd file
	extRamSize.all = (Uint32)&ExtRamSize; // extRamSize.all is a 32-bit value 0x4,0000
    mcsReceivedFileReadyToProgram = false;
}

bool mcs_readyToProgram(void){
	// main loop calls here to see if we have received a whole
	// file and are ready to erase and reprogram flash.
	return mcsReceivedFileReadyToProgram;
}

void MCS_format_error(void) {
// detected a format error in an MCS record.  Log this in MCS_CB.
     MCS_CB.MCS_format_errors += 1;
     if (MCS_CB.first_format_err_rec == 0) MCS_CB.first_format_err_rec = MCS_CB.MCS_records;
asm("_Brk_Pt_6:	NOP");         // public symbol useful for breakpoint, NOT destination for goto or call
}

void mcs_recv_record(void) {
// From the PC, the bootload receives DSP software formatted as a "Motorola", "MCS"
//   file.  Here we've received one record from the file.  It is stored in rx_buff.
//   Length of the record is SDO_MS_CB.byte_count_so_far.
// Example MCS records look like this:
//
//    # of data bytes in record	(2 hex digits)
//    |  Address - 4 hex digits
//    |  |   Record type: 00 -> data, 01 -> EOF
//    |  |   | ROM image data - 4 Hex digits per 16-bit word                  checksum - 2 hex digits
//    |  |   | |                                                              |
//   :20100000798030C0798029FF79802A0379802B4C79802B5079802B5479802B582001101780
//   :2010100012340000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF96
//   :00000001FF

  Uint16 byte_count;
  Uint16 i; // index into record, or sometimes binary value from conversion of ASCII/Hex
  Uint32 i_32;
  Uint16 k; // running sum for checksum, or sometimes binary value from conversion of ASCII/Hex
  Uint16 addr;
  Uint16 *rx_ptr;
  char *rx_buff;
  union EXTRAM16_32 *data_32;
  union EXTRAM16_32 addr_check;

  rx_buff = &multi_packet_buf.buff[0];

  MCS_CB.MCS_records += 1; // keep track of how many records we have received

  //Make sure what we received appears to be an MCS record
  if (rx_buff[0] != ':') {
     MCS_format_error();
     return;
  }

  //Convert all ascii values in rx_buff to binary
  rx_ptr = (Uint16 *)&rx_buff[1];
  for (i = 1;i < SDO_MS_CB.byte_count_so_far;i++) {
     *rx_ptr -= '0';
     if (*rx_ptr > 9) *rx_ptr -= 7;
     rx_ptr++;
  }

  // rx_buf[1] & rx_buf[2] hold a a 2-byte, ASCII/hex , byte-count -- count of data bytes in record
  // make sure # of bytes received in serial transmission of record agrees with ASCII/hex byte count in message
  rx_ptr = (Uint16 *)&rx_buff[1];
  byte_count = (*(rx_ptr++)) << 4;
  byte_count |= *rx_ptr;
  if (((byte_count << 1) + 11) > SDO_MS_CB.byte_count_so_far) {
     MCS_format_error();
     return;
  }

  //Now check the checksum on the record
  rx_ptr = (Uint16 *)&rx_buff[1];
  k = 0; // k accumulates sum of "bytes"
  for (i=1;i<((byte_count << 1) + 11);i+=2) {
	 k += (*(rx_ptr++) << 4);    // k accumulates sum of "bytes"
	 k += *(rx_ptr++);           // k accumulates sum of "bytes"
     k &= 0xFF;                  //  mod 256
  }
  if (k != 0) {
     MCS_CB.MCS_checksum_erors += 1;
     if (MCS_CB.first_checksum_err_rec == 0) MCS_CB.first_checksum_err_rec = MCS_CB.MCS_records;
     return;
  }

  //Examine record type: 00 -> data record, 01 -> EOF, 04 -> Load Addr MSW
  rx_ptr = (Uint16 *)&rx_buff[7];
  i = (*(rx_ptr++)) << 4;
  i |= *rx_ptr;
  if (i == 0) {        // - - - - - - - - - - - - - - - - data record

	  MCS_CB.MCS_data_records += 1; // keep track of how many data records we have received

     // Look at destination address info
     rx_ptr = (Uint16 *)&rx_buff[3];
     addr = (*(rx_ptr++)) << 12;
     addr |= (*(rx_ptr++)) << 8;
     addr |= (*(rx_ptr++)) << 4;
     addr |= *rx_ptr++;
     if (MCS_CB.MCS_data_records == 1) { // this is first data record

        data_32 = (union EXTRAM16_32 *)(&MCS_CB.new_program_start_addr);
        data_32->words.lsw = addr;

     } else {
        // check for non-contiguous data
    	addr_check.all = MCS_CB.new_program_start_addr + MCS_CB.new_program_buff_count;
        if (addr != addr_check.words.lsw) {
           if (MCS_CB.first_non_contig_err_rec == 0) MCS_CB.first_non_contig_err_rec = MCS_CB.MCS_records;
           MCS_CB.MCS_non_contiguous += 1;
           return;
		}
     }

     // Add data to new_program_buff
     rx_ptr = (Uint16 *)&rx_buff[9];
     for (i=0;i < byte_count >> 1;i++) {
        if (MCS_CB.new_program_buff_count >= NEW_PROG_BUF_SIZE) {
           MCS_format_error();
           return;
	    }
        k = (*(rx_ptr++)) << 12;
        k |= (*(rx_ptr++)) << 8;
        k |= (*(rx_ptr++)) << 4;
        k |= (*(rx_ptr++));

	    new_program_buff[MCS_CB.new_program_buff_count] = k;
        MCS_CB.new_program_buff_count += 1;
        MCS_CB.MCS_running_checksum += k;  // add k into the running checksum
     }

     // After each MCS record, read all data in new_program_buff and calculate the checksum.
     // This should match the running checksum calculated as the data was written into the new_program_buff.
     MCS_CB.MCS_checksum_validation = 0;
     for (i_32=0;i_32 < MCS_CB.new_program_buff_count;i_32++) {
        MCS_CB.MCS_checksum_validation += new_program_buff[i_32];
     }

	 return; // exit here after copying data to new_program_buff

  } else if (i == 1) { // - - - - - - - - - - - - - - - - EOF record
     // Received complete MCS file.  Go ahead and Program it into FLASH
     if ((MCS_CB.MCS_format_errors == 0) &&  (MCS_CB.MCS_non_contiguous == 0) && (MCS_CB.MCS_checksum_erors == 0) && (MCS_CB.MCS_records > 1)) {
        // Go ahead and Program it into FLASH
        if (MCS_CB.new_program_start_addr == 0x3D8000){
        	// 0x3D8000 Main Program FLASHC - FLASHJ
            MCS_CB.flash_sectors_to_program =
               SECTORC | SECTORD | SECTORE | SECTORF | SECTORG | SECTORH | SECTORI | SECTORJ;
            mcsReceivedFileReadyToProgram = true;
        } else if (MCS_CB.new_program_start_addr == 0x3F4000){
        	// 0x3F4000 Bootloader FLASHA, FLASHB
            MCS_CB.flash_sectors_to_program = SECTORA | SECTORB;
            mcsReceivedFileReadyToProgram = true;
		} else {
           MCS_format_error();
           return;
		}
 		//waiting_on_code_download = 0;
	 }
	 return;

  } else if (i == 4) { // - - - - - - - - - - - - - - - - Load Addr MSW
	  // MCS file only codes 16-bit address in type 01 data records
	  // Type 04 record provides the MS 16 bits of the load address.
	  rx_ptr = (Uint16 *)&rx_buff[9];
      k = (*(rx_ptr++)) << 12;
      k |= (*(rx_ptr++)) << 8;
      k |= (*(rx_ptr++)) << 4;
      k |= (*(rx_ptr++));

      MCS_CB.load_addr_msw = k;

      // but only store the first type-04 record address in MCS_CB.new_program_start_addr.
      // We assume that any subsequent type-04 record indicates a rollover in the LS 16
      // bit address, but that data is contiguous.
      data_32 = (union EXTRAM16_32 *)(&MCS_CB.new_program_start_addr);
      if (data_32->all == 0) {
         data_32->words.msw = k;
      }

  } else {             // - - - - - - - - - - - - - - - - bogus record-type
     MCS_format_error();
     return;
  }
}


enum CANOPEN_STATUS mcs_recv(struct CAN_COMMAND* can_command, Uint16* data){
	// Received an entire MCS record, as result of a multi-packet receive.
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex

	mcs_recv_record();

	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS mcs_sendMcsCb(struct CAN_COMMAND* can_command, Uint16* data){
	// Send one 16-bit value from the mcs_download_control_block, based on CAN Subindex
	// This is strictly diagnostic info, in case we need to troubleshoot.
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex
	Uint16 subindex;

	// Subindex is MS 8 bits of MboxB
	subindex = ((*(data+1)) >> 8) & 0x00FF;
	union EXTRAM16_32 data_32;

	*(data+3) = 0; 	//MboxD -- defaults to 0 for 16-bit values

	switch(subindex){
    case 0x07:
    	data_32.all = MCS_CB.new_program_buff_count;
    	*(data+2) = data_32.words.lsw;  //MboxC
    	*(data+3) = data_32.words.msw;  //MboxD
        break;
    case 0x08:
    	data_32.all = MCS_CB.new_program_start_addr;
    	*(data+2) = data_32.words.lsw;  //MboxC
    	*(data+3) = data_32.words.msw;  //MboxD
        break;
    case 0x09:
    	*(data+2) = MCS_CB.load_addr_msw;  //MboxC
        break;
    case 0x0A:
    	*(data+2) = MCS_CB.flash_sectors_to_program;  //MboxC
        break;
    case 0x0B:
    	*(data+2) = MCS_CB.MCS_records;  //MboxC
        break;
    case 0x0C:
    	*(data+2) = MCS_CB.MCS_data_records;  //MboxC
        break;
    case 0x0D:
    	*(data+2) = MCS_CB.MCS_format_errors;  //MboxC
        break;
    case 0x0E:
    	*(data+2) = MCS_CB.first_format_err_rec;  //MboxC
        break;
    case 0x0F:
    	*(data+2) = MCS_CB.MCS_checksum_erors;  //MboxC
        break;
    case 0x10:
    	*(data+2) = MCS_CB.first_checksum_err_rec;  //MboxC
        break;
    case 0x11:
    	*(data+2) = MCS_CB.MCS_non_contiguous;  //MboxC
        break;
    case 0x12:
    	*(data+2) = MCS_CB.first_non_contig_err_rec;  //MboxC
        break;
    case 0x13:
    	*(data+2) = MCS_CB.MCS_running_checksum;  //MboxC
        break;
    case 0x14:
    	*(data+2) = MCS_CB.MCS_checksum_validation;  //MboxC
        break;
    default:
    	*(data+2) = 0xBADD;
   	break;
   }

	return CANOPEN_NO_ERR;
}

enum CANOPEN_STATUS mcs_sendMcsStatus(struct CAN_COMMAND* can_command, Uint16* data){
	// Send 1-bit value, indicating whether or not we have logged any errors in the
	// MCS download so far.
	// Return 0 --> No Errors
	// Return 1 --> Error(s)
	// *data is MboxA of transmit Message
	// *can_command is Table entry (struct) of parameters for CANOpen Index.Subindex


	if ((MCS_CB.MCS_non_contiguous == 0)
	&& (MCS_CB.MCS_checksum_erors == 0)
	&& (MCS_CB.MCS_format_errors == 0)) {
		*(data+2) = 0;
	} else {
		*(data+2) = 1;
	}

	*(data+3) = 0; 	//MboxD
	return CANOPEN_NO_ERR;
}
