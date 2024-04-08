#/bin/bash

mkdir ./bin
mkdir ./build

mkdir ./mount_point

mkdir ./build/idt
mkdir ./build/memory
mkdir ./build/memory/heap
mkdir ./build/memory/paging
mkdir ./build/io
mkdir ./build/disk
mkdir ./build/fs
mkdir ./build/string
mkdir ./build/fs/fat
mkdir ./build/gdt
mkdir ./build/task
mkdir ./build/isr80h
mkdir ./build/keyboard
mkdir ./build/loader
mkdir ./build/loader/formats
mkdir ./build/vga

mkdir ./programs/blank/build
mkdir ./programs/shell/build
mkdir ./programs/libc_crt/build


export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make clean
make all