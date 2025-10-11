#ifndef CHEST_SERVICE_H
#define CHEST_SERVICE_H

#include <stdint.h>
#include "../../core/error.h" // New include for kernel_error_t

// Capability definitions (bitmask)
// More granular capabilities will be defined in a later phase.
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
// ... add more granular capabilities as needed

// Chest operation codes
#define CHEST_OP_RUN_COMMAND     0x01
#define CHEST_OP_REQUEST_PROFILE 0x02
#define CHEST_OP_DELEGATE        0x03
#define CHEST_OP_REVOKE          0x04
#define CHEST_OP_QUERY_TOKEN     0x05

// Public Chest service interface
kernel_error_t chest_service_handler(uint32_t operation, void* payload, uint32_t payload_size, uint32_t token_id);

// Token management (kernel internal use)
kernel_error_t chest_create_token(uint32_t id, uint32_t caps);
kernel_error_t chest_invalidate_token(uint32_t id);
kernel_error_t chest_check_capability(uint32_t token_id, uint32_t required_caps);

#endif // CHEST_SERVICE_H
