# EPIC

EPIC is a small kernel/OS experiment with the goal of running a precompiled 64-bit (or 32-bit) Linux ELF executable of `echo` to learn what it takes to make such an OS.

The kernel can currently only run in 32-bit protected mode on an x86 (i386) machine.

Development is sometimes streamed on [my Twitch channel](https://www.twitch.tv/nuffleee).

## Features

- Simple memory management (physical page allocator and malloc)
- Serial port communication (transmission/TX)
- VGA text mode driver
- Basic interrupt-driven keyboard driver
- Simple terminal emulator
- ATA driver
- Custom read-only file system - EPICFS

## Dependencies

- i686 gcc8
- make
- python3 (for mkfs.epic tool that generates an EPICFS image)
- qemu (for testing and debugging)
- xorriso (optional, only needed for ISO creation)

## Ubuntu Setup

(Note: this also works perfectly on Ubuntu WSL with an X Server for QEMU)

If you're using and Ubuntu / Debian based OS you can just run these commands to do all of the reuqired setup of the dependencies etc.

```sh
sudo apt update
sudo apt install gcc-8-i686-linux-gnu make python3 qemu xorriso
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