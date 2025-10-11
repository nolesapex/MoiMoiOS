# MoiMoiOS Toolchain File
# This file is used by CMake for cross-compilation to the x86_64-elf target.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross compilers
# For bare-metal x86_64, we typically use a GNU cross-compiler (e.g., x86_64-elf-gcc).
# If building on Windows, ensure this cross-compiler is in your PATH (e.g., via MinGW/WSL).
# MSVC is generally not used for bare-metal kernel development in this manner.
set(CMAKE_C_COMPILER   "x86_64-elf-gcc")
set(CMAKE_ASM_COMPILER "x86_64-elf-gcc") # GCC can compile assembly too

# Where to find the target environment
# This path should point to the root of your cross-compiler installation if not in standard PATH.
# For example, if installed to /usr/local/cross/x86_64-elf, set CMAKE_FIND_ROOT_PATH to /usr/local/cross
set(CMAKE_FIND_ROOT_PATH "/usr/local/cross") 

# Program and library searching will be done in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set additional compiler flags for bare-metal environment
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-stack-protector -fno-pie -mno-red-zone -mcmodel=large")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -f elf64")

# Set flags for the linker (these are usually handled by target_link_options in CMakeLists.txt)
# However, if you need to set global linker flags here, ensure they are compatible with your linker.
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -T ${CMAKE_SOURCE_DIR}/kernel/boot/linker.ld")

