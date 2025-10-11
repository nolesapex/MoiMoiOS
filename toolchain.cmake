# MoiMoiOS Toolchain File
# This file is used by CMake for cross-compilation to the x86_64-elf target.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross compilers
set(GCC_PREFIX "x86_64-elf-")
set(CMAKE_C_COMPILER   "${GCC_PREFIX}gcc")
set(CMAKE_ASM_COMPILER "${GCC_PREFIX}gcc") # GCC can compile assembly too

# Where to find the target environment
set(CMAKE_FIND_ROOT_PATH "/usr/local/cross") # Adjust if your cross-compiler is elsewhere

# Program and library searching will be done in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set flags for the linker
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -T ${CMAKE_SOURCE_DIR}/kernel/boot/linker.ld")

# Set additional compiler flags for bare-metal environment
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-stack-protector -fno-pie -mno-red-zone -mcmodel=large")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -f elf64")


