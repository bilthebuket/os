nasm -f bin boot1.asm -o boot1.bin
nasm -f bin transition.asm -o transition.bin
for file in src/*.c; do
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Iinclude -c "$file" -o "$(basename "${file%.c}.o")"
done

nasm -f elf32 interrupts.asm -o interrupts_asm.o
nasm -f elf32 disk.asm -o disk_asm.o

ld -m elf_i386 -T boot2.ld boot2.o text.o interrupts.o interrupts_asm.o disk_asm.o paging.o elf.o -o boot2.elf
objcopy -O binary boot2.elf boot2.bin
truncate -s 8192 boot2.bin
truncate -s 512 transition.bin
cat boot1.bin boot2.bin transition.bin > os.img
rm boot1.bin
rm boot2.bin
rm boot2.elf
rm boot2.o
rm text.o
rm interrupts.o
rm interrupts_asm.o
rm disk_asm.o
