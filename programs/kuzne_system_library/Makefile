FILES= ./build/crt.asm.o ./build/crt.o ./build/syscalls.asm.o ./build/syscalls.o ./build/stdlib.o ./build/stdio.o ./build/memory.o

INCLUDES= -I./src

FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./kuzne_system_library.elf

./build/crt.asm.o: ./src/c_runtime/crt.asm
	nasm -f elf ./src/c_runtime/crt.asm -o ./build/crt.asm.o

./build/crt.o: ./src/c_runtime/crt.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/c_runtime/crt.c -o ./build/crt.o

./build/syscalls.asm.o: ./src/syscalls/syscalls.asm
	nasm -f elf ./src/syscalls/syscalls.asm -o ./build/syscalls.asm.o

./build/syscalls.o: ./src/syscalls/syscalls.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/syscalls/syscalls.c -o ./build/syscalls.o

./build/stdlib.o: ./src/libc/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/libc/stdlib.c -o ./build/stdlib.o

./build/stdio.o: ./src/libc/stdio.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/libc/stdio.c -o ./build/stdio.o

./build/memory.o: ./src/libc/memory.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/libc/memory.c -o ./build/memory.o

clean:
	rm -rf ${FILES}
	rm ./kuzne_system_library.elf