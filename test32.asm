; TEST FILE
; For use with NASM/FASM

[bits 64]

; Test Addressing Modes

;	mov	[0x1000], bx
;	mov	[0x10], ax
;	mov	ax, [0x10]
;	mov	byte [bx+si], 0x10
;	mov	byte [bx+si+0x10], 0x10
;	mov	word [bp], 0x10
;	mov	word [bp+di+0x10], 0x10
;	mov	dword [si+0x10], 0x10
;	mov	word [di+0x10], 0x10
; 	mov	dword [bx+0x10], 0x1000
; 	mov	word [bx+0x1000], 0x1000
	mov	[ebx+ecx*4], dword 0x100
	mov	[eax+eax*2], eax
	mov	[edx+esi*8], ebp
	mov	[ecx*4], dword 0x100
	mov	[cs:0x100000], byte 0x10
	mov	[eax+0x10], word 0x10
	mov	[eax+0x10], ax
	mov	[eax+0x1000], ebx
	mov	[ebp+eax], esi
	mov	[ebp+edi+0x100000], esp
	mov	[esp], byte 0x10
	mov	[r12], eax
x:	mov	[r13+r12], eax
	mov	[r8*4], sil
	jmp	x
	jnz	x
	jz	x
	inc	rax
	dec	rax
	mov	[rip+0x200], rax
	mov	rax, qword 0x10000
