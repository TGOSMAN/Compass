

.section .text
	.equ FIR0, 0x00000750
	.equ FIR1, 0x00001592
	.equ FIR2, 0x00002251
	.equ FIR3, 0x00003138
	.equ FIR4, 0x00002251
	.equ FIR5, 0x00001592
	.equ FIR6, 0x00000750
__FIR:
	add r1, r2, r1
	
	blx r0
	__multipy:

.end 