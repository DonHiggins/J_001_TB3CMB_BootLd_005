;
; access functions to force 32 accesses to CAN peripheral registers
;
;
;	unsigned long read32( unsigned long *addr )
;
	.global	_read32
	
_read32:
	movl	acc, *xar4
	lretr
	
;
;	void write32( unsigned long *addr, unsigned long data )
;
	.global	_write32
	
_write32:
	movl	*xar4, acc
	lretr
	
;
;	void setbits32( unsigned long *addr, unsigned long data )
;
	.global	_setbits32
	
_setbits32:
	movl	p, *xar4
	or	ah, @ph
	or      al, @pl
	movl	*xar4, acc
	lretr
;
;	void clearbits32( unsigned long *addr, unsigned long data )
;
	.global	_clearbits32
	
_clearbits32:
	movl	p, *xar4
	not	acc
	and	ah, @ph
	and	al, @pl
	movl	*xar4, acc
	lretr
	
	
	
	
	

	