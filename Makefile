# Define the assembler and the flags
AS=nasm
ASFLAGS=-f bin

# Define the source and the output file
SRC=./boot.asm
OUT=./boot.bin

# Define the emulator
EMULATOR=qemu-system-x86_64

# Default target
all: $(OUT)

# Rule to build the binary
$(OUT): $(SRC)
	$(AS) $(ASFLAGS) $< -o $@

# Target to run the binary in an emulator
run: $(OUT)
	$(EMULATOR) -hda $(OUT)

# Clean target
clean:
	rm -f $(OUT)
