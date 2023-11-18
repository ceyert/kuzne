#/bin/bash

mkdir ./bin
mkdir ./build

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

mkdir ./programs/blank/build

mkdir ./mount_point

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all