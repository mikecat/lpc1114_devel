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
	; disable pullup / pulldown on PIO0_3
	LDR R7, iocon_pio0_3_addr
	LDR R6, [R7, #0]
	LDR R5, iocon_pio0_3_mask
	ANDS R6, R5
	STR R6, [R7, #0]
	; set PIO0_3 as output
	LDR R7, pio0_dir_addr
	MOVS R6, #8
	STR R6, [R7, #0]
	; output to PIO0_3
	LDR R7, pio0_3_addr
mainloop:
	MOVS R6, #8
	STR R6, [R7, #0]
	BL busyloop
	MOVS R6, #0
	STR R6, [R7, #0]
	BL busyloop
	B mainloop

	align 4
iocon_pio0_3_addr:
	dd 0x4004402C
iocon_pio0_3_mask:
	dd 0xFFFFFBC0 ; FUNC=0x0, MODE=0x0, HYS=0, OD=0
pio0_3_addr:
	dd 0x50000020
pio0_dir_addr:
	dd 0x50008000

busyloop:
	PUSH {R7}
	LDR R7, busyloop_num
busyloop_loop:
	SUBS R7, #1
	BPL busyloop_loop
	POP {R7}
	BX LR
	align 4
busyloop_num:
	dd 1500000

	align 4
