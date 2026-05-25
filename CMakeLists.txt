# MoiMoiOS Kernel CMakeLists.txt
cmake_minimum_required(VERSION 3.13)
project(MoiMoiOS C ASM)

# Target architecture
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Enable ASM (for GNU .asm files)
enable_language(ASM)

# Collect source files
file(GLOB_RECURSE CORE_SOURCES kernel/core/*.c)
file(GLOB_RECURSE SERVICE_SOURCES kernel/services/*/*.c)
file(GLOB BOOT_SOURCES kernel/boot/*.asm)

set(KERNEL_SOURCES
    ${CORE_SOURCES}
    ${SERVICE_SOURCES}
    ${BOOT_SOURCES}
)
# Include directories
include_directories(
    kernel/core
    kernel/services/chest
    kernel/services/io
    kernel/services/notification
)
# Define the kernel target
add_executable(moimoios.elf ${KERNEL_SOURCES})

# Compiler flags for bare metal
target_compile_options(moimoios.elf PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU>:
        -Wall -Wextra -std=c11 -ffreestanding -O2 -g
    >
    $<$<COMPILE_LANG_AND_ID:ASM,GNU>:
        -x assembler-with-cpp
    >
)

# Linker options
target_link_options(moimoios.elf PRIVATE
    $<$<C_COMPILER_ID:GNU>:
        -T ${CMAKE_SOURCE_DIR}/kernel/boot/linker.ld
        -nostdlib
        -static
    >
)

# Set output directories
set_target_properties(moimoios.elf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/boot
)
# ISO creation (GRUB bootable)
add_custom_target(iso ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/iso/boot/grub
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:moimoios.elf> ${CMAKE_BINARY_DIR}/iso/boot/moimoios.elf
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/kernel/boot/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
    COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/moimoios.iso ${CMAKE_BINARY_DIR}/iso
    DEPENDS moimoios.elf
    COMMENT "Creating bootable ISO image"
)

