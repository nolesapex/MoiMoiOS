/*
 * MoiMoiOS - error.c
 * Defines kernel-wide error codes and provides a utility to convert them to strings.
 */

#include "error.h"

/**
 * @brief Converts a kernel_error_t enum value to a human-readable string.
 *
 * This function is useful for debugging and logging, providing more context
 * than raw integer error codes.
 *
 * @param err The kernel_error_t value to convert.
 * @return A constant string describing the error.
 */
const char* kernel_error_to_string(kernel_error_t err) {
    switch (err) {
        case ERR_NONE: return "No error";
        case ERR_ACCESS_DENIED: return "Access denied";
        case ERR_INVALID_TOKEN: return "Invalid token";
        case ERR_INVALID_ARGUMENT: return "Invalid argument";
        case ERR_INVALID_OPERATION: return "Invalid operation";
        case ERR_NO_SPACE: return "No space available";
        case ERR_NOT_IMPLEMENTED: return "Not implemented";
        case ERR_OUT_OF_MEMORY: return "Out of memory";
        default: return "Unknown error";
    }
}

