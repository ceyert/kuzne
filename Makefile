CC = i686-elf-gcc 
FILES = ./build/kernel.asm.o ./build/kernel.o ./build/loader/formats/elf.o ./build/loader/formats/elfloader.o  ./build/isr80h/isr80h.o ./build/isr80h/process.o ./build/isr80h/heap.o ./build/keyboard/keyboard.o ./build/keyboard/classic.o ./build/isr80h/io.o ./build/isr80h/misc.o ./build/disk/disk.o ./build/disk/streamer.o ./build/task/process.o ./build/task/task.o ./build/task/task.asm.o ./build/task/tss.asm.o ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/string/string.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/vga/vga.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin user_programs
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin ./mount_point
	# Copy a file over
	sudo cp ./hello.txt ./mount_point
	sudo cp ./programs/blank/blank.elf ./mount_point
	sudo cp ./programs/shell/shell.elf ./mount_point

	sudo umount ./mount_point
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	$(CC) $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/Boot.asm
	nasm -f bin ./src/boot/Boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/Kernel.asm
	nasm -f elf -g ./src/Kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/Kernel.c
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/Kernel.c -o ./build/kernel.o

./build/vga/vga.o: ./src/vga/Vga.c
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/vga/Vga.c -o ./build/vga/vga.o	

./build/idt/idt.asm.o: ./src/idt/Idt.asm
	nasm -f elf -g ./src/idt/Idt.asm -o ./build/idt/idt.asm.o

./build/loader/formats/elf.o: ./src/loader/formats/Elf.c
	$(CC) $(INCLUDES) -I./src/loader/formats $(FLAGS) -std=gnu99 -c ./src/loader/formats/Elf.c -o ./build/loader/formats/elf.o

./build/loader/formats/elfloader.o: ./src/loader/formats/Elfloader.c
	$(CC) $(INCLUDES) -I./src/loader/formats $(FLAGS) -std=gnu99 -c ./src/loader/formats/Elfloader.c -o ./build/loader/formats/elfloader.o

./build/gdt/gdt.o: ./src/gdt/Gdt.c
	$(CC) $(INCLUDES) -I./src/gdt $(FLAGS) -std=gnu99 -c ./src/gdt/Gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/Gdt.asm
	nasm -f elf -g ./src/gdt/Gdt.asm -o ./build/gdt/gdt.asm.o

./build/isr80h/isr80h.o: ./src/isr80h/Isr80h.c
	$(CC) $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/Isr80h.c -o ./build/isr80h/isr80h.o

./build/isr80h/heap.o: ./src/isr80h/Heap.c
	$(CC) $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/Heap.c -o ./build/isr80h/heap.o

./build/isr80h/misc.o: ./src/isr80h/Misc.c
	$(CC) $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/Misc.c -o ./build/isr80h/misc.o

./build/isr80h/io.o: ./src/isr80h/Io.c
	$(CC) $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/Io.c -o ./build/isr80h/io.o

./build/isr80h/process.o: ./src/isr80h/Process.c
	$(CC) $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/Process.c -o ./build/isr80h/process.o


./build/keyboard/keyboard.o: ./src/keyboard/Keyboard.c
	$(CC) $(INCLUDES) -I./src/keyboard $(FLAGS) -std=gnu99 -c ./src/keyboard/Keyboard.c -o ./build/keyboard/keyboard.o


./build/keyboard/classic.o: ./src/keyboard/Classic.c
	$(CC) $(INCLUDES) -I./src/keyboard $(FLAGS) -std=gnu99 -c ./src/keyboard/Classic.c -o ./build/keyboard/classic.o


./build/idt/idt.o: ./src/idt/Idt.c
	$(CC) $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/Idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory/Memory.c
	$(CC) $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/Memory.c -o ./build/memory/memory.o


./build/task/process.o: ./src/task/Process.c
	$(CC) $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/Process.c -o ./build/task/process.o


./build/task/task.o: ./src/task/Task.c
	$(CC) $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/Task.c -o ./build/task/task.o

./build/task/task.asm.o: ./src/task/Task.asm
	nasm -f elf -g ./src/task/Task.asm -o ./build/task/task.asm.o

./build/task/tss.asm.o: ./src/task/Tss.asm
	nasm -f elf -g ./src/task/Tss.asm -o ./build/task/tss.asm.o

./build/io/io.asm.o: ./src/io/Io.asm
	nasm -f elf -g ./src/io/Io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/Heap.c
	$(CC) $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/Heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/Kheap.c
	$(CC) $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/Kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.o: ./src/memory/paging/Paging.c
	$(CC) $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/Paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/Paging.asm
	nasm -f elf -g ./src/memory/paging/Paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: src/disk/Disk.c
	$(CC) $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/Disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: src/disk/Streamer.c
	$(CC) $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/Streamer.c -o ./build/disk/streamer.o

./build/fs/fat/fat16.o: ./src/fs/fat/Fat16.c
	$(CC) $(INCLUDES) -I./src/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/Fat16.c -o ./build/fs/fat/fat16.o


./build/fs/file.o: ./src/fs/File.c
	$(CC) $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/File.c -o ./build/fs/file.o

./build/fs/pparser.o: ./src/fs/Pparser.c
	$(CC) $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/Pparser.c -o ./build/fs/pparser.o

./build/string/string.o: ./src/string/String.c
	$(CC) $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/String.c -o ./build/string/string.o

user_programs:
	cd ./programs/stdlib && $(MAKE) all
	cd ./programs/blank && $(MAKE) all
	cd ./programs/shell && $(MAKE) all

user_programs_clean:
	cd ./programs/stdlib && $(MAKE) clean
	cd ./programs/blank && $(MAKE) clean
	cd ./programs/shell && $(MAKE) clean

clean: user_programs_clean
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o