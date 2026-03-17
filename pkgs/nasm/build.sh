if [ ! -d "src" ]; then
    git clone https://github.com/netwide-assembler/nasm src
fi

(
    cd src

    ./autogen.sh
    
    CC=$TOOLCHAIN/bin/i686-microos-gcc ./configure --host=i686-elf --build=i686-pc-linux-gnu

    make -j$(nproc)
)


bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/nasm_install.mex