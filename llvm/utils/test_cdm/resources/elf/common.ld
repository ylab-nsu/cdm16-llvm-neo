SECTIONS {
    .isr_vector 0 : {
        *(.isr_vector)
    }

    .text : ALIGN(2) {
        *(.text*)
        *(.rodata*)
    }

    .data : ALIGN(2) {
        *(.data*)
    }

    .bss : ALIGN(2) {
        *(.bss*)
        *(COMMON)
    }
}

OUTPUT_FORMAT(binary)
