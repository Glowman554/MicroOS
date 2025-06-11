.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin smp_trampoline, "./resources/smp_trampoline.bin"
incbin default_keymap, "./resources/default_keymap.mkm"
incbin idle_task, "./resources/idle_task.bin"
incbin default_font, "./resources/zap-light16.psf"