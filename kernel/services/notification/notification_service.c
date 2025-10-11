// ==========================================================
// MoiMoiOS - Notification Service (Enhanced)
// ----------------------------------------------------------
// Provides unified notification/logging support for kernel
// and user services, ready for IPC expansion.
// ==========================================================

#include "notification_service.h"
#include "klog.h"
#include "types.h"
#include "chest_service.h"

// ----------------------------------------------------------
// Notification Levels
// ----------------------------------------------------------
typedef enum {
    NOTIFY_INFO,
    NOTIFY_WARNING,
    NOTIFY_ERROR,
    NOTIFY_DEBUG
} notify_level_t;

// ----------------------------------------------------------
// Internal helper for formatting messages
// ----------------------------------------------------------
static const char* level_to_str(notify_level_t level) {
    switch (level) {
        case NOTIFY_INFO:    return "INFO";
        case NOTIFY_WARNING: return "WARN";
        case NOTIFY_ERROR:   return "ERROR";
        case NOTIFY_DEBUG:   return "DEBUG";
        default:             return "UNKNOWN";
    }
}

// ----------------------------------------------------------
// Public API
// ----------------------------------------------------------

// Generic notification call
void notify(notify_level_t level, const char *message) {
    if (!message) return;

    switch (level) {
        case NOTIFY_ERROR:
            kprintf("[NOTIFICATION - %s]: %s\n", level_to_str(level), message);
            // Future: raise system-level event, trigger alert
            break;

        default:
            kprintf("[NOTIFICATION - %s]: %s\n", level_to_str(level), message);
            break;
    }
}

// Capability-protected interface (optional for user services)
int notify_secure(uint32_t token_id, notify_level_t level, const char *message) {
    // Ensure the calling token has I/O privileges
    if (chest_check_capability(token_id, CAP_IO_WRITE) != ERR_NONE) {
        return ERR_ACCESS_DENIED;
    }
    notify(level, message);
    return ERR_NONE;
}

// ----------------------------------------------------------
// Initialization
// ----------------------------------------------------------
void notification_service_init() {
    kprintf("[SERVICE] Notification Service Started\n");

    // Example startup messages
    notify(NOTIFY_INFO, "Notification system initialized.");
    notify(NOTIFY_DEBUG, "Ready for IPC/command handling.");
}
