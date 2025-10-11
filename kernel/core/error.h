#ifndef ERROR_H
#define ERROR_H

typedef enum {
    ERR_NONE = 0,
    ERR_ACCESS_DENIED = -1,
    ERR_INVALID_TOKEN = -2,
    ERR_INVALID_ARGUMENT = -3,
    ERR_INVALID_OPERATION = -4,
    ERR_NO_SPACE = -5,
    ERR_NOT_IMPLEMENTED = -6,
    ERR_OUT_OF_MEMORY = -7,
    // ... add more specific error codes
} kernel_error_t;

const char* kernel_error_to_string(kernel_error_t err);

#endif // ERROR_H

