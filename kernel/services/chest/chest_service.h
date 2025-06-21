#ifndef CHEST_SERVICE_H
#define CHEST_SERVICE_H

#include <stdint.h>

// Capability definitions (bitmask)
#define CAP_IO     0x01
#define CAP_MEM    0x02
#define CAP_SCHED  0x04
#define CAP_FS     0x08
#define CAP_NET    0x10

// Chest operation codes
#define CHEST_OP_RUN_COMMAND     0x01
#define CHEST_OP_REQUEST_PROFILE 0x02
#define CHEST_OP_DELEGATE        0x03
#define CHEST_OP_REVOKE          0x04
#define CHEST_OP_QUERY_TOKEN     0x05

// Result codes
#define SUCCESS               0
#define ERROR_ACCESS_DENIED   -1
#define ERROR_INVALID_TOKEN   -2
#define ERROR_INVALID_ARGUMENT -3
#define ERROR_INVALID_OPERATION -4
#define ERROR_NO_SPACE        -5

// Public Chest service interface
int chest_service_handler(uint32_t operation, void* payload, uint32_t payload_size, uint32_t token_id);

// Token management (kernel internal use)
int chest_create_token(uint32_t id, uint32_t caps);
int chest_invalidate_token(uint32_t id);

#endif // CHEST_SERVICE_H
