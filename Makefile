ARCH        := i386

CC          := i686-linux-gnu-gcc-8
AS          := i686-linux-gnu-as
CCFLAGS     := -g -ffreestanding -Wall -Wextra -Werror -Isrc -Isrc/libc -lgcc -nostartfiles -fno-pie
LDFLAGS     := -g -ffreestanding -nostdlib -nostartfiles

# Directories
BUILD_DIR   := build
SRC_DIR     := src
ISO_DIR     := isodir
ARCH_DIR    := $(SRC_DIR)

SOURCES     := $(shell find $(SRC_DIR) -type f -name "*.c")
OS.BIN      := $(BUILD_DIR)/os.bin

BOOT.S      := $(SRC_DIR)/boot.s
LINKER.LD   := $(ARCH_DIR)/linker.ld
OBJS        := $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.c.o) $(BOOT.S:.s=.s.o))

QEMU        := qemu-system-$(ARCH)
QEMU_FLAGS  := -serial stdio

.PHONY = all build objs link run clean

all: $(OS.BIN)

$(OS.BIN): build
	grub-file --is-x86-multiboot $(OS.BIN)

build: link

link: $(LINKER.LD) $(OBJS)
	$(CC) -Wl,-T$(LINKER.LD) -o $(OS.BIN) $(LDFLAGS) $(OBJS)

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CCFLAGS)

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
	-rm -rf $(BUILD_DIR)
	-rm -rf $(ISO_DIR)

	-rm -rf os.iso