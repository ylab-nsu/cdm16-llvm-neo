SECTIONS {
    .isr_vector 0 : {
        *(.isr_vector)
    } > IVT

    .text : ALIGN(2) {
        *(.text*)
        *(.rodata*)
    } > RAM

    .data : ALIGN(2) {
        *(.data*)
    } > RAM

    .bss : ALIGN(2) {
        *(.bss*)
        *(COMMON)
    } > RAM
}

OUTPUT_FORMAT(binary)
