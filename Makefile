# MoiMoiOS Makefile

# ----------------------------------------------------------
# Toolchain Configuration
# ----------------------------------------------------------
CC = gcc
AS = nasm
LD = ld

# Compiler and Linker Flags
CFLAGS = -Wall -Wextra -std=c11 -ffreestanding -O2 -Ikernel/core -Ikernel/services
LDFLAGS = -T kernel/boot/linker.ld -nostdlib
ASFLAGS = -f elf64

# ----------------------------------------------------------
# Source Files
# ----------------------------------------------------------
C_SOURCES = $(wildcard kernel/core/*.c) $(wildcard kernel/services/chest/*.c) $(wildcard kernel/services/io/*.c) $(wildcard kernel/services/notification/*.c) kernel/core/klog.c kernel/core/panic.c
ASM_SOURCES = $(wildcard kernel/boot/*.asm)

# Object Files
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)

# ----------------------------------------------------------
# Build Targets
# ----------------------------------------------------------
all: moimoios.bin

moimoios.bin: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.bin *.o $(C_OBJECTS) $(ASM_OBJECTS)

# ----------------------------------------------------------
# ISO Generation (for emulation)
# ----------------------------------------------------------
iso: moimoios.bin
	mkdir -p iso/boot/grub
	cp moimoios.bin iso/boot/
	cp kernel/boot/grub.cfg iso/boot/grub/
	grub-mkrescue -o moimoios.iso iso

