nasm -f bin boot_src/boot1.asm -o boot1.bin
nasm -f bin boot_src/transition.asm -o transition.bin

for file in boot_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -I. -c "$file" -o "$(basename "${file%.c}_b.o")"
done

for file in shared_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -I. -c "$file" -o "$(basename "${file%.c}_s.o")"
done

for file in kernel_src/*.c; do
	gcc -g -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fno-builtin -I. -c "$file" -o "$(basename "${file%.c}_k.o")"
done

nasm -f elf32 shared_src/disk.asm -o disk_asm_s.o
nasm -f elf32 shared_src/debug.asm -o debug_asm.o
nasm -f elf32 shared_src/mem.asm -o mem_asm.o
nasm -f elf32 shared_src/paging.asm -o paging_asm.o

nasm -f elf32 kernel_src/interrupts.asm -o interrupts_asm.o
nasm -f elf32 kernel_src/disk.asm -o disk_asm_k.o

ld -m elf_i386 -T boot2.ld boot2_b.o text_s.o disk_asm_s.o paging_s.o elf_s.o debug_asm.o mem_asm.o paging_asm.o disk_s.o -o boot2.elf
ld -m elf_i386 -T kernel.ld text_s.o main_k.o paging_asm.o global_k.o file_k.o interrupts_k.o disk_asm_s.o disk_asm_k.o debug_asm.o interrupts_asm.o disk_k.o disk_s.o string_k.o -o main.elf

objcopy -O binary boot2.elf boot2.bin

size=$(stat -c %s main.elf)
rounded=$(( (size + 511) / 512 * 512 ))
truncate -s "$rounded" main.elf

truncate -s 8192 boot2.bin
truncate -s 512 transition.bin

# create the filesystem with all os code and sector allocation table reserved (all of this is ai generated)

OUTPUT="filesystem.bin"
FILESYSTEM_SIZE=10000000
SECTOR_SIZE=512

SIZE=$((
    $(stat -c %s boot1.bin) +
    $(stat -c %s boot2.bin) +
    $(stat -c %s main.elf) +
    $(stat -c %s transition.bin)
))

OS_SECTORS=$((SIZE / SECTOR_SIZE))

perl -e '
    use strict;
    use warnings;

    my ($os_size, $filesystem_size) = @ARGV;

    my $sector_size = 512;

    # Total sectors in the ENTIRE os.img.
    my $total_size = $os_size + $filesystem_size;

    my $total_sectors = int(
        ($total_size + $sector_size - 1) / $sector_size
    );

    # Calculate bitmap storage requirements.
    my $bitmap_bytes = int(($total_sectors + 7) / 8);

    my $bitmap_sectors = int(
        ($bitmap_bytes + $sector_size - 1) / $sector_size
    );

    # OS sectors are already aligned.
    my $os_sectors = $os_size / $sector_size;

    # Reserve OS code and bitmap sectors.
    my $reserved = $os_sectors + $bitmap_sectors;

    die "Not enough sectors for reserved regions\n"
        if $reserved > $total_sectors;

    # One bit per sector of the entire os.img.
    # 1 = reserved, 0 = free.
    my $binary = ("1" x $reserved);
    $binary .= "0" x ($total_sectors - $reserved);

    # Pad to the full bitmap storage size.
    my $bitmap_storage_bits = $bitmap_sectors * $sector_size * 8;

    $binary .= "0" x ($bitmap_storage_bits - length($binary));

    # Write only the bitmap sectors.
    print pack("B*", $binary);
' "$SIZE" "$FILESYSTEM_SIZE" > "$OUTPUT"

truncate -s "$FILESYSTEM_SIZE" "$OUTPUT"

cat boot1.bin boot2.bin main.elf transition.bin "$OUTPUT" > os.img

rm boot1.bin
rm boot2.bin
rm boot2.elf
rm debug_asm.o
rm boot2_b.o
rm text_s.o
rm interrupts_k.o
rm interrupts_asm.o
rm elf_s.o
rm paging_s.o
rm transition.bin
#rm main.elf
rm main_k.o
rm mem_asm.o
rm paging_asm.o
rm file_k.o
rm disk_asm_s.o
rm disk_asm_k.o
rm global_k.o
rm disk_k.o
rm disk_s.o
rm "$OUTPUT"
rm string_k.o
