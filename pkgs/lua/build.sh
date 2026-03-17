if [ ! -d "src" ]; then
    git clone https://github.com/lua/lua src
fi

make -C src CC=$TOOLCHAIN/bin/i686-microos-gcc CFLAGS="-Wall -O2 \$(MYCFLAGS) -fno-stack-protector -fno-common" MYCFLAGS="-std=c99" MYLIBS="" -j$(nproc)

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/lua_install.mex