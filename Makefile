CC = i686-elf-gcc 

FILES = ./build/kernel.asm.o ./build/kernel.o ./build/loader/elf.o ./build/loader/elfloader.o  ./build/interrupt_service_routines/interrupt_service_routines.o ./build/interrupt_service_routines/process_isr.o ./build/interrupt_service_routines/heap_isr.o ./build/keyboard/keyboard.o ./build/keyboard/classic.o ./build/interrupt_service_routines/io_isr.o ./build/interrupt_service_routines/misc_isr.o ./build/disk/disk.o ./build/disk/streamer.o ./build/process/process.o ./build/process/task.o ./build/process/task.asm.o ./build/process/tss.asm.o ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/interrupt_descriptor_table/idt.asm.o ./build/interrupt_descriptor_table/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/global_descriptor_table/gdt.o ./build/global_descriptor_table/gdt.asm.o ./build/malloc/heap.o ./build/malloc/kheap.o ./build/paging/paging.o ./build/paging/paging.asm.o ./build/vga/vga.o

INCLUDES = -I./src

FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin 
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin ./mount_point

compile_user_programs: user_programs
	# Compiling User Programs
	sudo cp ./hello.txt ./mount_point
	sudo cp ./programs/blank/blank.elf ./mount_point
	sudo cp ./programs/shell/shell.elf ./mount_point
	sudo umount ./mount_point

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	$(CC) $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/arch/x86/Boot.asm
	nasm -f bin ./src/arch/x86/Boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/arch/x86/Kernel.asm
	nasm -f elf -g ./src/arch/x86/Kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/Kernel.c
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/Kernel.c -o ./build/kernel.o

./build/vga/vga.o: ./src/vga/Vga.c
	$(CC) $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/vga/Vga.c -o ./build/vga/vga.o	

./build/interrupt_descriptor_table/idt.asm.o: ./src/arch/x86/Idt.asm
	nasm -f elf -g ./src/arch/x86/Idt.asm -o ./build/interrupt_descriptor_table/idt.asm.o

./build/loader/elf.o: ./src/loader/Elf.c
	$(CC) $(INCLUDES) -I./src/loader $(FLAGS) -std=gnu99 -c ./src/loader/Elf.c -o ./build/loader/elf.o

./build/loader/elfloader.o: ./src/loader/Elfloader.c
	$(CC) $(INCLUDES) -I./src/loader $(FLAGS) -std=gnu99 -c ./src/loader/Elfloader.c -o ./build/loader/elfloader.o

./build/global_descriptor_table/gdt.o: ./src/global_descriptor_table/Gdt.c
	$(CC) $(INCLUDES) -I./src/global_descriptor_table $(FLAGS) -std=gnu99 -c ./src/global_descriptor_table/Gdt.c -o ./build/global_descriptor_table/gdt.o

./build/global_descriptor_table/gdt.asm.o: ./src/arch/x86/Gdt.asm
	nasm -f elf -g ./src/arch/x86/Gdt.asm -o ./build/global_descriptor_table/gdt.asm.o

./build/interrupt_service_routines/interrupt_service_routines.o: ./src/interrupt_service_routines/interrupt_service_routines.c
	$(CC) $(INCLUDES) -I./src/interrupt_service_routines $(FLAGS) -std=gnu99 -c ./src/interrupt_service_routines/interrupt_service_routines.c -o ./build/interrupt_service_routines/interrupt_service_routines.o

./build/interrupt_service_routines/heap_isr.o: ./src/interrupt_service_routines/Heap_isr.c
	$(CC) $(INCLUDES) -I./src/interrupt_service_routines $(FLAGS) -std=gnu99 -c ./src/interrupt_service_routines/Heap_isr.c -o ./build/interrupt_service_routines/heap_isr.o

./build/interrupt_service_routines/misc_isr.o: ./src/interrupt_service_routines/Misc_isr.c
	$(CC) $(INCLUDES) -I./src/interrupt_service_routines $(FLAGS) -std=gnu99 -c ./src/interrupt_service_routines/Misc_isr.c -o ./build/interrupt_service_routines/misc_isr.o

./build/interrupt_service_routines/io_isr.o: ./src/interrupt_service_routines/Io_isr.c
	$(CC) $(INCLUDES) -I./src/interrupt_service_routines $(FLAGS) -std=gnu99 -c ./src/interrupt_service_routines/Io_isr.c -o ./build/interrupt_service_routines/io_isr.o

./build/interrupt_service_routines/process_isr.o: ./src/interrupt_service_routines/Process_isr.c
	$(CC) $(INCLUDES) -I./src/interrupt_service_routines $(FLAGS) -std=gnu99 -c ./src/interrupt_service_routines/Process_isr.c -o ./build/interrupt_service_routines/process_isr.o

./build/keyboard/keyboard.o: ./src/keyboard/Keyboard.c
	$(CC) $(INCLUDES) -I./src/keyboard $(FLAGS) -std=gnu99 -c ./src/keyboard/Keyboard.c -o ./build/keyboard/keyboard.o

./build/keyboard/classic.o: ./src/keyboard/Classic.c
	$(CC) $(INCLUDES) -I./src/keyboard $(FLAGS) -std=gnu99 -c ./src/keyboard/Classic.c -o ./build/keyboard/classic.o

./build/interrupt_descriptor_table/idt.o: ./src/interrupt_descriptor_table/Idt.c
	$(CC) $(INCLUDES) -I./src/interrupt_descriptor_table $(FLAGS) -std=gnu99 -c ./src/interrupt_descriptor_table/Idt.c -o ./build/interrupt_descriptor_table/idt.o

./build/memory/memory.o: ./src/memory/Memory.c
	$(CC) $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/Memory.c -o ./build/memory/memory.o

./build/process/process.o: ./src/process/Process.c
	$(CC) $(INCLUDES) -I./src/process $(FLAGS) -std=gnu99 -c ./src/process/Process.c -o ./build/process/process.o

./build/process/task.o: ./src/process/Task.c
	$(CC) $(INCLUDES) -I./src/process $(FLAGS) -std=gnu99 -c ./src/process/Task.c -o ./build/process/task.o

./build/process/task.asm.o: ./src/process/Task.asm
	nasm -f elf -g ./src/process/Task.asm -o ./build/process/task.asm.o

./build/process/tss.asm.o: ./src/arch/x86/Tss.asm
	nasm -f elf -g ./src/arch/x86/Tss.asm -o ./build/process/tss.asm.o

./build/io/io.asm.o: ./src/arch/x86/Io.asm
	nasm -f elf -g ./src/arch/x86/Io.asm -o ./build/io/io.asm.o

./build/malloc/heap.o: ./src/malloc/Heap.c
	$(CC) $(INCLUDES) -I./src/malloc $(FLAGS) -std=gnu99 -c ./src/malloc/Heap.c -o ./build/malloc/heap.o

./build/malloc/kheap.o: ./src/malloc/Kheap.c
	$(CC) $(INCLUDES) -I./src/malloc $(FLAGS) -std=gnu99 -c ./src/malloc/Kheap.c -o ./build/malloc/kheap.o

./build/paging/paging.o: ./src/paging/Paging.c
	$(CC) $(INCLUDES) -I./src/paging $(FLAGS) -std=gnu99 -c ./src/paging/Paging.c -o ./build/paging/paging.o

./build/paging/paging.asm.o: ./src/arch/x86/Paging.asm
	nasm -f elf -g ./src/arch/x86/Paging.asm -o ./build/paging/paging.asm.o

./build/disk/disk.o: src/disk/Disk.c
	$(CC) $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/Disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: src/disk/Streamer.c
	$(CC) $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/Streamer.c -o ./build/disk/streamer.o

./build/fs/fat/fat16.o: ./src/fs/fat/Fat16.c
	$(CC) $(INCLUDES) -I./src/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/Fat16.c -o ./build/fs/fat/fat16.o

./build/fs/file.o: ./src/fs/File.c
	$(CC) $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/File.c -o ./build/fs/file.o

./build/fs/pparser.o: ./src/fs/PathParser.c
	$(CC) $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/PathParser.c -o ./build/fs/pparser.o

user_programs:
	cd ./programs/kuzne_system_library && $(MAKE) all
	cd ./programs/blank && $(MAKE) all
	cd ./programs/shell && $(MAKE) all

user_programs_clean:
	cd ./programs/kuzne_system_library && $(MAKE) clean
	cd ./programs/blank && $(MAKE) clean
	cd ./programs/shell && $(MAKE) clean

clean: user_programs_clean
	rm -rf ./bin
	rm -rf ${FILES}
	rm -rf ./build
	rm -rf ./mount_point