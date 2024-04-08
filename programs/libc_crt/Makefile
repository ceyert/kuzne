FILES=./build/crt.asm.o ./build/crt.o ./build/syscalls.asm.o ./build/syscalls.o ./build/stdlib.o ./build/stdio.o ./build/string.o ./build/memory.o
INCLUDES=-I./src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./stdlib.elf

./build/crt.asm.o: ./src/crt.asm
	nasm -f elf ./src/crt.asm -o ./build/crt.asm.o

./build/syscalls.asm.o: ./src/syscalls.asm
	nasm -f elf ./src/syscalls.asm -o ./build/syscalls.asm.o

./build/syscalls.o: ./src/syscalls.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/syscalls.c -o ./build/syscalls.o

./build/stdlib.o: ./src/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdlib.c -o ./build/stdlib.o

./build/stdio.o: ./src/stdio.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdio.c -o ./build/stdio.o


./build/string.o: ./src/string.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/string.c -o ./build/string.o

./build/memory.o: ./src/memory.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/memory.c -o ./build/memory.o

./build/crt.o: ./src/crt.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/crt.c -o ./build/crt.o


clean:
	rm -rf ${FILES}
	rm ./stdlib.elf