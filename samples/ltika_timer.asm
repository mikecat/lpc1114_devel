%include "lpc1114_macro.inc"

start:
; Initial SP value
	dd 0x10000FF0
; reset
	dd reset - start + 1
; NMI
	dd nmi - start + 1
; HardFault
	dd hardfault - start + 1
; reserved
	dd 0
	dd 0
	dd 0
	dd -(0x10000FF0 + (reset - start + 1) + (nmi - start + 1) + (hardfault - start + 1))
	dd 0
	dd 0
	dd 0
; SVCall
	dd svcall - start + 1
; reserved
	dd 0
	dd 0
; PendSV
	dd pendsv - start + 1
; SysTick
	dd systick - start + 1
; IRQn
	dd irq0 - start + 1
	dd irq1 - start + 1
	dd irq2 - start + 1
	dd irq3 - start + 1
	dd irq4 - start + 1
	dd irq5 - start + 1
	dd irq6 - start + 1
	dd irq7 - start + 1
	dd irq8 - start + 1
	dd irq9 - start + 1
	dd irq10 - start + 1
	dd irq11 - start + 1
	dd irq12 - start + 1
	dd irq13 - start + 1
	dd irq14 - start + 1
	dd irq15 - start + 1
	dd irq16 - start + 1
	dd irq17 - start + 1
	dd irq18 - start + 1
	dd irq19 - start + 1
	dd irq20 - start + 1
	dd irq21 - start + 1
	dd irq22 - start + 1
	dd irq23 - start + 1
	dd irq24 - start + 1
	dd irq25 - start + 1
	dd irq26 - start + 1
	dd irq27 - start + 1
	dd irq28 - start + 1
	dd irq29 - start + 1
	dd irq30 - start + 1
	dd irq31 - start + 1
	times (63 - 48 + 1) dd 0

nmi:
hardfault:
svcall:
pendsv:
systick:
irq0:
irq1:
irq2:
irq3:
irq4:
irq5:
irq6:
irq7:
irq8:
irq9:
irq10:
irq11:
irq12:
irq13:
irq14:
irq15:
irq17:
irq18:
irq19:
irq20:
irq21:
irq22:
irq23:
irq24:
irq25:
irq26:
irq27:
irq28:
irq29:
irq30:
irq31:
	BX LR

irq16:
	; interrupt from timer 0
	LDR R0, TMR16B0IR
	LDR R3, GPIO0DATA_PIO0_11
	LDR R1, [R0]
	MOVS R2, #1
	TST  R1, R2
	BEQ irq16_no_mr0
	STR R2, [R0] ; clear MR0 interrupt
	MOVS R2, #1
	LSLS R2, #11
	STR R2, [R3] ; set PIO0_11 to HIGH
irq16_no_mr0:
	MOVS R2, #2
	TST R1, R2
	BEQ irq16_no_mr1
	STR R2, [R0] ; clear MR1 interrupt
	EORS R2, R2
	STR R2, [R3] ; set PIO0_11 to LOW
irq16_no_mr1:
	BX LR

reset:
	; enable clock for IOCON
	LDR R0, SYSAHBCLKCTRL
	LDR R1, [R0]
	MOVS R2, #1
	LSLS R2, #16
	ORRS R1, R2
	STR R1, [R0]

	; disable pull-up register on PIO0_11 and select function PIO0_11
	LDR R0, IOCON_R_PIO0_11
	MOVS R1, #0xC1
	STR R1, [R0]
	; set PIO0_11 to output, other PIO0_n to input
	LDR R0, GPIO0DIR
	MOVS R1, #1
	LSLS R1, #11
	STR R1, [R0]
	;set PIO0_11 to LOW
	LDR R0, GPIO0DATA_PIO0_11
	EORS R1, R1
	STR R1, [R0]

	; disable clock for IOCON and enable clock for timer 0
	LDR R0, SYSAHBCLKCTRL
	LDR R1, [R0]
	BICS R1, R2
	MOVS R2, #0x80
	ORRS R1, R2
	STR R1, [R0]

	; set timer prescaler to 12000 = 0x2EE0
	LDR R0, TMR16B0PR
	MOVS R1, #0x2E
	LSLS R1, #8
	ADDS R1, #0xE0
	STR R1, [R0]
	; set match register 0 to 900 = 0x384
	LDR R0, TMR16B0MR0
	MOVS R1, #0x03
	LSLS R1, #8
	ADDS R1, #0x84
	STR R1, [R0]
	; set match register 1 to 1000 = 0x3E8
	LDR R0, TMR16B0MR1
	MOVS R1, #0x03
	LSLS R1, #8
	ADDS R1, #0xE8
	STR R1, [R0]
	; reset the timer
	LDR R0, TMR16B0TCR
	MOVS R1, #2
	STR R1, [R0]
	; interrupt on MR0, interrupt and reset on MR1
	LDR R0, TMR16B0MCR
	MOVS R1, #0x19
	STR R1, [R0]
	; enable IRQ16 interrupt
	LDR R0, ISER
	MOVS R1, #1
	LSLS R1, #16
	STR R1, [R0]
	; start the timer
	LDR R0, TMR16B0TCR
	MOVS R1, #1
	STR R1, [R0]

	; endless loop
mainloop:
	B mainloop

	align 4
SYSAHBCLKCTRL:
	dd 0x40048080
IOCON_R_PIO0_11:
	dd 0x40044074
GPIO0DIR:
	dd 0x50008000
GPIO0DATA_PIO0_11:
	dd 0x50002000
ISER:
	dd 0xE000E100
TMR16B0IR:
	dd 0x4000C000
TMR16B0TCR:
	dd 0x4000C004
TMR16B0PR:
	dd 0x4000C00C
TMR16B0MCR:
	dd 0x4000C014
TMR16B0MR0:
	dd 0x4000C018
TMR16B0MR1:
	dd 0x4000C01C
