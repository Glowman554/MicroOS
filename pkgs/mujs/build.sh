if [ ! -d "src" ]; then
    git clone https://github.com/ArtifexSoftware/mujs src
fi


make -C src HAVE_READLINE=no CC=$TOOLCHAIN/bin/i686-microos-gcc

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/mujs_install.mex
