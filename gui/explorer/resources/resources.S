.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin back_button, "./resources/back.fpic"
incbin up_arrow, "./resources/up_arrow.fpic"
incbin down_arrow, "./resources/down_arrow.fpic"