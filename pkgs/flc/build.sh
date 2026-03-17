#!/bin/bash


if [ ! -d "src" ]; then
    git clone https://github.com/Glowman554/FireStorm src
fi


make -C src/flc CC=$TOOLCHAIN/bin/i686-microos-gcc

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/flc_install.mex