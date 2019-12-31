ARCH    := i386

CC      := i686-linux-gnu-gcc-8
AS      := i686-linux-gnu-as
CCFLAGS := -g -ffreestanding -Wall -Wextra -Werror -Isrc -lgcc -nostartfiles -fno-pie
LDFLAGS := -g -ffreestanding -nostdlib -nostartfiles

# Directories
BUILD_DIR	:= build
SRC_DIR		:= src
ISO_DIR		:= isodir
ARCH_DIR    := $(SRC_DIR)

SOURCES		:= $(shell find $(SRCDIR) -type f -name "*.c")
OS.BIN		:= $(BUILD_DIR)/os.bin
BOOT.S		:= $(ARCH_DIR)/boot.s
BOOT.O		:= $(BUILD_DIR)/boot.o
LINKER.LD	:= $(ARCH_DIR)/linker.ld
OBJS        := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(SOURCES)))) $(BOOT.O)

QEMU        := qemu-system-$(ARCH)
QEMU_FLAGS  := -serial stdio

.PHONY = all build objs link run clean

all: $(OS.BIN) run

$(OS.BIN): build

build: $(BOOT.O) objs link

$(BOOT.O): $(BOOT.S)
	mkdir -p $(BUILD_DIR)
	
	$(AS) $(BOOT.S) -o $(BOOT.O)

objs:
	for f in $$(echo "$(SOURCES)" | tr -s " " "\012"); do $(CC) -c $$f -o $$(echo "build/$$(basename $$f | sed "s/\.c/\.o/g")") $(CCFLAGS) || exit; done;

link: $(LINKER.LD)
	$(CC) -T $(LINKER.LD) -o $(OS.BIN) $(LDFLAGS) $(OBJS)

setup_disk: $(OS.BIN)
	mkdir -p $(ISO_DIR)/boot/grub
	
	cp $(OS.BIN) $(ISO_DIR)/boot
	cp grub.cfg $(ISO_DIR)/boot/grub

run: setup_disk
	$(QEMU) -kernel $(ISO_DIR)/boot/os.bin $(QEMU_FLAGS)
 
debug: setup_disk
	gdb --command=debug.gdb

iso: setup_disk
	grub-mkrescue -o os.iso $(ISO_DIR)

run_iso: iso
	$(QEMU) -cdrom os.iso $(QEMU_FLAGS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)
	
	rm os.iso 
