BITS 32

%define SYS_WRITE 4
%define STDOUT 1

section .text
global _start
_start:
  mov eax, SYS_WRITE
  mov ebx, STDOUT
  mov ecx, buffer
  mov edx, buffer_size
  int 0x80

  ret

section .data
buffer: db "Hello World!", 10
buffer_size: equ $ - buffer