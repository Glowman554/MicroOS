.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin reboot_button, "./windows/sysctl/resources/reboot.fpic"
incbin shutdown_button, "./windows/sysctl/resources/shutdown.fpic"
