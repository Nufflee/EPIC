BITS 32

%define SYS_EXIT  1
%define SYS_WRITE 4
%define STDOUT 1

%ifdef FLAT
# Flat binaries need to have an origin (`org`) because they need to contain absolute addresses
org BASE_ADDRESS
%endif

section .text
global _start
_start:
  mov eax, SYS_WRITE
  mov ebx, STDOUT
  mov ecx, buffer
  mov edx, buffer_size
  int 0x80

  mov eax, SYS_EXIT
  mov ebx, 420
  int 0x80

section .data
buffer: db "Hello World!", 10
buffer_size: equ $ - buffer