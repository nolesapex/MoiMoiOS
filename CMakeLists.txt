# MoiMoiOS Kernel CMakeLists.txt
cmake_minimum_required(VERSION 3.13)
project(MoiMoiOS C ASM)

# Set the target architecture and compiler flags
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Toolchain file will set CMAKE_C_COMPILER, CMAKE_ASM_COMPILER, etc.

# Add kernel source directories
aux_source_directory(kernel/core CORE_SOURCES)
aux_source_directory(kernel/services/chest CHEST_SOURCES)
aux_source_directory(kernel/services/io IO_SOURCES)
aux_source_directory(kernel/services/notification NOTIFICATION_SOURCES)
aux_source_directory(kernel/boot BOOT_ASM_SOURCES)

# Combine all source files
set(KERNEL_SOURCES
    ${CORE_SOURCES}
    ${CHEST_SOURCES}
    ${IO_SOURCES}
    ${NOTIFICATION_SOURCES}
    ${BOOT_ASM_SOURCES}
)

# Add include directories
include_directories(
    kernel/core
    kernel/services/chest
    kernel/services/io
    kernel/services/notification
)

# Create the kernel executable FIRST
add_executable(moimoios.bin ${KERNEL_SOURCES})

# Set common compiler flags
target_compile_options(moimoios.bin PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU>: -Wall -Wextra -std=c11 -ffreestanding -O2 -g>
    $<$<COMPILE_LANG_AND_ID:ASM,GNU>: -f elf64>
    # MSVC specific flags (placeholder, may need adjustment for bare-metal)
    $<$<COMPILE_LANG_AND_ID:C,MSVC>: /W4 /WX /std:c11 /O2 /Zi>
    $<$<COMPILE_LANG_AND_ID:ASM,MSVC>: > # MSVC assembly flags might need specific handling or separate assembler
)

# Linker script and options
target_link_options(moimoios.bin PRIVATE
    # GNU linker options
    $<$<NOT:$<C_COMPILER_ID:MSVC>>: -T ${CMAKE_SOURCE_DIR}/kernel/boot/linker.ld -nostdlib -Wl,--section-start=.multiboot=0x100000>
    # MSVC linker options (placeholder, may need adjustment for bare-metal)
    $<$<C_COMPILER_ID:MSVC>: /NODEFAULTLIB /ENTRY:kernel_entry>
)

# Set output directory for the executable
set_target_properties(moimoios.bin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
)

# Custom command to create the ISO image
add_custom_target(iso ALL
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
    COMMAND cp $<TARGET_FILE:moimoios.bin> ${CMAKE_BINARY_DIR}/iso/boot/moimoios.bin
    COMMAND cp ${CMAKE_SOURCE_DIR}/kernel/boot/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
    COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/moimoios.iso ${CMAKE_BINARY_DIR}/iso
    DEPENDS moimoios.bin
    COMMENT "Creating bootable ISO image"
)

