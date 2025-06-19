// MoiMoiOS - trap.c
#include "trap.h"
#include "chest_service.h"
#include "io.h"
#include "types.h"

#define IDT_SIZE 256

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

// Trap dispatcher
void trap_dispatch(uint64_t trapno, uint64_t err, uint64_t rip) {
    if (trapno == 0x80) { // Software interrupt — service call
        uint32_t operation, token;
        void* payload;

        asm volatile("movl %%edi, %0" : "=r"(operation));
        asm volatile("movl %%esi, %0" : "=r"(payload));
        asm volatile("movl %%edx, %0" : "=r"(token));

        int result = chest_service_handler(operation, payload, 0, token);

        asm volatile("movl %0, %%eax" :: "r"(result)); // Return result
        return;
    }

    // Default handler for CPU exceptions
    kprintf("[TRAP] Interrupt: %d | Error: %x | RIP: %p\n", trapno, err, rip);

    if (trapno < 32) {
        panic("Unhandled CPU exception");
    }
}

// IDT setup
void set_idt_entry(int vector, void* handler) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].offset_low  = addr & 0xFFFF;
    idt[vector].selector    = 0x08;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = 0x8E; // Present, ring 0, interrupt gate
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

    kprintf("[TRAP] IDT initialized\n");
}
