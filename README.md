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
- Linux-compatible syscalls
- Flat and EFL32 binary execution
- Simple C program support

## Resources used
- [OSDev Wiki](https://wiki.osdev.org/Expanded_Main_Page) - Tons of tutorials about anything OS Dev related
- [JamesM's kernel development tutorials](http://www.jamesmolloy.co.uk/tutorial_html/) - This tutorial has [some known issues](https://wiki.osdev.org/James_Molloy%27s_Tutorial_Known_Bugs#Before_you_follow_the_tutorial).
- [Linux x86 Syscall Reference](http://shell-storm.org/shellcode/files/syscalls.html)
- [x86 ISA Reference](https://c9x.me/x86)
- And others...

## Building

(Note: this also works perfectly on Ubuntu WSL with an X Server for QEMU)

### Cloning

You have to clone this repository recursively because there are submodules:

```sh
git clone --recurse-submodules https://github.com/Nufflee/epic
```

### Dependencies

- git
- i686 gcc8
- nasm
- make
- python3 (for mkfs.epic tool that generates an EPICFS image)
- qemu (for testing and debugging)
- xorriso (optional, only needed for ISO creation)

```sh
sudo apt install git gcc-8-i686-linux-gnu nasm make python3 qemu xorriso # Ubuntu/Debian
                                                                         # Feel free to add dependency instructions for your distro/OS
```

### Building

Compile [epic-musl](https://github.com/Nufflee/epic-musl):

```sh
bash ./tools/build_musl.sh
```

In case you want to rebuild epic-musl, you have to clean it up first:


```sh
bash ./tools/build_musl.sh clean
```

<br/>

Build the kernel itself:

```sh
make build
```

Or build the kernel and then create a full .iso disk image (xorriso is needed for this to succeed):

```sh
make iso
```

### Running

There are a couple of different options to test run using qemu you can either build and run the kernel directly by using:

```sh
make run
```

<br/>

Alternatively you can build a full cd disk image and run qemu off of that using:

```sh
make run_iso
```

### Debugging

Running the kernel in a debugger (gdb) can be done using:

```sh
make debug
```