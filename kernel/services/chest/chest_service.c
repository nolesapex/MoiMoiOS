// MoiMoiOS Chest Service

#include "chest_service.h"
#include "chest_internal.h"
#include "security.h"
#include "logging.h" // Optional: for debugging purposes
#include "../../core/error.h" // New include for kernel_error_t

// Capability definitions (bitmask-based) - ensure these match the .h file
#define CAP_IO_READ     0x0001
#define CAP_IO_WRITE    0x0002
#define CAP_MEM_ALLOC   0x0004
#define CAP_MEM_FREE    0x0008
#define CAP_SCHED_CREATE_TASK 0x0010
#define CAP_SCHED_KILL_TASK   0x0020
#define CAP_FS_READ     0x0040
#define CAP_FS_WRITE    0x0080
#define CAP_NET_SEND    0x0100
#define CAP_NET_RECV    0x0200
#define CAP_SYSCALL     0x0400 // Generic capability for making any syscall

// Operation codes
#define CHEST_OP_RUN_COMMAND     0x01
#define CHEST_OP_REQUEST_PROFILE 0x02
#define CHEST_OP_DELEGATE        0x03
#define CHEST_OP_REVOKE          0x04
#define CHEST_OP_QUERY_TOKEN     0x05

// Token structure
typedef struct {
    uint32_t id;
    uint32_t capabilities;
    uint8_t valid;
} chest_token_t;

#define MAX_TOKENS 64
static chest_token_t token_table[MAX_TOKENS];

static chest_token_t* find_token(uint32_t id) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (token_table[i].valid && token_table[i].id == id) return &token_table[i];
    }
    return NULL;
}

static kernel_error_t chest_delegate(uint32_t from_token_id, uint32_t to_token_id, uint32_t cap_mask) {
    chest_token_t* from = find_token(from_token_id);
    chest_token_t* to = find_token(to_token_id);
    if (!from || !to) return ERR_INVALID_TOKEN;
    if ((from->capabilities & cap_mask) != cap_mask) return ERR_ACCESS_DENIED;

    to->capabilities |= cap_mask;
    return ERR_NONE;
}

static kernel_error_t chest_revoke(uint32_t token_id, uint32_t cap_mask) {
    chest_token_t* tok = find_token(token_id);
    if (!tok) return ERR_INVALID_TOKEN;
    tok->capabilities &= ~cap_mask;
    return ERR_NONE;
}

static kernel_error_t chest_query_token(uint32_t token_id, uint32_t* out_caps) {
    chest_token_t* tok = find_token(token_id);
    if (!tok) return ERR_INVALID_TOKEN;
    *out_caps = tok->capabilities;
    return ERR_NONE;
}

kernel_error_t chest_check_capability(uint32_t token_id, uint32_t required_caps) {
    chest_token_t* tok = find_token(token_id);
    if (!tok) return ERR_INVALID_TOKEN;
    if ((tok->capabilities & required_caps) == required_caps) {
        return ERR_NONE;
    }
    return ERR_ACCESS_DENIED;
}



kernel_error_t chest_service_handler(uint32_t operation, void* payload, uint32_t payload_size, uint32_t token_id) {
    if (chest_check_capability(token_id, CAP_SYSCALL) != ERR_NONE) {
        return ERR_ACCESS_DENIED;
    }

    switch (operation) {
        case CHEST_OP_RUN_COMMAND:
            if (chest_check_capability(token_id, CAP_IO_WRITE | CAP_SCHED_CREATE_TASK) != ERR_NONE) {
                return ERR_ACCESS_DENIED;
            }
            // Assuming chest_run_command returns kernel_error_t
            return chest_run_command((char*)payload, payload_size);

        case CHEST_OP_REQUEST_PROFILE:
            return chest_request_profile((char*)payload);

        case CHEST_OP_DELEGATE: {
            if (payload_size != 12) return ERR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_delegate(args[0], args[1], args[2]);
        }

        case CHEST_OP_REVOKE: {
            if (payload_size != 8) return ERR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_revoke(args[0], args[1]);
        }

        case CHEST_OP_QUERY_TOKEN: {
            if (payload_size != 8) return ERR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_query_token(args[0], (uint32_t*)args[1]);
        }

        default:
            return ERR_INVALID_OPERATION;
    }
}

// Optionally, provide a way to initialize and manage tokens externally
kernel_error_t chest_create_token(uint32_t id, uint32_t caps) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (!token_table[i].valid) {
            token_table[i].id = id;
            token_table[i].capabilities = caps;
            token_table[i].valid = 1;
            return ERR_NONE;
        }
    }
    return ERR_NO_SPACE;
}

kernel_error_t chest_invalidate_token(uint32_t id) {
    chest_token_t* tok = find_token(id);
    if (!tok) return ERR_INVALID_TOKEN;
    tok->valid = 0;
    return ERR_NONE;
}
