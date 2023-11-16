#!/bin/bash
# run.sh

# Define the emulator
EMULATOR=qemu-system-x86_64

# Run the binary in the emulator
$EMULATOR -hda ./bin/os.bin
