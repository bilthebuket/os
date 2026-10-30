nasm -f bin boot_src/boot1.asm -o boot1.bin
nasm -f bin boot_src/transition.asm -o transition.bin

for file in boot_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -Iboot_include -c "$file" -o "$(basename "${file%.c}.o")"
done

for file in shared_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -c "$file" -o "$(basename "${file%.c}.o")"
done

for file in kernel_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -Ikernel_include -c "$file" -o "$(basename "${file%.c}.o")"
done

nasm -f elf32 kernel_src/interrupts.asm -o interrupts_asm.o
nasm -f elf32 shared_src/disk.asm -o disk_asm.o
nasm -f elf32 shared_src/debug.asm -o debug_asm.o
nasm -f elf32 shared_src/mem.asm -o mem_asm.o
nasm -f elf32 shared_src/paging.asm -o paging_asm.o

ld -m elf_i386 -T boot2.ld boot2.o text.o disk_asm.o paging.o elf.o debug_asm.o mem_asm.o paging_asm.o -o boot2.elf
ld -m elf_i386 -T kernel.ld text.o main.o paging_asm.o -o main.elf

objcopy -O binary boot2.elf boot2.bin

size=$(stat -c %s main.elf)
rounded=$(( (size + 511) / 512 * 512 ))
truncate -s "$rounded" main.elf

truncate -s 8192 boot2.bin
truncate -s 512 transition.bin

cat boot1.bin boot2.bin main.elf transition.bin > os.img
rm boot1.bin
rm boot2.bin
#rm boot2.elf
rm debug_asm.o
rm boot2.o
rm text.o
rm interrupts.o
rm interrupts_asm.o
rm disk_asm.o
rm elf.o
rm paging.o
rm transition.bin
rm main.elf
rm main.o
rm mem_asm.o
rm paging_asm.o
