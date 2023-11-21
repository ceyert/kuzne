# Kuzne

Experimental kernel development in C.

- sudo apt install nasm
- brew install i686-elf-gcc (macos & linux)
- qemu-system-i386
- pipx install gdbgui

References : https://wiki.osdev.org

TODOs:
- User process malloc should use process pages instead kernel malloc.

- Switch terminal per-process.

- Sleep, waitqueue, runqueue.

- Kernel-userprocess shared page (userprocess readonly access, kernel read & write access)

- Task scheduler 
