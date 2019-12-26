CC				 := i686-linux-gnu-gcc-8
AS				 := i686-linux-gnu-as
CCFLAGS		 := -g -ffreestanding -Wall -Wextra -Werror -Isrc -lgcc -nostartfiles -fno-pie
LDFLAGS	   := -g -ffreestanding -nostdlib -nostartfiles

BUILDDIR	 := build
SRCDIR		 := src
ISODIR		 := $(BUILDDIR)
SOURCES		 := $(shell find $(SRCDIR) -type f -name "*.c")
ARCH			 := i386
ARCHDIR		 := $(SRCDIR)
OS.BIN		 := $(BUILDDIR)/os.bin
BOOT.S		 := $(ARCHDIR)/boot.s
BOOT.O		 := $(BUILDDIR)/boot.o
LINKER.LD	 := $(ARCHDIR)/linker.ld
OBJS			 := $(addprefix $(BUILDDIR)/, $(notdir $(patsubst %.c, %.o, $(SOURCES)))) $(BOOT.O)

.PHONY = all build objs link run clean

all: $(OS.BIN) run

$(OS.BIN): build

build: $(BOOT.O) objs link

$(BOOT.O): $(BOOT.S)
	mkdir -p $(BUILDDIR)

	$(AS) $(BOOT.S) -o $(BOOT.O)

objs:
	for f in $$(echo "$(SOURCES)" | tr -s " " "\012"); do $(CC) -c $$f -o $$(echo "build/$$(basename $$f | sed "s/.c/.o/g")") $(CCFLAGS) || exit; done;

link: $(LINKER.LD)
	$(CC) -T $(LINKER.LD) -o $(OS.BIN) $(LDFLAGS) $(OBJS)

setup_disk: $(OS.BIN)
	mkdir -p $(ISODIR)/boot/grub

	cp $(OS.BIN) $(ISODIR)/boot
	cp grub.cfg $(ISODIR)/boot/grub

run: setup_disk
	qemu-system-i386 -kernel $(ISODIR)/boot/os.bin -serial stdio
 
debug: setup_disk
	gdb --command=debug.gdb

clean:
	rm -rf $(BUILDDIR)/*
