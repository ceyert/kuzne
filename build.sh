#/bin/bash

mkdir ./bin
mkdir ./build

mkdir ./build/idt
mkdir ./build/memory
mkdir ./build/memory/heap
mkdir ./build/io

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all