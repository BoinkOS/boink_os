all: bootloader.bin

bootloader.bin: boot/bootloader.asm
	nasm -f bin boot/bootloader.asm -o bootloader.bin

run: bootloader.bin
	qemu-system-i386 -drive format=raw,file=bootloader.bin

clean:
	rm -f *.bin *.img *.iso