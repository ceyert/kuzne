ORG 0x7c00                ; The bootloader is loaded at memory address 0x7C00
BITS 16                  ; Start execution in 16-bit real mode

CODE_SEG equ gdt_code - gdt_start ; Define offset for the code segment in the GDT
DATA_SEG equ gdt_data - gdt_start ; Define offset for the data segment in the GDT

jmp short start           ; Jump to the start of the program
nop                       ; No-operation (used for alignment)

; FAT16 Header
OEMIdentifier           db 'PEACHOS '       ; 8-byte identifier for the operating system
BytesPerSector          dw 0x200            ; 512 bytes per sector
SectorsPerCluster       db 0x80             ; Number of sectors in a cluster
ReservedSectors         dw 200              ; Number of reserved sectors
FATCopies               db 0x02             ; Number of FAT tables
RootDirEntries          dw 0x40             ; Number of root directory entries
NumSectors              dw 0x00             ; Total sectors (if < 32MB)
MediaType               db 0xF8             ; Media descriptor (0xF8 for fixed disk)
SectorsPerFat           dw 0x100            ; Sectors per FAT
SectorsPerTrack         dw 0x20             ; Number of sectors per track
NumberOfHeads           dw 0x40             ; Number of heads
HiddenSectors           dd 0x00             ; Number of hidden sectors
SectorsBig              dd 0x773594         ; Total number of sectors (if > 32MB)

; Extended BPB (DOS 4.0)
DriveNumber             db 0x80             ; Drive number (0x80 for the first hard disk)
WinNTBit                db 0x00             ; Reserved for Windows NT
Signature               db 0x29             ; Extended boot signature
VolumeID                dd 0xD105           ; Volume serial number
VolumeIDString          db 'PEACHOS BOO'    ; Volume label (11 characters)
SystemIDString          db 'FAT16   '       ; File system type string (8 characters)

start:
    jmp 0:step2         ; Far jump to reset the segment registers

step2:
    cli                 ; Disable interrupts
    mov ax, 0x00        ; Zero-out AX
    mov ds, ax          ; Set DS to 0
    mov es, ax          ; Set ES to 0
    mov ss, ax          ; Set SS to 0
    mov sp, 0x7c00      ; Set the stack pointer to 0x7C00
    sti                 ; Enable interrupts

.load_protected:
    cli                 ; Disable interrupts
    lgdt [gdt_descriptor] ; Load the Global Descriptor Table (GDT)
    mov eax, cr0        ; Load CR0 register
    or eax, 0x1         ; Set the PE (Protected Mode Enable) bit
    mov cr0, eax        ; Write back to CR0 to enable protected mode
    jmp CODE_SEG:load32 ; Far jump to 32-bit protected mode

; GDT
gdt_start:
gdt_null:
    dd 0x0             ; Null descriptor (required by GDT specifications)
    dd 0x0             ; Null descriptor continuation

; offset 0x8
gdt_code:               ; Code segment descriptor
    dw 0xffff          ; Limit (16 bits)
    dw 0               ; Base (16 bits)
    db 0               ; Base (next 8 bits)
    db 0x9a            ; Access byte (executable, readable, accessed)
    db 11001111b       ; Flags (granularity and limit bits)
    db 0               ; Base (final 8 bits)

; offset 0x10
gdt_data:               ; Data segment descriptor
    dw 0xffff          ; Limit (16 bits)
    dw 0               ; Base (16 bits)
    db 0               ; Base (next 8 bits)
    db 0x92            ; Access byte (read/write, accessed)
    db 11001111b       ; Flags (granularity and limit bits)
    db 0               ; Base (final 8 bits)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1 ; Size of GDT (in bytes - 1)
    dd gdt_start             ; Base address of the GDT

[BITS 32]                ; Switch to 32-bit protected mode instructions

load32:
    mov eax, 1          ; Placeholder operation (setup data)
    mov ecx, 100        ; Number of sectors to read
    mov edi, 0x0100000  ; Destination address for read data
    call ata_lba_read   ; Call ATA disk read function
    jmp CODE_SEG:0x0100000 ; Far jump to execute the loaded data

ata_lba_read:
    mov ebx, eax        ; Backup LBA (Logical Block Address)
    ; Send the highest 8 bits of the LBA
    shr eax, 24         ; Shift to extract the highest byte
    or eax, 0xE0        ; Set the master drive bit
    mov dx, 0x1F6       ; Drive/head register
    out dx, al          ; Output to the drive/head register

    ; Send the total sectors to read
    mov eax, ecx        ; Load the sector count
    mov dx, 0x1F2       ; Sector count register
    out dx, al          ; Output sector count

    ; Send the lower 24 bits of the LBA
    mov eax, ebx        ; Restore LBA
    mov dx, 0x1F3       ; LBA low byte register
    out dx, al          ; Output low byte
    shr eax, 8          ; Shift to the middle byte
    mov dx, 0x1F4       ; LBA middle byte register
    out dx, al          ; Output middle byte
    shr eax, 8          ; Shift to the high byte
    mov dx, 0x1F5       ; LBA high byte register
    out dx, al          ; Output high byte

    ; Send the read command
    mov dx, 0x1f7       ; Command register
    mov al, 0x20        ; Read sectors command
    out dx, al          ; Send command

    ; Read all sectors into memory
.next_sector:
    push ecx            ; Save sector count
.try_again:
    mov dx, 0x1f7       ; Status register
    in al, dx           ; Read status
    test al, 8          ; Check DRQ (Data Request) bit
    jz .try_again       ; Wait if DRQ is not set

    ; Read data
    mov ecx, 256        ; Words to read (512 bytes)
    mov dx, 0x1F0       ; Data register
    rep insw            ; Read words into memory
    pop ecx             ; Restore sector count
    loop .next_sector   ; Repeat for remaining sectors
    ret                 ; Return to caller

times 510-($ - $$) db 0 ; Fill to 510 bytes with zeros
dw 0xAA55               ; Boot signature (0xAA55)