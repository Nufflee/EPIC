BITS 32

%define SYS_WRITE 4
%define STDOUT 1

org PROCESS_BASE_ADDRESS

; ebp            [ebp + 0]
; return address [ebp + 4]
; argc           [ebp + 8]
; argv[i]        [ebp + (12 + i * 4)]

section .text
global _start
_start:
  push ebp ; save caller's ebp
  mov ebp, esp

  mov ecx, [ebp + 8] ; argc
  cmp ecx, 1
  jbe _start_end

  mov ecx, [ebp + 16] ; argv[1]
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
  mov eax, 69

  pop ebp ; restore caller's ebp
  ret

; if (*edx == NULL) return;

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