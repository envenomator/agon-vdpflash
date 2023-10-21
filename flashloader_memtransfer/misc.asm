
			INCLUDE	"equs.inc"

			.ASSUME	ADL = 1

			SEGMENT CODE
							
			XDEF	_wait_timer0 
; Wait for timer0 to hit 0
;
__wait_timer0:
_wait_timer0:		PUSH	AF 
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