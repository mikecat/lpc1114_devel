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
irq16:
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

reset:
	; enable clock for I/O configuration block
	LDR R0, SYSAHBCLKCTRL
	LDR R1, [R0]
	MOVS R2, #1
	LSLS R2, #16
	ORRS R1, R2
	STR R1, [R0]
	; select RXD, pull-down/pull-up inactive
	LDR R0, IOCON_PIO1_6
	MOVS R1, #0xC1
	STR R1, [R0]
	; select TXD, pull-down/pull-up inactive
	LDR R0, IOCON_PIO1_7
	STR R1, [R0]

	; disable clock for I/O contfiguration block, enable clock for UART
	LDR R0, SYSAHBCLKCTRL
	LDR R1, [R0]
	MOVS R2, #1
	LSLS R2, #16
	BICS R1, R2
	LSRS R2, #4
	ORRS R1, R2
	STR R1, [R0]
	; set UART clock to divide by 1
	LDR R0, UARTCLKDIV
	MOVS R1, #1
	STR R1, [R0]
	; enable FIFOs
	LDR R0, U0FCR
	MOVS R1, #1
	STR R1, [R0]
	; word length = 8bit, stop bit = 1, parity disabled, access divisor latches
	LDR R0, U0LCR
	MOVS R1, #0x83
	STR R1, [R0]
	; divisor LSB = 71
	LDR R0, U0DLL
	MOVS R1, #71
	STR R1, [R0]
	; divisor MSB = 0
	LDR R0, U0DLM
	MOVS R1, #0
	STR R1, [R0]
	; disable access divisor latches
	LDR R0, U0LCR
	MOVS R1, #0x03
	STR R1, [R0]
	; fractional divaddval = 1, mulval = 10
	LDR R0, U0FDR
	MOVS R1, #0xA1
	STR R1, [R0]

mainloop:
	; wait until some data is read
	LDR R0, U0LSR
	MOVS R2, #1
wait_for_input:
	LDR R1, [R0]
	TST R1, R2
	BEQ wait_for_input
	; read the data
	LDR R0, U0RBR
	LDR R3, [R0]

	; if the data is 0x21 <= data <= 0x7D, add one to the data
	CMP R3, #0x21
	BLO no_increment
	CMP R3, #0x7D
	BHI no_increment
	ADDS R3, #1
no_increment:

	; wait until data can be written
	LDR R0, U0LSR
	MOVS R2, #0x20 ; check for THRE
wait_for_output:
	LDR R1, [R0]
	TST R1, R2
	BEQ wait_for_output
	; write the data
	LDR R0, U0THR
	STR R3, [R0]

	B mainloop

	align 4
SYSAHBCLKCTRL:
	dd 0x40048080
UARTCLKDIV:
	dd 0x40048098
IOCON_PIO1_6:
	dd 0x400440A4
IOCON_PIO1_7:
	dd 0x400440A8
U0RBR:
U0THR:
U0DLL:
	dd 0x40008000
U0DLM:
	dd 0x40008004
U0FCR:
	dd 0x40008008
U0LCR:
	dd 0x4000800C
U0LSR:
	dd 0x40008014
U0FDR:
	dd 0x40008028
