#!/bin/bash
# run.sh

EMULATOR=qemu-system-x86_64

RAM_SIZE=1G

$EMULATOR -hda ./bin/os.bin -m $RAM_SIZE