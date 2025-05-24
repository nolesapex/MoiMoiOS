// Simple notification service stub for MoiMoiOS
#include <stdio.h>

void notify(const char *message) {
    printf("[NOTIFICATION]: %s\n", message);
}

// Initialization
void notification_service_init() {
    printf("Notification Service Started\n");
    // Ready for IPC/command in future expansion
}
