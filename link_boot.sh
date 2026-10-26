nasm -f bin boot_src/boot1.asm -o boot1.bin
nasm -f bin boot_src/transition.asm -o transition.bin

for file in boot_src/*.c; do
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -Iboot_include -c "$file" -o "$(basename "${file%.c}.o")"
done

for file in shared_src/*.c; do
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -c "$file" -o "$(basename "${file%.c}.o")"
done

for file in kernel_src/*.c; do
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -Ishared_include -Ikernel_include -c "$file" -o "$(basename "${file%.c}.o")"
done

nasm -f elf32 kernel_src/interrupts.asm -o interrupts_asm.o
nasm -f elf32 shared_src/disk.asm -o disk_asm.o

ld -m elf_i386 -T boot2.ld boot2.o text.o disk_asm.o paging.o elf.o -o boot2.elf
ld -m elf_i386 text.o main.o -o main.elf

objcopy -O binary boot2.elf boot2.bin
objcopy -O binary main.elf main.bin

truncate -s 8192 boot2.bin
truncate -s 512 transition.bin
truncate -s 512 main.bin

cat boot1.bin boot2.bin main.bin transition.bin > os.img
rm boot1.bin
rm boot2.bin
rm boot2.elf
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
rm main.bin
