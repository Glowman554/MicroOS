.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin back_button, "./windows/explorer/resources/back.fpic"
incbin up_arrow, "./windows/explorer/resources/up_arrow.fpic"
incbin down_arrow, "./windows/explorer/resources/down_arrow.fpic"
