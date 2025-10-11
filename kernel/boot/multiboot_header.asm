; MoiMoiOS Multiboot2 Header
; This file defines the Multiboot2 header required by GRUB to load the kernel.

.section .multiboot

.align 8
multiboot_header:
    .long 0xe85250d6            ; Multiboot2 magic number (MB2_MAGIC)
    .long 0                     ; Architecture (0 for i386)
    .long multiboot_header_end - multiboot_header ; Header length
    ; Checksum (magic + arch + length + checksum = 0)
    .long -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header))

    ; Tag: End tag (required)
    .word 0                     ; Type (MB2_TAG_END)
    .word 0                     ; Flags
    .long 8                     ; Size (8 bytes for end tag)
multiboot_header_end:


