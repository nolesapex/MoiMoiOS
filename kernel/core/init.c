// MoiMoiOS Kernel Initialization

#include "types.h"
#include "traps.h"
#include "scheduler.h"
#include "memory.h"
#include "devices.h"
#include "log.h"
#include "panic.h"

void kernel_main() {
    klog("\n=== MoiMoiOS Kernel Boot ===\n");

    klog("[0] Initializing Trap Vector...\n");
    if (!init_trap_dispatch()) {
        panic("[FATAL] Failed to initialize trap dispatch.");
    }

    klog("[1] Starting Scheduler Subsystem...\n");
    if (!init_scheduler()) {
        panic("[FATAL] Scheduler initialization failed.");
    }

    klog("[2] Preparing Memory Manager...\n");
    if (!init_memory_manager()) {
        panic("[FATAL] Memory manager failed to initialize.");
    }

    klog("[3] Loading Device Manager...\n");
    if (!init_device_manager()) {
        panic("[FATAL] Device manager initialization failed.");
    }

    klog("[OK] System services ready. Booting task scheduler...\n");
    scheduler_start();

    panic("[FATAL] scheduler_start() returned unexpectedly.");
}
