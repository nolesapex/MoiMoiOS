// MoiMoiOS Chest Service

#include "chest_service.h"
#include "chest_internal.h"
#include "security.h"
#include "logging.h" // Optional: for debugging purposes

// Capability definitions (bitmask-based)
#define CAP_IO     0x01
#define CAP_MEM    0x02
#define CAP_SCHED  0x04
#define CAP_FS     0x08
#define CAP_NET    0x10

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

static int chest_delegate(uint32_t from_token_id, uint32_t to_token_id, uint32_t cap_mask) {
    chest_token_t* from = find_token(from_token_id);
    chest_token_t* to = find_token(to_token_id);
    if (!from || !to) return ERROR_INVALID_TOKEN;
    if ((from->capabilities & cap_mask) != cap_mask) return ERROR_ACCESS_DENIED;

    to->capabilities |= cap_mask;
    return SUCCESS;
}

static int chest_revoke(uint32_t token_id, uint32_t cap_mask) {
    chest_token_t* tok = find_token(token_id);
    if (!tok) return ERROR_INVALID_TOKEN;
    tok->capabilities &= ~cap_mask;
    return SUCCESS;
}

static int chest_query_token(uint32_t token_id, uint32_t* out_caps) {
    chest_token_t* tok = find_token(token_id);
    if (!tok) return ERROR_INVALID_TOKEN;
    *out_caps = tok->capabilities;
    return SUCCESS;
}

int chest_service_handler(uint32_t operation, void* payload, uint32_t payload_size, uint32_t token_id) {
    chest_token_t* tok = find_token(token_id);
    if (!tok || !(tok->capabilities & CAP_IO)) {
        return ERROR_ACCESS_DENIED;
    }

    switch (operation) {
        case CHEST_OP_RUN_COMMAND:
            return chest_run_command((char*)payload, payload_size);

        case CHEST_OP_REQUEST_PROFILE:
            return chest_request_profile((char*)payload);

        case CHEST_OP_DELEGATE: {
            if (payload_size != 12) return ERROR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_delegate(args[0], args[1], args[2]);
        }

        case CHEST_OP_REVOKE: {
            if (payload_size != 8) return ERROR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_revoke(args[0], args[1]);
        }

        case CHEST_OP_QUERY_TOKEN: {
            if (payload_size != 8) return ERROR_INVALID_ARGUMENT;
            uint32_t* args = (uint32_t*)payload;
            return chest_query_token(args[0], (uint32_t*)args[1]);
        }

        default:
            return ERROR_INVALID_OPERATION;
    }
}

// Optionally, provide a way to initialize and manage tokens externally
int chest_create_token(uint32_t id, uint32_t caps) {
    for (int i = 0; i < MAX_TOKENS; i++) {
        if (!token_table[i].valid) {
            token_table[i].id = id;
            token_table[i].capabilities = caps;
            token_table[i].valid = 1;
            return SUCCESS;
        }
    }
    return ERROR_NO_SPACE;
}

int chest_invalidate_token(uint32_t id) {
    chest_token_t* tok = find_token(id);
    if (!tok) return ERROR_INVALID_TOKEN;
    tok->valid = 0;
    return SUCCESS;
}
