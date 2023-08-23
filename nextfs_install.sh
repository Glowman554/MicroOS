git clone https://github.com/Glowman554/nextfs-loader --recurse

(
    cd nextfs-loader

    make -C loader
    make -C tools

    ./tools/fstool.elf ./loader/loader_mb.bin ../mckrnl/mckrnl.elf:mckrnl.elf ../initrd/keymap.mkm:keymap.mkm ../user/bin/init.elf:init.elf ../user/bin/terminal.elf:terminal.elf ../user/bin/ls.elf:ls.elf ../user/bin/tasks.elf:tasks.elf ../user/bin/env.elf:env.elf
    mv fs.nfs ../res/nextfs.img
    ./tools/fstool.elf ./loader/loader_mb.bin ../mckrnl/mckrnl.elf:mckrnl.elf ../initrd/keymap.mkm:keymap.mkm ../user/bin/init.elf:init.elf ../user/bin/terminal.elf:terminal.elf
    mv fs.nfs ../res/nextfs.minimal.img
)

rm -rfv nextfs-loader