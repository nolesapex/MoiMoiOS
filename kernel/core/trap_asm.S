.section .text
.global isr_stub_table
.extern trap_dispatch

isr_stub_table:
    .rept 256
    .quad isr_stub_entry
    .endr

isr_stub_entry:
    cli
    pushq $0          // Dummy error code
    pushq $0          // Dummy vector
    call trap_dispatch
    addq $16, %rsp
    iretq
