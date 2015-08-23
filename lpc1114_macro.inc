; A macro file for writing program for LPC1114FN28/102

; The MIT License (MIT)
;
; Copyright (c) 2015 MikeCAT
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.

; References:
; UM10398 LPC111x/LPC11Cxx User manual
; http://www.nxp.com/documents/user_manual/UM10398.pdf
; The Thumb instruction set
; http://apt.cs.manchester.ac.uk/ftp/pub/apt/peve/PEVE05/Slides/05_Thumb.pdf
; 16/32-Bit ARM-Thumb Architecture and AX Extensions
; http://ww2.cs.fsu.edu/~kulkarni/thumb_ax.pdf
; Thumb Cramsheet v2
; http://www.coranac.com/files/gba/re-ejected-thumbref2.pdf

%define SP R13
%define LR R14
%define PC R15

; these checkers are disabled to avoid "not defined before use" errors

; Usage: checkbits number, bit
%macro checkbits 2
;%if ((%1) & ((1 << (%2)) - 1)) != 0 && ((~(%1)) & ((1 << (%2)) - 1)) != 0
;%error "value too large"
;%endif
%endmacro

; Usage: checkalign number, bit
%macro checkalign 2
;%if ((%1) & ((1 << (%2)) - 1)) != 0
;%error "invalid alignment"
;%endif
%endmacro

%macro __B_cond 2
%define addr ((%1) - ($ + 4))
	checkbits addr, 9
	checkalign addr, 1
	dw 0xD000 | (((%2) & 0x0F) << 8) | ((addr >> 1) & 0xFF)
%undef addr
%endmacro

%macro BEQ 1
	__B_cond %1, 0x0
%endmacro

%macro BNE 1
	__B_cond %1, 0x1
%endmacro

%macro BCS 1
	__B_cond %1, 0x2
%endmacro

%macro BCC 1
	__B_cond %1, 0x3
%endmacro

%macro BHS 1
	__B_cond %1, 0x2
%endmacro

%macro BLO 1
	__B_cond %1, 0x3
%endmacro

%macro BMI 1
	__B_cond %1, 0x4
%endmacro

%macro BPL 1
	__B_cond %1, 0x5
%endmacro

%macro BVS 1
	__B_cond %1, 0x6
%endmacro

%macro BVC 1
	__B_cond %1, 0x7
%endmacro

%macro BHI 1
	__B_cond %1, 0x8
%endmacro

%macro BLS 1
	__B_cond %1, 0x9
%endmacro

%macro BGE 1
	__B_cond %1, 0xA
%endmacro

%macro BLT 1
	__B_cond %1, 0xB
%endmacro

%macro BGT 1
	__B_cond %1, 0xC
%endmacro

%macro BLE 1
	__B_cond %1, 0xD
%endmacro

%macro BAL 1
	__B_cond %1, 0xE
%endmacro

%macro B 1
%define addr ((%1) - ($ + 4))
	checkbits addr, 12
	checkalign addr, 1
	dw 0xE000 | ((addr >> 1) & 0x7FF)
%undef addr
%endmacro

%macro BL 1
%define addr ((%1) - ($ + 4))
	checkbits addr, 23
	checkalign addr, 1
	dw 0xF000 | ((addr >> 12) & 0x7FF)
	dw 0xF800 | (((addr + 2) >> 1) & 0x7FF)
%undef addr
%endmacro

%macro __BX_BLX 3
%defstr str1 %1
%substr p1 str1 1
%substr s1 str1 2, -1
%deftok s1n s1
%ifidn p1, 'R'
	checkbits (s1n), 4
	dw 0x4700 | (((%3) & 1) << 7) | (((s1n) & 0xF) << 3)
%else
	%error First argument of %2 is not a regsiter.
%endif
%undef str1
%undef p1
%undef s1
%undef s1s
%endmacro

%macro BX 1
	__BX_BLX %1, BX, 0
%endmacro

%macro BLX 1
%defstr str1 %1
%substr prefix str1 1
%ifidn prefix, 'R'
	__BX_BLX %1, BLX, 1
%else
	%warning "BLX label doesn't seem to be in UM10398"
%define addr ((%1) - ($ + 4))
	checkbits addr, 23
	checkalign addr, 1
	dw 0xF000 | ((addr >> 12) & 0x7FF)
	dw 0xE800 | (((addr + 2) >> 1) & 0x7FF)
%undef addr
%endif
%undef str1
%undef prefix
%endmacro

%macro SWI 1
	checkbits (%1), 8
	dw 0xDF00 | ((%1) & 0xFF)
	%warning "SWI doesn't seem to be in UM10398"
%endmacro

%macro __data_process_12 5
%substr p1 %1 1
%substr s1 %1 2, -1
%substr p2 %2 1
%substr s2 %2 2, -1
%substr p3 %3 1
%substr s3 %3 2, -1
%deftok s1n s1
%deftok s2n s2
%deftok s3n s3
%ifidn p1, 'R'
%ifidn p2, 'R'
	checkbits (s1n), 3
	checkbits (s2n), 3
%ifidn p3, 'R'
	checkbits (s3n), 3
	dw 0x1800 | (((%5) & 1) << 9) | (((s3n) & 7) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%else
%ifidn p3, '#'
	checkbits (s3n), 3
	dw 0x1C00 | (((%5) & 1) << 9) | (((s3n) & 7) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%else
%error Third argument of %4 is neither a regsiter nor an immediate value.
%endif
%endif
%else
%error Second argument of %4 is not a regsiter.
%endif
%else
%error First argument of %4 is not a regsiter.
%endif
%undef p1
%undef s1
%undef p2
%undef s2
%undef p3
%undef s3
%undef s1n
%undef s2n
%undef s3n
%endmacro

%macro ADDS 3
	__data_process_12 %1, %2, %3, ADDS, 1
%endmacro

%macro SUBS 3
	__data_process_12 %1, %2, %3, SUBS, 0
%endmacro

%macro __data_process_4 5
%defstr str1 %1
%defstr str2 %2
%defstr str3 %3
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -1
%substr p3 str3 1
%substr s3 str3 2, -1
%deftok s1n s1
%deftok s2n s2
%deftok s3n s3
%ifidn p1, 'R'
%ifidn p2, 'R'
%ifidn p3, '#'
	checkbits (s1n), 3
	checkbits (s2n), 3
	checkbits (s3n), 5
	dw 0x0000 | (((%5) & 3) << 11) | (((s3n) & 0x1F) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%else
%error Third argument of %4 is not an immediate value.
%endif
%else
%error Second argument of %4 is not a regsiter.
%endif
%else
%error First argument of %4 is not a regsiter.
%endif
%undef str1
%undef str2
%undef str3
%undef p1
%undef s1
%undef p2
%undef s2
%undef p3
%undef s3
%undef s1n
%undef s2n
%undef s3n
%endmacro

%macro LSLS 3
	__data_process_4 %1, %2, %3, LSLS, 0
%endmacro

%macro LSRS 3
	__data_process_4 %1, %2, %3, LSRS, 1
%endmacro

%macro ASRS 3
	__data_process_4 %1, %2, %3, ASRS, 2
%endmacro

%macro __data_process_3 4
%defstr str1 %1
%defstr str2 %2
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -1
%deftok s1n s1
%deftok s2n s2
%ifidn p1, 'R'
%ifidn p2, '#'
	checkbits (s1n), 3
	checkbits (s2n), 8
	dw 0x2000 | (((%4) & 3) << 11) | (((s1n) & 7) << 8) | ((s2n) & 0xFF)
%else
%error Second argument of %3 is not an immediate value.
%endif
%else
%error First argument of %3 is not a regsiter.
%endif
%undef str1
%undef str2
%undef p1
%undef s1
%undef p2
%undef s2
%undef s1n
%undef s2n
%endmacro

%macro __data_process_5 4
%defstr str1 %1
%defstr str2 %2
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -1
%deftok s1n s1
%deftok s2n s2
%ifidn p1, 'R'
%ifidn p2, 'R'
	checkbits (s1n), 3
	checkbits (s2n), 3
	dw 0x4000 | (((%4) & 0xF) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%else
%error Second argument of %3 is not a register.
%endif
%else
%error First argument of %3 is not a regsiter.
%endif
%undef str1
%undef str2
%undef p1
%undef s1
%undef p2
%undef s2
%undef s1n
%undef s2n
%endmacro

%macro MOVS 2
	__data_process_3 %1, %2, MOVS, 0
%endmacro

%macro ADDS 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
	ADDS %1, %1, %2
%else
	__data_process_3 %1, %2, ADDS, 2
%endif
%undef str2
%undef prefix
%endmacro

%macro SUBS 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
	SUBS %1, %1, %2
%else
	__data_process_3 %1, %2, SUBS, 3
%endif
%undef str2
%undef prefix
%endmacro

%macro ANDS 2
	__data_process_5 %1, %2, ANDS, 0x0
%endmacro

%macro EORS 2
	__data_process_5 %1, %2, EORS, 0x1
%endmacro

%macro LSLS 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
	__data_process_5 %1, %2, LSLS, 0x2
%else
	LSLS %1, %1, %2
%endif
%undef str2
%undef prefix
%endmacro

%macro LSRS 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
	__data_process_5 %1, %2, LSRS, 0x3
%else
	LSRS %1, %1, %2
%endif
%undef str2
%undef prefix
%endmacro

%macro ASRS 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
	__data_process_5 %1, %2, ASRS, 0x4
%else
	ASRS %1, %1, %2
%endif
%undef str2
%undef prefix
%endmacro

%macro ADCS 2
	__data_process_5 %1, %2, ADCS, 0x5
%endmacro

%macro SBCS 2
	__data_process_5 %1, %2, SBCS, 0x6
%endmacro

%macro RORS 2
	__data_process_5 %1, %2, RORS, 0x7
%endmacro

%macro TST 2
	__data_process_5 %1, %2, TST, 0x8
%endmacro

%macro NEG 2
	__data_process_5 %1, %2, NEG, 0x9
	%warning "NEG Rd, Rm doesn't seem to be in UM10398"
%endmacro

%macro CMN 2
	__data_process_5 %1, %2, CMN, 0xB
%endmacro

%macro ORRS 2
	__data_process_5 %1, %2, ORRS, 0xC
%endmacro

%macro MULS 2
	__data_process_5 %1, %2, MULS, 0xD
%endmacro

%macro BICS 2
	__data_process_5 %1, %2, BICS, 0xE
%endmacro

%macro MVNS 2
	__data_process_5 %1, %2, MVNS, 0xF
%endmacro

%macro __data_process_6 4
%defstr str1 %1
%defstr str2 %2
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -1
%deftok s1n s1
%deftok s2n s2
%ifidn p1, 'R'
%ifidn p2, 'R'
	checkbits (s1n), 4
	checkbits (s2n), 4
	dw 0x4200 | (((%4) & 3) << 8) | (((s1n) & 0x8) << 4) | (((s2n) & 0xF) << 3) | ((s1n) & 7)
%else
%error Second argument of %3 is not a regsiter.
%endif
%else
%error First argument of %3 is not a regsiter.
%endif
%undef str1
%undef str2
%undef p1
%undef s1
%undef p2
%undef s2
%undef s1n
%undef s2n
%endmacro

%macro __data_process_8 3
%defstr str1 %1
%substr p1 str1 1
%substr s1 str1 2, -1
%deftok s1n s1
%ifidn p1, '#'
	checkbits (s1n), 9
	checkalign (s1n), 2
	dw 0xB000 | (((%3) & 1) << 7) | ((((s1n) >> 2) & 0x7F)
%else
%error Second argument of %2 is not an immediate value.
%endif
%undef str1
%undef p1
%undef s1
%undef s1n
%endmacro

%macro ADD 2
%defstr str2 %2
%substr check str2 1
%ifidn check, '#'
%ifidn %1, R13
	__data_process_8 %2, ADD, 1
%else
%error invalid instruction
%endif
%else
	__data_process_6 %1, %2, ADD, 0
%endif
%undef str2
%undef check
%endmacro

%macro CMP 2
%defstr str2 %2
%substr prefix str2 1
%ifidn prefix, 'R'
%defstr str1 %1
%substr p1 str1 1
%substr s1 str1 2, -1
%substr s2 str2 2, -1
%deftok s1n s1
%deftok s2n s2
%ifidn p1, 'R'
%if (s1n) >= 8 || (s2n) >= 8
	__data_process_6 %1, %2, CMP, 1
%else
	__data_process_5 %1, %2, CMP, 0xA
%endif
%else
%error First argumet of CMP is not a register.
%endif
%undef str1
%undef p1
%undef s1
%undef s2
%undef s1n
%undef s2n
%else
	__data_process_3 %1, %2, CMP, 1
%endif
%undef str2
%undef prefix
%endmacro

%macro MOV 2
	__data_process_6 %1, %2, MOV, 2
%endmacro

%macro SUB 2
%defstr str2 %2
%substr check str2 1
%ifidn check, '#'
%ifidn %1, R13
	__data_process_8 %2, SUB, 0
%else
%error invalid arguments
%endif
%else
%error invalid argument
%endif
%undef str2
%undef check
%endmacro

%macro ADD 3
%defstr str1 %1
%defstr str2 %2
%defstr str3 %3
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -1
%substr p3 str3 1
%substr s3 str3 2, -1
%deftok s1n s1
%deftok s2n s2
%deftok s3n s3
%ifidn p1, 'R'
%ifidn p2, 'R'
%ifidn p3, '#'
%if (s2n == 13) || (s2n == 15)
	chechbit (s1n), 3
	checkbit (s3n), 10
	checkalign (s3n), 2
	dw 0xA000 | (((s2n) & 2) << 10) | (((s1n) & 7) << 8) | ((((s3n) >> 2) & 0xFF)
%else
%error invalid argument
%endif
%else
%error Third argument of ADD is not an immediate value.
%endif
%else
%error Second argument of ADD is not a regsiter.
%endif
%else
%error First argument of ADD is not a regsiter.
%endif
%undef str1
%undef str2
%undef str3
%undef p1
%undef s1
%undef p2
%undef s2
%undef p3
%undef s3
%undef s1n
%undef s2n
%undef s3n
%endmacro

%macro REV 2
%error REV Rd,Rn is not implemented
%endmacro

%macro REV16 2
%error REV16 Rd,Rn is not implemented
%endmacro

%macro REVSH 2
%error REVSH Rd,Rn is not implemented
%endmacro

%macro SXTB 2
%error SXTB Rd,Rm is not implemented
%endmacro

%macro SXTH 2
%error SXTH Rd,Rm is not implemented
%endmacro

%macro UXTB 2
%error UXTB Rd,Rm is not implemented
%endmacro

%macro UXTH 2
%error UXTH Rd,Rm is not implemented
%endmacro

%macro __single_register_data_transfer_123 7
%defstr str1 %1
%defstr str2 %2
%defstr str3 %3
%substr p1 str1 1
%substr s1 str1 2, -1
%substr pp2 str2 1
%substr p2 str2 2
%substr s2 str2 3, -1
%substr p3 str3 1
%substr s3 str3 2, -2
%strlen str3len str3
%substr ss3 str3 str3len
%undef str3len
%deftok s1n s1
%deftok s2n s2
%deftok s3n s3
%ifidn pp2, '['
%ifidn ss3, ']'
%ifidn p1, 'R'
%ifidn p2, 'R'
	checkbits (s1n), 3
	checkbits (s2n), 3
%ifidn p3, 'R'
	checkbits (s3n), 3
	dw 0x5000 | (((%7) & 7) << 9) | (((s3n) & 7) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%else
%ifidn p3, '#'
%if %5 == -1
%error invalid instruction
%else
	checkbits (s3n), 5 + (%6)
	checkalign (s3n), (%6)
	dw (((%5) & 0x1F) << 11) | ((((s3n) >> (%6)) & 0x1F) << 6) | (((s2n) & 7) << 3) | ((s1n) & 7)
%endif
%else
%error Third argument of %4 is neither a regsiter nor an immediate value.
%endif
%endif
%else
%error Second argument of %4 is not a regsiter.
%endif
%else
%error First argument of %4 is not a regsiter.
%endif
%else
%error invalid format
%endif
%else
%error invalid format
%endif
%undef str1
%undef str2
%undef str3
%undef p1
%undef s1
%undef pp2
%undef p2
%undef s2
%undef p3
%undef s3
%undef ss3
%undef s1n
%undef s2n
%undef s3n
%endmacro

%macro __single_register_data_transfer_45 4
%defstr str1 %1
%defstr str2 %2
%substr p1 str1 1
%substr s1 str1 2, -1
%substr p2 str2 1
%substr s2 str2 2, -2
%strlen str2len str2
%substr ss2 str2 str2len
%undef str2len
%deftok s1n s1
%deftok s2n s2
%ifidn ss2, ']'
%ifidn p1, 'R'
%ifidn p2, '#'
	checkbits (s1n), 3
	checkbits (s2n), 10
	checkalign (s2n), 4
	dw (((%4) & 0x1F) << 11) | (((s1n) & 7) << 8) | (((s2n) >> 2) & 0xff)
%else
%error Third argument of %3 is not an immediate value.
%endif
%else
%error First argument of %3 is not a regsiter.
%endif
%else
%error invalid format
%endif
%undef str1
%undef str2
%undef p1
%undef s1
%undef p2
%undef s2
%undef ss2
%undef s1n
%undef s2n
%endmacro

%macro LDR 3
%ifidn %2, [R15
	__single_register_data_transfer_45 %1, %3, LDR, 0x09
%else
%ifidn %2, [R13
	__single_register_data_transfer_45 %1, %3, LDR, 0x13
%else
	__single_register_data_transfer_123 %1, %2, %3, LDR, 0x0D, 2, 0
%endif
%endif
%endmacro

%macro STR 3
%ifidn %2, [R13
	__single_register_data_transfer_45 %1, %3, STR, 0x12
%else
	__single_register_data_transfer_123 %1, %2, %3, STR, 0x0C, 2, 4
%endif
%endmacro

%macro LDRB 3
	__single_register_data_transfer_123 %1, %2, %3, LDRB, 0x0F, 0, 2
%endmacro

%macro STRB 3
	__single_register_data_transfer_123 %1, %2, %3, STRB, 0x0E, 0, 6
%endmacro

%macro LDRH 3
	__single_register_data_transfer_123 %1, %2, %3, LDRH, 0x11, 1, 5
%endmacro

%macro STRH 3
	__single_register_data_transfer_123 %1, %2, %3, STRH, 0x10, 1, 1
%endmacro

%macro LDRSB 3
	__single_register_data_transfer_123 %1, %2, %3, LDRSB, -1, 0, 3
%endmacro

%macro LDRSH 3
	__single_register_data_transfer_123 %1, %2, %3, LDRSH, -1, 1, 7
%endmacro

%macro LDR 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	LDR %1, prefix, #0]
%undef prefix
%else
%defstr str1 %1
%substr p1 str1 1
%substr s1 str1 2, -1
%deftok s1n s1
%ifidn p1, 'R'
%define addr ((((%2) - ($ + 4)) + 3) & ~3)
	checkbits (s1n), 3
	checkbits addr, 10
	checkalign addr, 2
	dw 0x4800 | (((s1n) & 0x07) << 8) | ((addr >> 2) & 0xFF)
%undef addr
%else
%error First argument of LDR is not a regsiter.
%endif
%undef str1
%undef p1
%undef s1
%undef s1n
%endif
%undef str2
%undef suffix
%endmacro

%macro LDRB 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	LDRB %1, prefix, #0]
%undef prefix
%else
%error invalid operands
%endif
%undef str2
%undef suffix
%endmacro

%macro LDRH 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	LDRH %1, prefix, #0]
%undef prefix
%else
%error invalid operands
%endif
%undef str2
%undef suffix
%endmacro

%macro STR 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	STR %1, prefix, #0]
%undef prefix
%else
%error invalid operands
%endif
%undef str2
%undef suffix
%endmacro

%macro STRB 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	STRB %1, prefix, #0]
%undef prefix
%else
%error invalid operands
%endif
%undef str2
%undef suffix
%endmacro

%macro STRH 2
%defstr str2 %2
%strlen str2len str2
%substr suffix str2 str2len
%undef str2len
%ifidn suffix, ']'
%substr prefix str2 1, -2
	STRH %1, prefix, #0]
%undef prefix
%else
%error invalid operands
%endif
%undef str2
%undef suffix
%endmacro

%macro __multiple_register_data_transfer 5
%defstr str1 %1
%substr p1 str1 1
%strlen str1len str1
%substr ss1 str1 str1len
%undef str1len
%ifidn ss1, '!'
%substr s1 str1 2, -2
%define writeback_suffix 1
%else
%substr s1 str1 2, -1
%define writeback_suffix 0
%endif
%deftok s1n s1
%assign reglist 0
%define r_id %5
%macro read_reglist 0-*
%rep %0
%defstr tempstr %1
%substr tp tempstr 1
%substr ts tempstr 2, -1
%deftok tsn ts
%undef tempstr
%ifidn tp, 'R'
%if 0 <= (tsn) && (tsn) <= 7
%assign reglist reglist|(1<<(tsn))
%elif r_id != -1 && (tsn) == r_id
%assign reglist reglist|0x0100
%else
%error invalid register
%endif
%else
%error invalid argument
%endif
%undef tp
%undef ts
%undef tsn
%rotate 1
%endrep
%endmacro
	read_reglist %2
%if r_id == -1 && (((%4) == 0x19 && (writeback_suffix ^^ (reglist & (1 << (s1n))) == 0)) || ((%4) != 0x19 && !writeback_suffix))
%error invalid arguments
%endif
	checkbits (s1n), 3
	dw (((%4) & 0x1F) << 11) | (((s1n) & 7) << 8) | reglist
%undef str1
%undef p1
%undef s1
%undef ss1
%undef s1n
%undef writeback_suffix
%undef reglist
%undef r_id
%unmacro read_reglist 0-*
%endmacro

%macro LDMIA 2
	__multiple_register_data_transfer %1, {%2}, LDMIA, 0x19, -1
%endmacro

%macro STMIA 2
	__multiple_register_data_transfer %1, {%2}, LDMIA, 0x18, -1
%endmacro

%macro POP 1
	__multiple_register_data_transfer R4, {%1}, POP, 0x17, 15
%endmacro

%macro PUSH 1
	__multiple_register_data_transfer R4, {%1}, PUSH, 0x16, 14
%endmacro

%macro BKPT 1
%defstr str1 %1
%substr p1 str1 1
%strlen str1len str1
%substr s1 str1 2, str1len
%undef str1len
%deftok s1n s1
%ifidn p1, '#'
	checkbits (s1n), 8
	dw 0xBF00 | ((s1n) & 0xFF)
%else
%error Third argument of %4 is not an immediate value.
%endif
%undef str1
%undef p1
%undef s1
%undef s1n
%endmacro

%macro CPSID 1
%error CPSID i is not implemented
%endmacro

%macro CPSIE 1
%error CPSIE i is not implemented
%endmacro

%macro DMB 0
%error DMB is not implemented
%endmacro

%macro DSB 0
%error DSB is not implemented
%endmacro

%macro ISB 0
%error ISB is not implemented
%endmacro

%macro MRS 2
%error MRS Rd,spec_reg is not implemented
%endmacro

%macro MSR 2
%error MSR spec_reg,Rn is not implemented
%endmacro

%macro NOP 0
%error NOP is not implemented
%endmacro

%macro SEV 0
%error SEV is not implemented
%endmacro

%macro SVC 1
%error SVC #imm is not implemented
%endmacro

%macro WFE 0
%warning The WFE instruction is not implemented on the LPC111x/LPC11Cxx, according to UM10398
%error WFE is not implemented
%endmacro

%macro WFI 0
%error WFI is not implemented
%endmacro
