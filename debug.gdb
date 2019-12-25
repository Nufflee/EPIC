file build/os.bin
target remote | qemu-system-i386 -gdb stdio -S -kernel build/boot/os.bin