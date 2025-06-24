# === Filenames ===
BOOTLOADER_SRC := boot/bootloader.asm
BOOTLOADER_BIN := build/bootloader.bin

KERNEL_SRCS := $(shell find kernel -name '*.c')
KERNEL_OBJS := $(patsubst kernel/%.c, build/%.o, $(KERNEL_SRCS))
IVEC_SRC := kernel/interrupts/interrupt_vectors.asm
IVEC_OBJ := build/interrupts/interrupt_vectors.o

EVEC_SRC := kernel/interrupts/exception_vectors.asm
EVEC_OBJ := build/interrupts/exception_vectors.o

# put main.o first when linking
KERNEL_OBJS_ORDERED := $(filter build/main.o, $(KERNEL_OBJS)) \
					$(filter-out build/main.o, $(KERNEL_OBJS))

KERNEL_BIN := build/kernel.bin
LINKER_SCRIPT := link.ld

IMAGE := os-image.img

# === Tools ===
CC := i686-elf-gcc
LD := i386-elf-ld
CFLAGS := -m32 -ffreestanding -O2 -Wall -Wextra -Isrc
LD_FLAGS := -T $(LINKER_SCRIPT) --oformat binary -nostdlib -e kmain

# === Targets ===

all: $(IMAGE)

# make sure build dir exists
build:
	mkdir -p build

# assemble bootloader
$(BOOTLOADER_BIN): $(BOOTLOADER_SRC) | build
	nasm -f bin $(BOOTLOADER_SRC) -o $(BOOTLOADER_BIN)

# compile C files into object files under /build
build/%.o: kernel/%.c | build
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(IVEC_OBJ): $(IVEC_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(IVEC_SRC) -o $(IVEC_OBJ)

$(EVEC_OBJ): $(EVEC_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(EVEC_SRC) -o $(EVEC_OBJ)

# link kernel with main.o first
$(KERNEL_BIN): $(KERNEL_OBJS_ORDERED) $(IVEC_OBJ) $(EVEC_OBJ) $(LINKER_SCRIPT)
	$(LD) $(LD_FLAGS) $(KERNEL_OBJS_ORDERED) $(IVEC_OBJ) $(EVEC_OBJ) -o $(KERNEL_BIN)

# build bootable image
$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) bs=512 seek=1 conv=notrunc

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE) -hdb testdisk.glfs -vga std

clean:
	rm -rf build *.img