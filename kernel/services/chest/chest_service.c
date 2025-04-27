// MoiMoiOS Chest Service

#include "chest_service.h"
#include "chest_internal.h"
#include "security.h"

int chest_service_handler(uint32_t operation, void* payload, uint32_t payload_size, uint32_t token_id) {
    if (!security_validate_token(token_id, CAPABILITY_CHEST)) {
        return ERROR_ACCESS_DENIED;
    }

    switch (operation) {
        case CHEST_OP_RUN_COMMAND:
            return chest_run_command((char*)payload, payload_size);
        case CHEST_OP_REQUEST_PROFILE:
            return chest_request_profile((char*)payload);
        default:
            return ERROR_INVALID_OPERATION;
    }
}
