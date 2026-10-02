nasm -f bin boot1.asm -o boot1.bin
nasm -f bin boot2.asm -o boot2.bin
truncate -s 512 boot2.bin
cat boot1.bin boot2.bin > os.img
rm boot1.bin
rm boot2.bin
