nasm -f bin boot.asm -o bin/boot.bin
$HOME/opt/cross/bin/$TARGET-gcc -ffreestanding -m32 -g -c kernel.c -o bin/kernel.o
nasm -f elf kernel_entry.asm -o bin/kernel_entry.o
$HOME/opt/cross/bin/$TARGET-ld -o bin/full_kernel.bin -Ttext 0x1000 bin/kernel_entry.o bin/kernel.o --oformat binary
cat bin/boot.bin bin/full_kernel.bin >bin/everything.bin
