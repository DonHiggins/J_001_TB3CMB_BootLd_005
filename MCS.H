// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     MCS.H
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef MCSx_H
#define MCSx_H

enum CANOPEN_STATUS mcs_recv(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS mcs_sendMcsCb(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS mcs_sendMcsStatus(struct CAN_COMMAND* can_command, Uint16* data);

void mcs_init_mcs_cb(void);
bool mcs_readyToProgram(void);

union EXTRAM16_32 {
	Uint32 		all;
	struct TWO_WORDS_ {
		Uint16 lsw;
		Uint16 msw;
	} words;
};

 // used to manage download of whole MCS <Motorola format object> file
struct MCS_DOWNLOAD_CONTROL_BLOCK {
	Uint32 new_program_buff_count;   // # of words of data stored into new_program_buff
	Uint32 new_program_start_addr;   // FLASH dest addr for first word stored into new_program_buff
	Uint16 load_addr_msw;			 // MS-16 bits of 32-bit load address, from type 04 record
	Uint16 flash_sectors_to_program; // (SECTOR0+SECTOR1+SECTOR2+SECTOR3);
	Uint16 MCS_records;              // number of records received
	Uint16 MCS_data_records;         // number of type 00 data records received
	Uint16 MCS_format_errors;        // any MCS records not formatted properly?
	Uint16 first_format_err_rec;
	Uint16 MCS_checksum_erors;       // any checksum errors on MCS records?
	Uint16 first_checksum_err_rec;
	Uint16 MCS_non_contiguous;       // any places where code addresses were not contiguous <error>.
	Uint16 first_non_contig_err_rec;
	Uint16 MCS_running_checksum;     // checksum on words placed into new_program_buff.
	Uint16 MCS_checksum_validation;  // checksum of words read from new_program_buff.
};


#endif
