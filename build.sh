nasm -f bin cores.asm -o cores.bin
dd if=/dev/zero of=disk.img bs=512 count=2880
dd if=cores.bin of=disk.img bs=512 conv=notrunc
qemu-system-x86_64 -fda disk.img -smp cores=5
