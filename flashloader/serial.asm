PORT			EQU	%D0			; UART1
				
REG_RBR:		EQU	PORT+0		; Receive buffer
REG_THR:		EQU	PORT+0		; Transmitter holding
REG_DLL:		EQU	PORT+0		; Divisor latch low
REG_IER:		EQU	PORT+1		; Interrupt enable
REG_DLH:		EQU	PORT+1		; Divisor latch high
REG_IIR:		EQU	PORT+2		; Interrupt identification
REG_FCT			EQU	PORT+2;		; Flow control
REG_LCR:		EQU	PORT+3		; Line control
REG_MCR:		EQU	PORT+4		; Modem control
REG_LSR:		EQU	PORT+5		; Line status
REG_MSR:		EQU	PORT+6		; Modem status

REG_SCR:		EQU PORT+7		; Scratch

TX_WAIT			EQU	16384 		; Count before a TX times out

UART_LSR_ERR	EQU %80			; Error
UART_LSR_ETX	EQU %40			; Transmit empty
UART_LSR_ETH	EQU	%20			; Transmit holding register empty
UART_LSR_RDY	EQU	%01			; Data ready


			.ASSUME	ADL = 1
			SEGMENT CODE

			XDEF	_init_UART0
			;XDEF	_uart1_getch
			;XDEF	_uart1_putch

			include "ez80F92.inc"			

PORTD_DRVAL_DEF       .equ    0ffh			;The default value for Port D data register (set for Mode 2).
PORTD_DDRVAL_DEF      .equ    0ffh			;The default value for Port D data direction register (set for Mode 2).
PORTD_ALT0VAL_DEF     .equ    0ffh			;The default value for Port D alternate register-0 (clear interrupts).
PORTD_ALT1VAL_DEF     .equ    000h			;The default value for Port D alternate register-1 (set for Mode 2).
PORTD_ALT2VAL_DEF     .equ    000h			;The default value for Port D alternate register-2 (set for Mode 2).

; baudrate divisors
; 18432000 / 16*1152000 = 1,0
BRD_LOW                .equ    001h
BRD_HIGH               .equ    000h

_init_UART0:
    ; all pins to GPIO mode 2, high impedance input
    ld a, PORTD_DRVAL_DEF
    out0 (PD_DR),a
    ld a, PORTD_DDRVAL_DEF
    out0 (PD_DDR),a
    ld a, PORTD_ALT1VAL_DEF
    out0 (PD_ALT1),a
    ld a, PORTD_ALT2VAL_DEF
    out0 (PD_ALT2),a

    ; initialize for correct operation
    ; pin 0 and 1 to alternate function
    ; set pin 3 (CTS) to high-impedance input
    in0 a,(PD_DDR)
    or  00001011b; set pin 0,1,3
    out0 (PD_DDR), a
    in0 a,(PD_ALT1)
    and 11110100b; reset pin 0,1,3
    out0 (PD_ALT1), a
    in0 a,(PD_ALT2)
    and 11110111b; reset pin 3
    or  00000011b; set pin 0,1
    out0 (PD_ALT2), a
    
    in0 a,(UART0_LCTL)
    or 10000000b ; set UART_LCTL_DLAB
    out0 (UART0_LCTL),a
    ld a, BRD_LOW ;// Load divisor low
    out0 (UART0_BRG_L),a
    ld a, BRD_HIGH ;// Load divisor high
    out0 (UART0_BRG_H),a
    in0 a,(UART0_LCTL)
    and 01111111b ; reset UART_LCTL_DLAB
    out0 (UART0_LCTL),a
    ld a, 000h  ; reset modem control register
    out0 (UART0_MCTL),a
    ld a, 007h  ; enable and clear hardware fifo's
    out0 (UART0_FCTL),a
    ld a, 000h  ; no interrupts
    out0 (UART0_IER),a
    in0 a, (UART0_LCTL)
    or  00000011b    ; 8 databits, 1 stopbit
    and 11110111b    ; no parity
    out0 (UART0_LCTL),a
	ret