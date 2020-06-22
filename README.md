# EPIC

EPIC is a small kernel/OS experiment with the goal of running a precompiled 32-bit Linux ELF executable of `echo` to learn what it takes to make such an OS.

The kernel can currently only run in 32-bit protected mode on an x86 (i386) machines.

Development is streamed on [my Twitch channel](https://www.twitch.tv/nuffleee).

## Features

- Simple memory management (physical page allocator and malloc)
- Serial port output
- VGA text mode driver
- Basic interrupt-driven keyboard driver
- Simple terminal emulator
- ATA harad drive driver
- Custom read-only file system - EPICFS
- Syscalls
- Binary execution (only flat binaries for now)

## Dependencies

- i686 gcc8
- nasm
- make
- python3 (for mkfs.epic tool that generates an EPICFS image)
- qemu (for testing and debugging)
- xorriso (optional, only needed for ISO creation)

## Resources used
- [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page) - Tons of tutorials about anything OS Dev related
- [JamesM's kernel development tutorials](http://www.jamesmolloy.co.uk/tutorial_html/) - This tutorial has [some known issues](https://wiki.osdev.org/James_Molloy%27s_Tutorial_Known_Bugs#Before_you_follow_the_tutorial).
- [Linux x86 Syscall Reference](https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86-32_bit)
- [x86 ISA Reference](https://c9x.me/x86)
- And others...

## Ubuntu Setup

(Note: this also works perfectly on Ubuntu WSL with an X Server for QEMU)

If you're using and Ubuntu / Debian based OS you can just run these commands to do all of the required setup of the dependencies etc.

```sh
sudo apt update
sudo apt install gcc-8-i686-linux-gnu nasm make python3 qemu xorriso
```

## Building

To build just the kernel you can run:

```sh
make build
```

To build the kernel and then create a full cd disk image run (xorriso is needed for this to succeed):

```sh
make iso
```

## Running

There are a couple of different options to test run using qemu you can either build and run the kernel directly by using:

```sh
make run
```

Alternativly you can build a full cd disk image and run qemu off of that using:

```sh
make run_iso
```

Note: All of these options currently also complete of the build steps as well.

## Debugging

Running the kernel in a debugger (gdb) can be done using:

```sh
make debug
```