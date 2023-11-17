#/bin/bash

mkdir ./bin
mkdir ./build

mkdir ./build/idt
mkdir ./build/memory

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all