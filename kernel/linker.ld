SECTIONS
{
    . = 0x100000; /* Start loading at 1MB */

    .text : {
        *(.text) /* Load all .text sections */
    }

    .rodata : {
        *(.rodata) /* Load all .rodata sections */
    }

    .data : {
        *(.data) /* Load all .data sections */
    }

    .bss : {
        *(.bss COMMON) /* Load all .bss sections and zero them */
    }
}
