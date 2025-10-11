/* MoiMoiOS Kernel Linker Script */

ENTRY(kernel_entry)

SECTIONS {
    . = 0x100000; /* Kernel starts at 1MB */

    .text ALIGN (0x1000) : {
        *(.multiboot)
        *(.text)
    }

    .rodata ALIGN (0x1000) : {
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        *(.data)
    }

    .bss ALIGN (0x1000) : {
        *(.bss)
    }

    /DISCARD/ : {
        *(.fini)
        *(.eh_frame)
    }
}
