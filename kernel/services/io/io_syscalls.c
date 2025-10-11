/*
 * MoiMoiOS - io_syscalls.c
 * Implements I/O related system calls.
 */

#include "io_syscalls.h"
#include "../../core/trap.h"
#include "../chest/chest_service.h"
#include "../../core/klog.h"

/**
 * @brief System call handler for printing a string to the console.
 *
 * This function assumes str_ptr points to a valid userspace string and len
 * specifies its length. In a real kernel, robust memory validation would be
 * required here to prevent unauthorized memory access.
 *
 * @param str_ptr Pointer to the string in userspace memory.
 * @param len Length of the string.
 * @param unused1 Unused argument.
 * @param token_id The token ID associated with the calling process.
 * @return ERR_NONE on success, or an appropriate kernel_error_t on failure.
 */
int sys_puts_handler(uint64_t str_ptr, uint64_t len, uint64_t unused1, uint64_t token_id) {
    // Validate CAP_IO for this token
    if (chest_check_capability((uint32_t)token_id, CAP_IO) != ERR_NONE) {
        return ERR_ACCESS_DENIED;
    }
    // Basic validation: ensure length is not excessive
    if (len > 1024) { // Arbitrary limit for safety
        return ERR_INVALID_ARGUMENT;
    }

    // Assuming str_ptr points to a valid userspace string
    // NOTE: Proper memory validation (e.g., checking if str_ptr is in userspace
    // and accessible by the current process) is crucial for security and omitted
    // here for brevity.
    kprintf("%.*s\n", (int)len, (char*)str_ptr);
    return ERR_NONE;
}

/**
 * @brief Initializes and registers all I/O related system calls.
 */
void init_io_syscalls() {
    register_syscall(SYS_PUTS, sys_puts_handler);
    klog("[IO_SYSCALLS] Registered SYS_PUTS.\n");
    // Register other I/O related syscalls here as they are implemented.
}

