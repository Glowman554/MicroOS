TARGET=i686-linux-gnu
PREFIX=$(pwd)/.cross

BINUTILS_VERSION=2.42
GCC_VERSION=13.2.0

mkdir -p .cross-build
(
    cd .cross-build
    
    if [ ! -f "binutils-$BINUTILS_VERSION.tar.xz" ]; then
        wget -q --show-progress "https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz"
        tar xf binutils-$BINUTILS_VERSION.tar.xz
        mkdir -p build-binutils
    fi

    if [ ! -f "gcc-$GCC_VERSION.tar.xz" ]; then
        wget -q --show-progress "https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz"
        tar xf gcc-$GCC_VERSION.tar.xz
        mkdir -p build-gcc
    fi
            


    (
        cd build-binutils
        ../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror --disable-gprofng
        make -j$(nproc) && make install
    )

    (
        cd build-gcc
        ../gcc-$GCC_VERSION/configure \
            --target=$TARGET \
            --prefix=$PREFIX \
            --enable-languages=c,c++ \
            --without-headers \
            --disable-nls \
            --disable-threads \
            --disable-shared \
            --disable-libssp \
            --disable-libquadmath \
            --disable-libgomp
        make all-gcc -j$(nproc)
        make install-gcc

        make all-target-libgcc -j$(nproc)
        make install-target-libgcc
    )
)