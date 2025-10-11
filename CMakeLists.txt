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

# Set common compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -ffreestanding -O2 -g")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -f elf64")

# Create the kernel executable
add_executable(moimoios.bin ${KERNEL_SOURCES})

# Linker script
target_link_options(moimoios.bin PRIVATE
    -T kernel/boot/linker.ld
    -nostdlib
)

# Ensure Multiboot2 header is linked first
target_link_libraries(moimoios.bin PRIVATE
    -Wl,--section-start=.multiboot=0x100000
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
    COMMAND cp $< ${CMAKE_BINARY_DIR}/iso/boot/moimoios.bin
    COMMAND cp ${CMAKE_SOURCE_DIR}/kernel/boot/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
    COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/moimoios.iso ${CMAKE_BINARY_DIR}/iso
    DEPENDS moimoios.bin
    COMMENT "Creating bootable ISO image"
)

