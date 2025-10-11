// ==========================================================
// MoiMoiOS - trap.c (Revised Modular Version)
// ----------------------------------------------------------
// Implements modular trap dispatch with syscall table,
// integrates Chest capability checks, and keeps original
// IDT setup compatible with existing ASM stubs.
// ==========================================================

#include "trap.h"
#include "chest_service.h"
#include "io.h"
#include "types.h"
#include "error.h"
#include "klog.h"
#include "panic.h"

#define IDT_SIZE 256
#define INT_SYSCALL 0x80
#define MAX_SYSCALLS 256

// ----------------------------------------------------------
// IDT Structures
// ----------------------------------------------------------
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_SIZE];
extern void* isr_stub_table[];  // ISR handlers defined in assembly

// ----------------------------------------------------------
// Syscall Infrastructure
// ----------------------------------------------------------
static syscall_handler_t syscall_table[MAX_SYSCALLS];

// Register a syscall handler
void register_syscall(uint64_t syscall_num, syscall_handler_t handler) {
    if (syscall_num < MAX_SYSCALLS) {
        syscall_table[syscall_num] = handler;
        kprintf("[TRAP] Registered syscall #%llu\n", syscall_num);
    } else {
        kprintf("[TRAP] Failed to register syscall #%llu (out of range)\n", syscall_num);
    }
}

// ----------------------------------------------------------
// Trap Dispatcher
// ----------------------------------------------------------
void trap_dispatch(uint64_t trapno, uint64_t err, uint64_t rip) {

    // ------------------------------------------------------
    // Software Interrupt (Syscall)
    // ------------------------------------------------------
    if (trapno == INT_SYSCALL) {
        uint64_t syscall_num = 0;
        uint64_t arg1 = 0, arg2 = 0, arg3 = 0, token_id = 0;

        // Retrieve syscall arguments via calling convention
        asm volatile("movq %%rax, %0" : "=r"(syscall_num));
        asm volatile("movq %%rdi, %0" : "=r"(arg1));
        asm volatile("movq %%rsi, %0" : "=r"(arg2));
        asm volatile("movq %%rdx, %0" : "=r"(arg3));
        asm volatile("movq %%rcx, %0" : "=r"(token_id));

        if (syscall_num >= MAX_SYSCALLS || syscall_table[syscall_num] == NULL) {
            kprintf("[TRAP] Invalid syscall: %llu (RIP: %p)\n", syscall_num, (void*)rip);
            asm volatile("movl %0, %%eax" :: "r"(ERR_INVALID_OPERATION));
            return;
        }

        // Global syscall capability check
        kernel_error_t cap_result = chest_check_capability((uint32_t)token_id, CAP_SYSCALL);
        if (cap_result != ERR_NONE) {
            kprintf("[TRAP] Access denied for syscall %llu | Token %llu\n", syscall_num, token_id);
            asm volatile("movl %0, %%eax" :: "r"(ERR_ACCESS_DENIED));
            return;
        }

        // Execute syscall handler
        int result = syscall_table[syscall_num](arg1, arg2, arg3, token_id);
        asm volatile("movl %0, %%eax" :: "r"(result)); // Return result
        return;
    }

    // ------------------------------------------------------
    // Default CPU Exception Handler
    // ------------------------------------------------------
    kprintf("[TRAP] Interrupt: %llu | Error: 0x%llx | RIP: %p\n", trapno, err, (void*)rip);

    if (trapno < 32) {
        panic("Unhandled CPU exception");
    }
}

// ----------------------------------------------------------
// IDT Setup
// ----------------------------------------------------------
void set_idt_entry(int vector, void* handler) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low  = addr & 0xFFFF;
    idt[vector].selector    = 0x08;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = 0x8E;  // Present, ring 0, interrupt gate
    idt[vector].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[vector].offset_high = (addr >> 32);
    idt[vector].zero        = 0;
}

void init_trap_dispatch() {
    for (int i = 0; i < IDT_SIZE; i++) {
        set_idt_entry(i, isr_stub_table[i]);
    }

    struct idt_ptr idtp;
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint64_t)&idt;

    asm volatile("lidt %0" :: "m"(idtp));
    asm volatile("sti");  // Enable interrupts

    for (int i = 0; i < MAX_SYSCALLS; i++)
        syscall_table[i] = NULL;

    kprintf("[TRAP] IDT initialized and syscall table ready.\n");
}

