;// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;//
;//     BootLdEntry.asm
;//
;//   This provides an entry point from the Main Program into the Bootloader
;//		via a vector at a fixed location.
;//
;// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    .ref _c_int00
	.global  _vector_from_main_prog
	.global _BL_key
;***********************************************************************
;* Function: bootentry section
;*
;* Description: Branch to code starting point
;***********************************************************************
    .sect "bootentry"

_vector_from_main_prog:
        LB _c_int00         ;//Branch to start program

_BL_key  .word 0x2001		;//When main prog sees these values in Flash, rather than 0xFFFF's
         .word 0x1776       ;//It knows that the Bootloader program is installed, and that
                            ;//it can jump to _vector_from_main_prog:

;end bootentry section

