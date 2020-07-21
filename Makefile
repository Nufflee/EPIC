ARCH         := i386

CC           := i686-linux-gnu-gcc-8
AS           := i686-linux-gnu-as
LD           := i686-linux-gnu-ld
CCFLAGS      := -g -ffreestanding -Wall -Wextra -Isrc -Isrc/libc -lgcc -nostartfiles -fno-pie -MMD -MP -march=$(ARCH)
LDFLAGS      := -g -ffreestanding -nostdlib -nostartfiles

BUILD_DIR    := build
SRC_DIR      := src
ISO_DIR      := isodir
ARCH_DIR     := $(SRC_DIR)
ROOT_DIR     := root
USERLAND_DIR := src/userland

SOURCES      := $(shell find $(SRC_DIR) -path "src/userland/*" -prune -o -name '*.c' -print)
OS.BIN       := $(BUILD_DIR)/os.bin

USERLAND_ASM := $(shell find $(USERLAND_DIR) -type f -name "*.asm")
USERLAND     := $(addprefix $(ROOT_DIR)/, flat elf32asm test)
USERLAND_CC  := toolchain/i686-epic-musl-gcc

BOOT.S       := $(SRC_DIR)/boot.s
LINKER.LD    := $(ARCH_DIR)/linker.ld
OBJS         := $(addprefix $(BUILD_DIR)/,$(SOURCES:.c=.c.o) $(BOOT.S:.s=.s.o))
DEPS         := $(OBJS:.o=.d)

QEMU         := qemu-system-$(ARCH)
QEMU_FLAGS   := -kernel $(ISO_DIR)/boot/os.bin -drive file=$(ISO_DIR)/boot/drive.img,format=raw -m 256 -cpu 486

.PHONY = all build objs link run clean

all: $(OS.BIN)

$(OS.BIN): build
	grub-file --is-x86-multiboot $(OS.BIN)

build: link $(USERLAND)

link: $(LINKER.LD) $(OBJS)
	$(CC) -Wl,-T$(LINKER.LD) -o $(OS.BIN) $(LDFLAGS) $(OBJS)

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CCFLAGS)

$(ROOT_DIR)/flat: $(USERLAND_DIR)/hello_world.asm
	nasm -DFLAT -DBASE_ADDRESS=0x90000 -f bin $< -o $@

$(ROOT_DIR)/elf32asm: $(USERLAND_DIR)/hello_world.asm
	$(eval OUT := $(BUILD_DIR)/$@.o)

	@mkdir -p $(dir $(OUT))

	nasm -f elf32 $< -o $(OUT)
	$(LD) $(OUT) -o $@

$(ROOT_DIR)/test: $(USERLAND_DIR)/test.c
	$(USERLAND_CC) -static $< -o $@

setup_disk: $(OS.BIN)
	mkdir -p $(ISO_DIR)/boot/grub

	cp $(OS.BIN) $(ISO_DIR)/boot
	cp grub.cfg $(ISO_DIR)/boot/grub

	python3 ./tools/mkfs.epic.py ./root/ $(ISO_DIR)/boot/drive.img -f

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
	-rm -rf $(USERLAND)

	-rm -rf os.iso

-include $(DEPS)