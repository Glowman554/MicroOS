git clone https://github.com/Glowman554/nextfs-loader --recurse

(
    cd nextfs-loader

    make -C loader
    make -C tools

    ./tools/fstool.elf ./loader/loader_mb.bin ../mckrnl/mckrnl.elf:kernel.elf ../initrd/keymap.mkm:keymap.mkm ../user/bin/init.mex:init.mex ../user/bin/terminal.mex:terminal.mex ../user/bin/ls.mex:ls.mex ../user/bin/tasks.mex:tasks.mex ../user/bin/env.mex:env.mex
    mv fs.nfs ../res/nextfs.img
    ./tools/fstool.elf ./loader/loader_mb.bin ../mckrnl/mckrnl.elf:kernel.elf ../initrd/keymap.mkm:keymap.mkm ../user/bin/init.mex:init.mex ../user/bin/terminal.mex:terminal.mex
    mv fs.nfs ../res/nextfs.minimal.img
)

rm -rfv nextfs-loader