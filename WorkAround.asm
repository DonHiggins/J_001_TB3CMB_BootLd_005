;****************************************************************************************
; C callable assembly functions to access to Can hardware register or memory location   *
;****************************************************************************************

RETRYCOUNT  .set    (5-1)                  ; 4 retries = total of 5 tries
WRITECOUNT  .set    (3-1)                  ; 2 retries = total of 3 tries

;****************************************************************************************
;   C callable assembly function to read Can Registers multiple times as a workaround   *
;   for TI hardware problem (read access conflict between DSP and Can state-machine).   *
;   [XAR5]-->address of Can register      [XAR4]-->address to store data (reg value).   *
;****************************************************************************************
        .global _CanDoubleRead
_CanDoubleRead:
        MOVB  AR6, #RETRYCOUNT
        PUSH  ST1                               ; Save status/state register on stack
        SETC  INTM                              ; Disable interrupts
CanDoubleReRead:
        MOVL  ACC, *XAR5                        ; read the CAN Register multiple times
        SBF   CanDoubleReadExit, NEQ            ;      until a non-zero value is read,
        BANZ  CanDoubleReRead,   AR6--          ;      or the reread count exausts.
CanDoubleReadExit:
        POP   ST1                               ; Restore status/state registers
        MOVL  *XAR4,ACC                         ; return value read into memory
        LRETR


;****************************************************************************************
;   C callable assembly function to write Can Registers multiple times as a workaround  *
;   for TI hardware problem (write access conflict between DSP and Can state-machine).  *
;   [XAR4]-->address of Can register      [XAR5]-->address of load data to be written.  *
;****************************************************************************************
        .global _CanDoubleWrite
_CanDoubleWrite:
        MOVB  AR6, #RETRYCOUNT
        MOVL  ACC,*XAR5                         ; Load ACC with value to write
        PUSH  ST1                               ; Save status register on stack
        SETC  INTM                              ; Disable interrupts
CanDoubleReWrite:
        MOVL  *XAR4, ACC                        ; Store value in register twice
        RPT   #2  || NOP                ; keep read loop and write loop timing the same
        BANZ  CanDoubleReWrite, AR6--
        POP   ST1                               ; Restore status register
        LRETR



;****************************************************************************************
;   C callable assembly function to read/modify/write a Can Register multiple times as  *
;   a workaround for TI hardware problem (register access conflict DSP/state-machine).  *
;   [XAR4]-->address of Can register to be read, selected bits modified and rewritten.  *
;   [XAR5]-->address of load data  (bits=0: nop   bits=1: clear corresponding reg-bit.  *
;****************************************************************************************
        .global _CanDoubleClear
_CanDoubleClear:
        MOVB  AR6, #RETRYCOUNT
        PUSH  ST1                               ; Save status register on stack
        SETC  INTM                              ; Disable interrupts
CanDoubleClearRead:
        MOVL  ACC,*XAR4                         ; read the CAN Register up to
        SBF   CanDoubleClearBits,   NEQ         ;      four times in a row if
        BANZ  CanDoubleClearRead,   AR6--
CanDoubleClearBits:
        NOT   ACC
        OR    AL, *+XAR5[0]                     ; Reset selected bits in the designated
        OR    AH, *+XAR5[1]                     ;       register up to 32-bits of them.
        NOT   ACC
        MOVB  AR6, #RETRYCOUNT
CanDoubleClearWrite:
        MOVL  *XAR4, ACC                        ; Store value in register twice
        RPT   #2  || NOP                ; keep read loop and write loop timing the same
        BANZ  CanDoubleClearWrite, AR6--
        POP   ST1                               ; Restore status register
        LRETR



;****************************************************************************************
;   C callable assembly function to write Can Registers multiple times as a workaround  *
;   for TI hardware problem (write access conflict between DSP and Can state-machine).  *
;   [XAR4]-->address of Can register           [XAR5]-->address of data to be written.  *
;   [XAR4]-->address of Can register to be read, selected bits modified and rewritten.  *
;   [XAR5]-->address of load data   (bits=0: nop   bits=1: set corresponding reg-bits.  *
;****************************************************************************************
        ; XAR4 -> RegPtr
        ; XAR5 -> LoadPtr
        .global _CanDoubleSet
_CanDoubleSet:
        MOVB  AR6, #RETRYCOUNT
        PUSH  ST1                               ; Save status register on stack
        SETC  INTM                              ; Disable interrupts
CanDoubleSetRead:
        MOVL  ACC, *XAR4                        ; read the CAN Register up to
        SBF   CanDoubleSetBits,  NEQ            ;      four times in a row if
        BANZ  CanDoubleSetRead,  AR6--
CanDoubleSetBits:
        OR    AL, *+XAR5[0]                     ; Set selected bits in the designated
        OR    AH, *+XAR5[1]                     ;     register up to 32-bits of them.
        MOVB  AR6, #RETRYCOUNT
CanDoubleSetWrite:
        MOVL  *XAR4, ACC                        ; Store value in register twice
        RPT   #2  || NOP                ; keep read loop and write loop timing the same
        BANZ  CanDoubleSetWrite, AR6--
        POP   ST1                               ; Restore status register
        LRETR



;************************************************************************************************************************
;   The Mbx Number (input parameter) has already been configured to transmit a 6-byte Can Timestamp message.   This     *
;   function will temporarily disable the mailbox, update the 6-byte data field with the current time (parameters),     *
;   re-enable the mailbox, and set the hardware trigger to transmit a new timestamp message on the Can bus.             *
;************************************************************************************************************************
MBX_ENABLE_REG        .set          6000h;
MBX0_DATA_LOW         .set          6104h;
MBX0_DATA_HIGH        .set          6106h;
SIXEOFMBX             .set             8;
MBX_TRS_REG           .set          6004h;
        .global _XmitCanTimestamp
_XmitCanTimestamp:
        ; AL   =  Mailbox number
        ; XAR4 -> Ptr to Milliseconds
        ; XAR5 -> Ptr to Days
        MOVL    XAR6,       #MBX_ENABLE_REG         ;   setup address of Mbx enable register
        PUSH    ST1                                 ;   Save status register on stack
        SETC    INTM                                ;   Disable interrupts

        MOVL    P,          *XAR6                   ; 0 1st read of Mbx enable Register
        MOVL    XAR7,       #MBX0_DATA_LOW          ; 1 build the address of the
        MOV     T,          AL                      ; 2       selected mailbox's
        MPYB    ACC,        T,      #SIXEOFMBX      ; 3       data low dword
        ADDL    @XAR7,      ACC                     ; 4
        MOVL    ACC,        *XAR6                   ; 5 2nd read of Mbx enable Register
        MOVL    P,          ACC,    NEQ             ;       use 2nd read if not zero

        MOVB    ACC,        #1                      ; disable the selected mailbox
        LSL     ACC,        T                       ;         while we update the
        NOT     ACC                                 ;         mailbox data field.
        AND     AH,         PH
        AND     AL,         PL
        MOVL    *XAR6,      ACC                     ; 1st write mbx enable register
        RPT     #2
     || NOP
        MOVL    *XAR6,      ACC                     ; 2nd write mbx enable register

        MOVB    AL.LSB,     *+XAR4[3]               ; reformat the data into big
        MOVB    AL.MSB,     *+XAR4[2]               ;          endian format for
        MOVB    AH.LSB,     *+XAR4[1]               ;          the Can bus.
        MOVB    AH.MSB,     *+XAR4[0]
        MOVL    *XAR7,      ACC                     ; 1st write of mbx data field
        RPT     #2
     || NOP
        MOVL    *XAR7,      ACC                     ; 2nd write of mbx data field

        MOVB    AH.LSB,     *+XAR5[1]               ; reformat the data into big
        MOVB    AH.MSB,     *+XAR5[0]               ;          endian format. for
        MOVL    *+XAR7[2],  ACC                     ; 1st write of mbx data field
        RPT     #2
     || NOP
        MOVL    *+XAR7[2],  ACC                     ; 2nd write of mbx data field

        MOVL    *XAR6,      P                       ; 1st write to re-enable the
        MOVB    ACC,        #1                      ;     selected mailbox.
        LSL     ACC,        T                       ;
        MOVL    XAR7,       #MBX_TRS_REG            ;
        NOP
        MOVL    *XAR6,      P                       ; 2nd write to re-enable mbx

        MOVL    *XAR7,      ACC                     ; 1st write to selected mbx's
        RPT     #2                                  ;     Transmit-Request-Send
     || NOP                                         ;     register flag.
        MOVL    *XAR7,      ACC                     ; 2nd write to TRS register

        POP     ST1                                 ; restore interrupt state
        LRETR                                       ;         and return
