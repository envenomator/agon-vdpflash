; Title:	Initialisation Code for flashloader
; Author:	Jeroen Venema

; Memory parameters from compiled MOS
__STACKINIT:				EQU A0000h
__CS0_LBR_INIT_PARAM:		EQU 04h
__CS0_UBR_INIT_PARAM:		EQU 0bh
__CS0_CTL_INIT_PARAM:		EQU 08h
__CS0_BMC_INIT_PARAM:		EQU 01h
__CS1_LBR_INIT_PARAM:		EQU c0h
__CS1_UBR_INIT_PARAM:		EQU c7h
__CS1_CTL_INIT_PARAM:		EQU 08h
__CS1_BMC_INIT_PARAM:		EQU 00h
__CS2_LBR_INIT_PARAM:		EQU 80h
__CS2_UBR_INIT_PARAM:		EQU bfh
__CS2_CTL_INIT_PARAM:		EQU 08h
__CS2_BMC_INIT_PARAM:		EQU 00h
__CS3_LBR_INIT_PARAM:		EQU 03h
__CS3_UBR_INIT_PARAM:		EQU 03h
__CS3_CTL_INIT_PARAM:		EQU 18h
__CS3_BMC_INIT_PARAM:		EQU 82h
__RAM_CTL_INIT_PARAM:		EQU 80h
__RAM_ADDR_U_INIT_PARAM:	EQU B7h
__FLASH_CTL_INIT_PARAM:		EQU 28h
__FLASH_ADDR_U_INIT_PARAM:	EQU 00h

			INCLUDE	"equs.inc"
			INCLUDE "ez80f92.inc"

			XDEF _wait_timer0
			XDEF _enableFlashKeyRegister
			XDEF _lockFlashKeyRegister
			XDEF _fastmemcpy
			XDEF _reset

			XREF __low_bss
			XREF __len_bss			
			XREF _main

			SEGMENT CODE
			.ASSUME	ADL = 1	

; Start in ADL mode
			JP	_start			; Jump to start
;
; The header stuff
;
_exec_name:		DB	"FLASHLOADER.BIN", 0		; The executable name, only used in argv
;
; Place the waitBreakpoint routine at a fixed address (0x40020) so that it
; won't be affected by code changes in the future.
;
			ALIGN	64			; The executable header is from byte 64 onwards
			DB	"MOS"			; Flag for MOS - to confirm this is a valid MOS command
			DB	00h			; MOS header version 0
			DB	01h			; Flag for run mode (0: Z80, 1: ADL)

;
; And the code follows on immediately after the header
;
_start:		DI
; Config SPI
			LD A, %04
			OUT0 (SPI_CTL), A        ; SPI

; Configure external memory
			LD A, __CS0_LBR_INIT_PARAM
			OUT0 (CS0_LBR), A
			LD A, __CS0_UBR_INIT_PARAM
			OUT0 (CS0_UBR), A
			LD A, __CS0_BMC_INIT_PARAM
			OUT0 (CS0_BMC), A
			LD A, __CS0_CTL_INIT_PARAM
			OUT0 (CS0_CTL), A

			LD A, __CS1_LBR_INIT_PARAM
			OUT0 (CS1_LBR), A
			LD A, __CS1_UBR_INIT_PARAM
			OUT0 (CS1_UBR), A
			LD A, __CS1_BMC_INIT_PARAM
			OUT0 (CS1_BMC), A
			LD A, __CS1_CTL_INIT_PARAM
			OUT0 (CS1_CTL), A

			LD A, __CS2_LBR_INIT_PARAM
			OUT0 (CS2_LBR), A
			LD A, __CS2_UBR_INIT_PARAM
			OUT0 (CS2_UBR), A
			LD A, __CS2_BMC_INIT_PARAM
			OUT0 (CS2_BMC), A
			LD A, __CS2_CTL_INIT_PARAM
			OUT0 (CS2_CTL), A

			LD A, __CS3_LBR_INIT_PARAM
			OUT0 (CS3_LBR), A
			LD A, __CS3_UBR_INIT_PARAM
			OUT0 (CS3_UBR), A
			LD A, __CS3_BMC_INIT_PARAM
			OUT0 (CS3_BMC), A
			LD A, __CS3_CTL_INIT_PARAM
			OUT0 (CS3_CTL), A

; Enable internal memory
			LD A, __FLASH_ADDR_U_INIT_PARAM
			OUT0 (FLASH_ADDR_U), A
			LD A, __FLASH_CTL_INIT_PARAM
			OUT0 (FLASH_CTRL), A

			LD A, __RAM_ADDR_U_INIT_PARAM
			OUT0 (RAM_ADDR_U), A
			LD A, __RAM_CTL_INIT_PARAM
			OUT0 (RAM_CTL), A

; Setup the stack
			LD SP, __STACKINIT

; Clear BSS
			CALL	_clear_bss

; Push 0 parameters and call main
			LD  BC, 0
			PUSH	BC			; Parameter 1: argc
			CALL	_main			; int main(int argc, char *argv[])

infinite:
			JP infinite
			
; Clear the memory
;
_clear_bss:	LD	BC, __len_bss		; Check for non-zero length
			LD	a, __len_bss >> 16
			OR	A, C
			OR	A, B
			RET	Z			; BSS is zero-length ...
			XOR	A, A
			LD 	(__low_bss), A
			SBC	HL, HL			; HL = 0
			DEC	BC			; 1st byte's taken care of
			SBC	HL, BC
			RET	Z		  	; Just 1 byte ...
			LD	HL, __low_bss		; Reset HL
			LD	DE, __low_bss + 1	; [DE] = bss + 1
			LDIR				; Clear this section
			RET

; Wait for timer0 to hit 0
_wait_timer0:
			PUSH	AF 
			PUSH	BC 
			IN0	A, (TMR0_CTL)	; Enable the timer
			OR	3
			OUT0	(TMR0_CTL), A
$$:			IN0	B, (TMR0_DR_L)	; Fetch the counter L
			IN0 	A, (TMR0_DR_H)	; And the counter H
			OR	B 
			JR	NZ, $B
			POP	BC 
			POP	AF 
			RET

_enableFlashKeyRegister:
	push	ix
	ld		ix,0
	add		ix,sp
	
	ld		a, b6h			; unlock
	out0	(F5h), a
	ld		a, 49h
	out0	(F5h), a
	
	ld		sp,ix
	pop		ix
	ret

_lockFlashKeyRegister:
	push	ix
	ld		ix,0
	add		ix,sp
	
	ld		a, ffh			; lock
	out0	(F5h), a
	
	ld		sp,ix
	pop		ix
	ret
	
_reset:
	rst 0h
	ret	;will never get here
	
_fastmemcpy:
	push	ix
	ld		ix,0
	add		ix,sp
		
	push bc
	push de
	push hl
	
	ld		de, (ix+6)	; destination address
	ld		hl, (ix+9)	; source
	ld		bc, (ix+12)	; number of bytes to write to flash
	ldir

	pop hl
	pop de
	pop bc

	ld		sp,ix
	pop		ix
	ret
