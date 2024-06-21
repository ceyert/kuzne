#!/bin/bash

# Refresh page if gdb connection lost!
fuser -k 5000/tcp
gdbgui -p 5000 -g "gdb -ex 'add-symbol-file ./build/kernelfull.o 0x100000' -ex 'target remote | qemu-system-i386 -hda ./bin/os.bin -gdb stdio -S' -ex 'b kernel_main' -ex 'c'"
