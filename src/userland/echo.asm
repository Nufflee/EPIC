BITS 32

%define SYS_EXIT  1
%define SYS_WRITE 4

%define STDOUT 1

org PROCESS_BASE_ADDRESS

; ebp            [ebp + 0]
; argc           [ebp + 4]
; argv[i]        [ebp + (8 + i * 4)]

section .text
global _start
_start:
  push ebp
  mov ebp, esp

  mov ecx, [ebp + 4] ; argc
  cmp ecx, 1
  jbe _start_end

  mov ecx, [ebp + 12] ; argv[1]
  mov edx, ecx

  call strlen

  ; add newline
  mov BYTE [edx], 10
  add edx, 1

  sub edx, ecx

  mov eax, SYS_WRITE
  mov ebx, STDOUT
  ; mov ecx, buffer
  ; mov edx, buffer_size
  int 0x80

_start_end:
  pop ebp

  mov eax, SYS_EXIT
  mov ebx, 69
  int 0x80

; if (*edx == NULL) return;
;
; while(*edx != NULL) edx++;
strlen:
  cmp BYTE [edx], 0
  je strlen_finish

strlen_loop:
  add edx, 1
  cmp BYTE [edx], 0
  jne strlen_loop

strlen_finish:
  ret