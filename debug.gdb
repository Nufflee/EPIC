file isodir/boot/os.bin
target remote | qemu-system-i386 -gdb stdio -S -kernel isodir/boot/os.bin