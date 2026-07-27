/*
 * MoiMoiOS - scheduler.c
 * Implements a basic Round Robin scheduler for tasks.
 */

#include "scheduler.h"
#include "klog.h"
#include "panic.h"
#include "error.h"
#include "io.h" // For basic I/O if needed for debugging

// --- External Dependencies (placeholders for now) ---
// These functions would be provided by the Memory Management Subsystem
// and potentially the VMM.
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);
extern uint64_t vmm_get_kernel_pml4(); // Placeholder for getting kernel's PML4

// --- Scheduler State ---
static task_t* current_task = NULL;
static task_t* run_queue_head = NULL;
static task_t* idle_task = NULL;
static uint32_t next_pid = 1;

#define KERNEL_STACK_SIZE 4096 // 4KB kernel stack per task

// --- Private Helper Functions ---

// Initializes a new task's CPU state on its stack
static void setup_task_stack(task_t* task, void* entry_point) {
    // Stack grows downwards. Set up initial stack frame for context_switch.
    uint64_t* stack = (uint64_t*)((uint64_t)task->stack_base + KERNEL_STACK_SIZE);

    // Simulate an interrupt return frame for the first execution
    *(--stack) = 0x202; // RFLAGS: Interrupts enabled, bit 1 always 1
    *(--stack) = (uint64_t)entry_point; // RIP: Entry point of the task

    // Dummy values for segment selectors (CS, SS) and error code, etc.
    // These are usually pushed by the CPU during an interrupt.
    // For a clean start, we'll push dummy values or ensure they are not used.
    *(--stack) = 0x08; // CS (Kernel Code Segment)
    *(--stack) = 0;    // Error code (dummy)
    *(--stack) = 0;    // Interrupt number (dummy)

    // General purpose registers (pushed by context_switch before saving)
    *(--stack) = 0; // R15
    *(--stack) = 0; // R14
    *(--stack) = 0; // R13
    *(--stack) = 0; // R12
    *(--stack) = 0; // R11
    *(--stack) = 0; // R10
    *(--stack) = 0; // R9
    *(--stack) = 0; // R8
    *(--stack) = 0; // RDI
    *(--stack) = 0; // RSI
    *(--stack) = 0; // RDX
    *(--stack) = 0; // RCX
    *(--stack) = 0; // RBX
    *(--stack) = 0; // RAX
    *(--stack) = 0; // RBP

    task->cpu_state.rsp = (uint64_t)stack;
    task->cpu_state.cr3 = vmm_get_kernel_pml4(); // Use kernel's page table for now
}

// The idle task function - runs when no other tasks are ready
static void idle_function() {
    klog("[SCHED] Idle task started.\n");
    while (1) {
        asm volatile("hlt"); // Halt the CPU until the next interrupt
    }
}

// --- Public Scheduler Functions ---

bool init_scheduler() {
    klog("[SCHED] Initializing scheduler...\n");

    // Initialize the run queue
    run_queue_head = NULL;

    // Create the idle task
    uint32_t idle_pid;
    if (create_task(idle_function, 0, &idle_pid) != ERR_NONE) {
        panic("[FATAL] Failed to create idle task.");
    }
    idle_task = run_queue_head; // Idle task is the first and always in the queue
    current_task = idle_task;

    klog("[SCHED] Scheduler initialized. Idle task (PID %u) created.\n", idle_pid);
    return true;
}

kernel_error_t create_task(void* entry_point, uint32_t capabilities, uint32_t* out_pid) {
    task_t* new_task = (task_t*)kmalloc(sizeof(task_t));
    if (!new_task) {
        klog("[SCHED] Failed to allocate TCB for new task.\n");
        return ERR_NO_SPACE;
    }

    new_task->stack_base = (uint64_t)kmalloc(KERNEL_STACK_SIZE);
    if (!new_task->stack_base) {
        klog("[SCHED] Failed to allocate stack for new task.\n");
        kfree(new_task);
        return ERR_NO_SPACE;
    }

    new_task->pid = next_pid++;
    new_task->capabilities = capabilities;
    new_task->next = NULL;

    setup_task_stack(new_task, entry_point);

    // Add to run queue (simple singly linked list for now)
    if (run_queue_head == NULL) {
        run_queue_head = new_task;
        new_task->next = new_task; // Point to itself for a circular list with one element
    } else {
        task_t* temp = run_queue_head;
        while (temp->next != run_queue_head) {
            temp = temp->next;
        }
        temp->next = new_task;
        new_task->next = run_queue_head;
    }

    if (out_pid) {
        *out_pid = new_task->pid;
    }

    klog("[SCHED] Created new task with PID %u, entry %p.\n", new_task->pid, entry_point);
    return ERR_NONE;
}

void yield() {
    if (!current_task || !run_queue_head) {
        panic("[SCHED] Yield called with no current task or empty run queue.");
    }

    task_t* prev_task = current_task;
    current_task = current_task->next; // Simple Round Robin

    if (current_task == prev_task) {
        // Only one task in the system (e.g., just the idle task)
        return;
    }

    klog("[SCHED] Yielding from PID %u to PID %u.\n", prev_task->pid, current_task->pid);
    context_switch(&prev_task->cpu_state, &current_task->cpu_state);
}

void scheduler_start() {
    if (!current_task) {
        panic("[FATAL] Scheduler_start called with no current task. Did init_scheduler fail?");
    }

    klog("[SCHED] Starting scheduler, jumping to first task (PID %u).\n", current_task->pid);

    // This is the initial jump into the first task. It will not return.
    // The context_switch here is slightly different as it doesn't save a 'previous' state
    // because the kernel_main stack is not a task stack.
    // We need an assembly stub that directly loads the first task's state.
    // For simplicity, we'll call context_switch and rely on it not saving the current state.
    // A proper implementation might have a dedicated 'start_first_task' assembly function.
    asm volatile(
        "movq %0, %%rsp\n"
        "popq %%r15\n"
        "popq %%r14\n"
        "popq %%r13\n"
        "popq %%r12\n"
        "popq %%r11\n"
        "popq %%r10\n"
        "popq %%r9\n"
        "popq %%r8\n"
        "popq %%rdi\n"
        "popq %%rsi\n"
        "popq %%rdx\n"
        "popq %%rcx\n"
        "popq %%rbx\n"
        "popq %%rax\n"
        "popq %%rbp\n"
        "addq $16, %%rsp\n" // Skip dummy interrupt number and error code
        "iretq"
        :: "r"(current_task->cpu_state.rsp)
        : "memory"
    );

    // Should never reach here
    panic("[FATAL] scheduler_start() returned unexpectedly.");
}

task_t* get_current_task() {
    return current_task;
}

// --- Placeholder for context_switch assembly (to be in a separate .S file) ---
/*
// kernel/arch/x86_64/context_switch.S
.global context_switch
context_switch:
    // Save current CPU state (old_state)
    movq %rdi, %rsp // old_state is first arg, new_state is second
    movq %rsp, (%rdi) // Save RSP
    movq %rbp, 8(%rdi)
    movq %rax, 16(%rdi)
    movq %rbx, 24(%rdi)
    movq %rcx, 32(%rdi)
    movq %rdx, 40(%rdi)
    movq %rsi, 48(%rdi)
    movq %rdi, 56(%rdi)
    movq %r8, 64(%rdi)
    movq %r9, 72(%rdi)
    movq %r10, 80(%rdi)
    movq %r11, 88(%rdi)
    movq %r12, 96(%rdi)
    movq %r13, 104(%rdi)
    movq %r14, 112(%rdi)
    movq %r15, 120(%rdi)
    movq %cr3, %rax
    movq %rax, 128(%rdi) // Save CR3

    // Load new CPU state (new_state)
    movq %rsi, %rdi // new_state is second arg
    movq (%rdi), %rsp // Load RSP
    movq 8(%rdi), %rbp
    movq 16(%rdi), %rax
    movq 24(%rdi), %rbx
    movq 32(%rdi), %rcx
    movq 40(%rdi), %rdx
    movq 48(%rdi), %rsi
    movq 56(%rdi), %rdi
    movq 64(%rdi), %r8
    movq 72(%rdi), %r9
    movq 80(%rdi), %r10
    movq 88(%rdi), %r11
    movq 96(%rdi), %r12
    movq 104(%rdi), %r13
    movq 112(%rdi), %r14
    movq 120(%rdi), %r15
    movq 128(%rdi), %rax
    movq %rax, %cr3 // Load CR3

    ret
*/

// --- Dummy Memory Management Functions (for compilation) ---
// These need to be replaced by actual PMM/VMM/Heap implementations.
void* kmalloc(size_t size) {
    (void)size; // Unused parameter
    panic("[SCHED] kmalloc called before memory manager is ready!");
    return NULL;
}

void kfree(void* ptr) {
    (void)ptr; // Unused parameter
    panic("[SCHED] kfree called before memory manager is ready!");
}

uint64_t vmm_get_kernel_pml4() {
    // This should return the physical address of the kernel's top-level page table.
    // For now, return a dummy value or panic if not properly initialized.
    panic("[SCHED] vmm_get_kernel_pml4 called before VMM is ready!");
    return 0;
}
