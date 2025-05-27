bootloader.bin: bootloader.asm
	nasm -f bin bootloader.asm -o bootloader.bin

run: bootloader.bin
	qemu-system-i386 -drive format=raw,file=bootloader.bin

clean:
	rm -f bootloader.bin