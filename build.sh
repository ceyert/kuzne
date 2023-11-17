#/bin/bash

mkdir ./bin
mkdir ./build

mkdir ./build/idt
mkdir ./build/memory
mkdir ./build/memory/heap
mkdir ./build/memory/paging
mkdir ./build/io
mkdir ./build/disk

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all