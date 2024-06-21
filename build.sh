#/bin/bash

mkdir ./bin
mkdir ./build

mkdir ./mount_point

mkdir ./build/interrupt_descriptor_table
mkdir ./build/interrupt_service_routines
mkdir ./build/memory
mkdir ./build/malloc
mkdir ./build/paging
mkdir ./build/io
mkdir ./build/disk
mkdir ./build/fs
mkdir ./build/fs/fat
mkdir ./build/global_descriptor_table
mkdir ./build/process
mkdir ./build/keyboard
mkdir ./build/loader
mkdir ./build/vga

mkdir ./programs/kuzne_system_library/build
mkdir ./programs/blank/build
mkdir ./programs/shell/build

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

make clean

echo "***** Compiling Kernel *****"
make all

echo "***** Compiling Programs *****"
make compile_user_programs