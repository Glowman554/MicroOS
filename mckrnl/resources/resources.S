.macro incbin name file
    .section .rodata
    .global \name
    .global \name\()_size
\name:
    .incbin "\file"
\name\()_size: .int . - \name
.endm

incbin smp_trampoline, "./resources/smp_trampoline.bin"
incbin imposter_keymap, "./resources/default_keymap.mkm"
incbin idle_task, "./resources/idle_task.bin"
incbin imposter_font, "./resources/zap-light16.psf"