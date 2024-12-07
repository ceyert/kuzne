#!/bin/bash

# Create necessary directories
mkdir -p ./bin
mkdir -p ./build
mkdir -p ./mount_point

# Create subdirectories
SUBDIRS=(
    interrupt_descriptor_table
    interrupt_service_routines
    memory
    malloc
    paging
    io
    disk
    fs
    fs/fat
    global_descriptor_table
    process
    keyboard
    loader
    vga
)

for subdir in "${SUBDIRS[@]}"; do
    mkdir -p "./build/$subdir"
done

# Create program-specific directories
mkdir -p ./programs/kuzne_system_library/build
mkdir -p ./programs/blank/build
mkdir -p ./programs/shell/build

# Set environment variables
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Clean and build
make clean

echo "***** Compiling Kernel *****"
make all

echo "***** Compiling Programs *****"
make compile_user_programs