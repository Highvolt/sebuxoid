CPU 686
BITS 32

global _asm_wait
global _msec
global _sec

section .data

_msec dd 0
_sec dd 0
_cnt dd 1000

section .text

_asm_wait:
	push eax
	
	inc dword[_msec]
	dec dword[_cnt]
	jnz fim
	inc dword[_sec]
	mov dword[_cnt], 1000
fim:
	mov al, 0x20 ;sinaliza EOI (0x20)...
	out 0x20, al ;para o PIC1 (0x20)

	pop eax
	iretd
END