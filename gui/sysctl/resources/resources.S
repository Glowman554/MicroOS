.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin reboot, "./resources/reboot.fpic"
incbin shutdown, "./resources/shutdown.fpic"