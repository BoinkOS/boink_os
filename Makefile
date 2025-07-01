# === Filenames ===

KERNEL_SRCS := $(shell find kernel -name '*.c')
KERNEL_OBJS := $(patsubst kernel/%.c, build/%.o, $(KERNEL_SRCS))
IVEC_SRC := kernel/interrupts/interrupt_vectors.asm
IVEC_OBJ := build/interrupts/interrupt_vectors.o

EVEC_SRC := kernel/interrupts/exception_vectors.asm
EVEC_OBJ := build/interrupts/exception_vectors.o

USER_SWITCH_SRC := kernel/cpu/user_switch.asm
USER_SWITCH_OBJ := build/kernel/cpu/user_switch.o

SYSC_ENTRY_SRC := kernel/sys/syscall_entry.asm
SYSC_ENTRY_OBJ := build/kernel/sys/syscall_entry.o

GDT_SRC := gdt/gdt.asm
GDT_OBJ := build/gdt/gdt.o

# put main.o first when linking
KERNEL_OBJS_ORDERED := $(filter build/main.o, $(KERNEL_OBJS)) \
					$(filter-out build/main.o, $(KERNEL_OBJS)) \
					$(USER_SWITCH_OBJ) $(SYSC_ENTRY_OBJ) \
					$(GDT_OBJ) $(IVEC_OBJ) $(EVEC_OBJ)

KERNEL_BIN := build/kernel.bin
LINKER_SCRIPT := link.ld

DISKNAME := boinkos

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

# compile C files into object files under /build
build/%.o: kernel/%.c | build
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(GDT_OBJ): $(GDT_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(GDT_SRC) -o $(GDT_OBJ)

$(IVEC_OBJ): $(IVEC_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(IVEC_SRC) -o $(IVEC_OBJ)

$(EVEC_OBJ): $(EVEC_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(EVEC_SRC) -o $(EVEC_OBJ)
	
$(USER_SWITCH_OBJ): $(USER_SWITCH_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(USER_SWITCH_SRC) -o $(USER_SWITCH_OBJ)

$(SYSC_ENTRY_OBJ): $(SYSC_ENTRY_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(SYSC_ENTRY_SRC) -o $(SYSC_ENTRY_OBJ)

$(TSS_OBJ): $(TSS_SRC) | build
	@mkdir -p $(dir $@)
	nasm -f elf32 $(TSS_SRC) -o $(TSS_OBJ)

# link kernel with main.o first
$(KERNEL_BIN): $(KERNEL_OBJS_ORDERED) $(IVEC_OBJ) $(EVEC_OBJ) $(LINKER_SCRIPT)
	$(LD) $(LD_FLAGS) $(KERNEL_OBJS_ORDERED) -o $(KERNEL_BIN)

disk: $(KERNEL_BIN)
	glfs-mkfs $(DISKNAME).glfs
	glfs-add $(DISKNAME).glfs $(KERNEL_BIN) boink.bin
	glfs-add $(DISKNAME).glfs x.txt lipsum.txt
	glfs-ls $(DISKNAME).glfs

run: disk
	qemu-system-i386 -fda boinkboot.img -hdb $(DISKNAME).glfs -vga std

clean:
	rm -rf build $(DISKNAME).glfs