; MoiMoiOS Kernel Entry Point
; This file defines the entry point for the kernel after being loaded by a Multiboot2-compliant bootloader.

.section .text
.global kernel_entry
.type kernel_entry, @function

kernel_entry:
    ; Disable interrupts
    cli

    ; Set up a stack
    mov $stack_top, %rsp

    ; Multiboot2 specification states that EBX contains the address of the Multiboot information structure
    ; and EAX contains the magic number. These are passed as arguments to kernel_main.
    push %ebx
    push %eax

    ; Call the C kernel_main function
    call kernel_main

    ; If kernel_main returns, halt the CPU
hlt_loop:
    hlt
    jmp hlt_loop

.section .bss
.align 4096
stack_bottom:
    .skip 16384 ; 16KB stack
stack_top:

