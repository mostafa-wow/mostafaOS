CC = ~/opt/cross/bin/i686-elf-gcc
ASM = nasm
LD = ~/opt/cross/bin/i686-elf-ld
 
all:
	$(CC) -ffreestanding -m32 -g -c kernel.c bin/kernel.o
	$(ASM) kernel_entry.asm -f elf -o bin/kernel_entry.o
	$(LD)  -o bin/full_kernel.bin  -Ttext 0x1000 bin/kernel.o bin/kernel_entry.o --oformat binary
	$(ASM) boot.asm -f bin -o bin/boot.bin
	cat bin/boot.bin bin/full_kernel.bin > bin/os.bin

