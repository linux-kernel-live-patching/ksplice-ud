; TEST FILE
; For use with NASM/FASM

[bits 32]

mov	esi, [eax+ecx*4+0x800]
add	ax, [bp]
fadd	dword [eax]
push	dword [eax]
call	eax
call	word near [eax]
call	dword near [eax]
call	word far [eax]
call	dword far [eax]
