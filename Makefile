ARCH        := i386

CC          := i686-linux-gnu-gcc-8
AS          := i686-linux-gnu-as
CCFLAGS     := -g -ffreestanding -Wall -Wextra -Isrc -Isrc/libc -lgcc -nostartfiles -fno-pie
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
QEMU_FLAGS  := -kernel $(ISO_DIR)/boot/os.bin -drive file=$(ISO_DIR)/boot/drive.img,format=raw

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

$(BUILD_DIR)/%.c.o: %.c %.h
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CCFLAGS)

setup_disk: $(OS.BIN)
	mkdir -p $(ISO_DIR)/boot/grub

	cp $(OS.BIN) $(ISO_DIR)/boot
	cp grub.cfg $(ISO_DIR)/boot/grub

	dd if=hello_world.txt of=$(ISO_DIR)/boot/drive.img

run: setup_disk
	$(QEMU) -serial stdio $(QEMU_FLAGS)

debug: setup_disk
	gdb -ex 'file $(ISO_DIR)/boot/os.bin' -ex 'target remote | $(QEMU) -gdb stdio -S $(QEMU_FLAGS)'

iso: setup_disk
	grub-mkrescue -o os.iso $(ISO_DIR)

run_iso: iso
	$(QEMU) -cdrom os.iso $(QEMU_FLAGS)

clean:
	-rm -rf $(BUILD_DIR)
	-rm -rf $(ISO_DIR)

	-rm -rf os.iso