# === Filenames ===
BOOTLOADER_SRC = boot/bootloader.asm
BOOTLOADER_BIN = bootloader.bin

KERNEL_SRCS = $(wildcard kernel/*.c)
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)
KERNEL_BIN = kernel.bin
LINKER_SCRIPT = link.ld

IMAGE = os-image.img

# === Tools ===
CC = i686-elf-gcc
LD = i386-elf-ld
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

# === Targets ===

all: $(IMAGE)

# build bootloader (pure ASM)
$(BOOTLOADER_BIN): $(BOOTLOADER_SRC) boot/*.asm
	nasm -f bin $(BOOTLOADER_SRC) -o $(BOOTLOADER_BIN)

# build all .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# link all .o files into a flat binary kernel
$(KERNEL_BIN): $(KERNEL_OBJS) $(LINKER_SCRIPT)
	$(LD) -T $(LINKER_SCRIPT) --oformat binary $(KERNEL_OBJS) -o $(KERNEL_BIN)

# build final floppy image with bootloader + kernel
$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) bs=512 seek=1 conv=notrunc

# run in QEMU
run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE)

# clean up all build files
clean:
	rm -f *.bin *.o *.img kernel/*.o
