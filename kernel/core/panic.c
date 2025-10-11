#include "panic.h"
#include "klog.h"

// A very basic kernel panic function.
// In a real OS, this would save state, dump registers, and halt.
void panic(const char* message) {
    klog("[PANIC] %s\n", message);
    // For now, just halt the CPU.
    asm volatile("cli\nhlt");
    while (1) { /* loop forever */ }
}

