nasm -f bin boot_src/boot1.asm -o boot1.bin
nasm -f bin boot_src/transition.asm -o transition.bin

for file in boot_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -c "$file" -o "$(basename "${file%.c}_b.o")"
done

for file in shared_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -c "$file" -o "$(basename "${file%.c}_s.o")"
done

for file in kernel_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -c "$file" -o "$(basename "${file%.c}_k.o")"
done

nasm -f elf32 shared_src/disk.asm -o disk_asm_s.o
nasm -f elf32 shared_src/debug.asm -o debug_asm.o
nasm -f elf32 shared_src/mem.asm -o mem_asm.o
nasm -f elf32 shared_src/paging.asm -o paging_asm.o

nasm -f elf32 kernel_src/interrupts.asm -o interrupts_asm.o
nasm -f elf32 kernel_src/disk.asm -o disk_asm_k.o

ld -m elf_i386 -T boot2.ld boot2_b.o text_s.o disk_asm_s.o paging_s.o elf_s.o debug_asm.o mem_asm.o paging_asm.o disk_s.o -o boot2.elf
ld -m elf_i386 -T kernel.ld text_s.o main_k.o paging_asm.o global_k.o file_k.o interrupts_k.o disk_asm_k.o disk_asm_s.o debug_asm.o interrupts_asm.o disk_k.o disk_s.o -o main.elf

objcopy -O binary boot2.elf boot2.bin

size=$(stat -c %s main.elf)
rounded=$(( (size + 511) / 512 * 512 ))
truncate -s "$rounded" main.elf

truncate -s 8192 boot2.bin
truncate -s 512 transition.bin

size=$(($(stat -c %s boot1.bin) + $(stat -c %s boot2.bin) + $(stat -c %s main.elf) + $(stat -c %s transition.bin) + 10000000))
echo "" > filesystem.bin
truncate -s "$size" filesystem.bin

cat boot1.bin boot2.bin main.elf transition.bin filesystem.bin > os.img
rm boot1.bin
rm boot2.bin
rm boot2.elf
rm debug_asm.o
rm boot2_b.o
rm text_s.o
rm interrupts_kernel.o
rm interrupts_asm.o
rm elf_s.o
rm paging_s.o
rm transition.bin
#rm main.elf
rm main_k.o
rm mem_asm.o
rm paging_asm.o
rm file_k.o
rm disk_asm_k.o
rm disk_asm_s.o
rm global_k.o
rm disk_k.o
rm disk_s.o
rm filesystem.bin
