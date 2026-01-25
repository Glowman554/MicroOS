.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin kill_button, "./resources/exit.fpic"