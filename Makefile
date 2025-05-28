# filenames
BOOTLOADER_SRC = boot/bootloader.asm
BOOTLOADER_BIN = bootloader.bin
KERNEL_SRC = kernel/main.c
KERNEL_OBJ = kernel.o
KERNEL_BIN = kernel.bin
LINKER_SCRIPT = link.ld
IMAGE = os-image.img

CC = i686-elf-gcc
LD = i386-elf-ld
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

all: $(IMAGE)

$(BOOTLOADER_BIN): $(BOOTLOADER_SRC) boot/*.asm
	nasm -f bin $(BOOTLOADER_SRC) -o $(BOOTLOADER_BIN)

$(KERNEL_OBJ): $(KERNEL_SRC)
	$(CC) $(CFLAGS) -c $(KERNEL_SRC) -o $(KERNEL_OBJ)

$(KERNEL_BIN): $(KERNEL_OBJ) $(LINKER_SCRIPT)
	$(LD) -T $(LINKER_SCRIPT) --oformat binary $(KERNEL_OBJ) -o $(KERNEL_BIN)

# create 1.44MB floppy image and copy bootloader + kernel into it
$(IMAGE): $(BOOTLOADER_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER_BIN) of=$(IMAGE) conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) bs=512 seek=1 conv=notrunc

run: $(IMAGE)
	qemu-system-i386 -fda $(IMAGE)

clean:
	rm -f *.bin *.o *.img $(KERNEL_OBJ) $(KERNEL_BIN)
