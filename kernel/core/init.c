// MoiMoiOS Kernel Initialization

#include "types.h"
#include "traps.h"
#include "scheduler.h"

void kernel_main() {
    init_trap_dispatch();
    init_scheduler();
    init_memory_manager();
    init_device_manager();

    scheduler_start();
}
