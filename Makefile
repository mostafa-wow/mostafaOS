CC = ~/opt/cross/bin/i686-elf-gcc
ASM = nasm
LD = ~/opt/cross/bin/i686-elf-ld
KERNEL = bin/full_kernel.bin

all:
	$(CC) -Iinclude -ffreestanding -m32 -g -c src/kernel.c -o bin/kernel.o
	$(ASM) bootloader/kernel_entry.asm -f elf -o bin/kernel_entry.o
	$(LD) -o bin/full_kernel.bin -Ttext 0x1000 bin/kernel_entry.o bin/kernel.o --oformat binary
	$(ASM) bootloader/boot.asm -DKERNEL_SECTORS=20 -f bin -o bin/boot.bin
	cat bin/boot.bin bin/full_kernel.bin > bin/os.bin
