#!/bin/bash

set -ex

TARGET=i686-microos
PREFIX="$(pwd)/../../.cross"
PREFIX=$(realpath "$PREFIX")
SYSROOT="$PREFIX/$TARGET"

STUBS="$(pwd)/stubs"
CRT0="$(pwd)/crt0.S"
LIBC="$(pwd)/../../user/libraries/libc"
LIBC=$(realpath "$LIBC")


BINUTILS_VERSION=2.42
GCC_VERSION=13.2.0

JOBS=$(nproc)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/.build/build"
SRC_DIR="$SCRIPT_DIR/.build/src"
BUILD_LOG="$SCRIPT_DIR/.build/build.log"

SHOULD_CLEAN=no

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() { echo -e "${GREEN}[TOOLCHAIN]${NC} $1"; }
warn() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
error() { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

run_silent() {
    echo "+ $*" >> "$BUILD_LOG"
    "$@" >> "$BUILD_LOG" 2>&1
}

create_dirs() {
    mkdir -p "$BUILD_DIR" "$SRC_DIR" "$PREFIX" "$SYSROOT"
    mkdir -p "$SYSROOT/usr/include" "$SYSROOT/lib"

    echo "=== MicroOS Toolchain Build Log ==="  > "$BUILD_LOG"
    echo "" >> "$BUILD_LOG"
}

download_sources() {
    log "Downloading sources..."
    cd "$SRC_DIR"
    
    if [ ! -f "binutils-$BINUTILS_VERSION.tar.xz" ]; then
        log "Downloading binutils $BINUTILS_VERSION..."
        wget -q --show-progress "https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz"
    fi
    
    if [ ! -f "gcc-$GCC_VERSION.tar.xz" ]; then
        log "Downloading GCC $GCC_VERSION..."
        wget -q --show-progress "https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz"
    fi
}

extract_sources() {
    log "Extracting sources..."
    cd "$SRC_DIR"
    
    if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
        log "Extracting binutils..."
        run_silent tar xf "binutils-$BINUTILS_VERSION.tar.xz"
    fi
    
    if [ ! -d "gcc-$GCC_VERSION" ]; then
        log "Extracting GCC..."
        run_silent tar xf "gcc-$GCC_VERSION.tar.xz"
    fi
}

patch_config_sub() {
    local file="$1"
    if [ -f "$file" ]; then
        if ! grep -q "microos" "$file"; then
            log "Patching $(basename $(dirname $file))/config.sub for microos..."
            # Add microos to the list of operating systems  
            sed -i '/linux-musl\*/a\	| microos* \\' "$file" 2>/dev/null || \
            sed -i 's/| linux-musl\* /| linux-musl* | microos* /' "$file" 2>/dev/null || true
        fi
    fi
}

apply_patches() {
    log "Applying patches..."
    
    local BINUTILS_DIR="$SRC_DIR/binutils-$BINUTILS_VERSION"
    local GCC_DIR="$SRC_DIR/gcc-$GCC_VERSION"
    local NEWLIB_DIR="$SRC_DIR/newlib-$NEWLIB_VERSION"
    
    # ===== BINUTILS PATCHES =====
    log "Patching binutils..."
    
    # Patch all config.sub files - add microos to valid OS list
    find "$BINUTILS_DIR" -name "config.sub" -exec sh -c '
        if ! grep -q "microos" "$1"; then
            # Add microos to the OS list (around line 1766 in modern config.sub)
            sed -i "s/| mlibc\* | cos\* | mbr\* )/| mlibc* | cos* | mbr* | microos* )/" "$1" 2>/dev/null || \
            sed -i "s/| fiwix\* | mlibc\*/| fiwix* | microos* | mlibc*/" "$1" 2>/dev/null || \
            # Fallback: add after fuchsia
            sed -i "s/| fuchsia\* /| fuchsia* | microos* /" "$1" 2>/dev/null || true
        fi
    ' _ {} \;
    
    # Patch bfd/config.bfd
    local bfd_config="$BINUTILS_DIR/bfd/config.bfd"
    if [ -f "$bfd_config" ] && ! grep -q "microos" "$bfd_config"; then
        log "Patching bfd/config.bfd..."
        # Insert microos target before i386-elf target (line ~590)
        sed -i '/i\[3-7\]86-\*-elf\* | i\[3-7\]86-\*-rtems/i\
  i[3-7]86-*-microos*)\
    targ_defvec=i386_elf32_vec\
    targ_selvecs="i386_elf32_vec"\
    ;;' "$bfd_config"
    fi
    
    # Patch gas/configure.tgt
    local gas_config="$BINUTILS_DIR/gas/configure.tgt"
    if [ -f "$gas_config" ] && ! grep -q "microos" "$gas_config"; then
        log "Patching gas/configure.tgt..."
        sed -i '/i386-\*-elf\*)/i\
  i386-*-microos*)			fmt=elf ;;' "$gas_config"
    fi
    
    # Patch ld/configure.tgt
    local ld_config="$BINUTILS_DIR/ld/configure.tgt"
    if [ -f "$ld_config" ] && ! grep -q "microos" "$ld_config"; then
        log "Patching ld/configure.tgt..."
        sed -i '/i\[3-7\]86-\*-elf\*/i\
i[3-7]86-*-microos*)	targ_emul=elf_i386\
			;;' "$ld_config"
    fi
    
    # ===== GCC PATCHES =====
    log "Patching GCC..."
    
    # Patch all config.sub files - add microos to valid OS list
    find "$GCC_DIR" -name "config.sub" -exec sed -i 's/| fiwix\* )/| microos* | fiwix* )/' {} \;
    
    # Patch libgcc/config.host for microos target
    local libgcc_config="$GCC_DIR/libgcc/config.host"
    if [ -f "$libgcc_config" ] && ! grep -q "microos" "$libgcc_config"; then
        log "Patching libgcc/config.host..."
        sed -i '/i\[34567\]86-\*-elf\*)/i\
i[34567]86-*-microos*)\
\ttmake_file="$tmake_file i386/t-crtstuff t-crtstuff-pic t-libgcc-pic"\
\t;;
' "$libgcc_config"
    fi
    
    # Patch gcc/config.gcc
    local gcc_config="$GCC_DIR/gcc/config.gcc"
    if [ -f "$gcc_config" ] && ! grep -q "microos" "$gcc_config"; then
        log "Patching gcc/config.gcc..."
        sed -i '/^i\[34567\]86-\*-elf\*)/i\
i[34567]86-*-microos*)\
	tm_file="${tm_file} i386/unix.h i386/att.h elfos.h newlib-stdint.h i386/i386elf.h i386/microos.h"\
	tmake_file="i386/t-i386elf t-svr4"\
	use_gcc_stdint=wrap\
	;;' "$gcc_config"
    fi
    
    # Add microos.h target header
    local microos_h="$GCC_DIR/gcc/config/i386/microos.h"
    if [ ! -f "$microos_h" ]; then
        log "Creating gcc/config/i386/microos.h..."
        cat > "$microos_h" << 'MICROOS_H'
/* MicroOS target header for GCC */

#define TARGET_OS_CPP_BUILTINS()                \
  do {                                          \
    builtin_define ("__microos__");             \
    builtin_define ("__unix__");                \
    builtin_assert ("system=microos");          \
    builtin_assert ("system=unix");             \
  } while (0)

/* Default link specification for MicroOS */
#undef LIB_SPEC
#define LIB_SPEC "-lc"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0.o%s"

#undef ENDFILE_SPEC  
#define ENDFILE_SPEC ""

/* MicroOS loads programs at 0xB0000000 */
#undef LINK_SPEC
#define LINK_SPEC "-Ttext=0xB0000000 %{shared:-shared} %{!shared:-static}"

/* Don't use PIC by default - MicroOS doesn't support it yet */
#undef CC1_SPEC
#define CC1_SPEC "%{!fpic:%{!fPIC:%{!fpie:%{!fPIE:-fno-pic -fno-pie}}}}"

/* No stack protector by default */
#undef SSP_DEFAULT
#define SSP_DEFAULT 0
MICROOS_H
    fi
 
    log "Patches applied successfully"
}

# Build binutils
build_binutils() {
    log "Building binutils..."
    mkdir -p "$BUILD_DIR/binutils"
    cd "$BUILD_DIR/binutils"
    
    if [ ! -f "Makefile" ]; then
        log "  Configuring binutils..."
        run_silent "$SRC_DIR/binutils-$BINUTILS_VERSION/configure" \
            --target=$TARGET \
            --prefix="$PREFIX" \
            --with-sysroot="$SYSROOT" \
            --disable-nls \
            --disable-werror
    fi
    
    log "  Compiling binutils..."
    run_silent make -j$JOBS
    log "  Installing binutils..."
    run_silent make install
    
    log "Binutils installed to $PREFIX"
}

deploy_headers() {
    log "Deploying headers to sysroot..."

    INCLUDE="$SYSROOT/include"
    mkdir -p "$INCLUDE"
    cp -r "$STUBS"/* "$INCLUDE/"
    cp -r "$LIBC/include"/* "$INCLUDE/"
}

# Build GCC (stage 1 - without libc)
build_gcc_stage1() {
    log "Building GCC (stage 1 - bootstrap)..."
    mkdir -p "$BUILD_DIR/gcc-stage1"
    cd "$BUILD_DIR/gcc-stage1"
    
    if [ ! -f "Makefile" ]; then
        log "  Configuring GCC stage 1..."
        run_silent "$SRC_DIR/gcc-$GCC_VERSION/configure" \
            --target=$TARGET \
            --prefix="$PREFIX" \
            --with-sysroot="$SYSROOT" \
            --disable-nls \
            --enable-languages=c \
            --disable-shared \
            --disable-threads \
            --disable-libssp \
            --disable-libquadmath \
            --disable-libgomp \
            --disable-libatomic \
            --disable-decimal-float \
            --disable-libffi
    fi
    
    log "  Compiling GCC stage 1..."
    run_silent make -j$JOBS all-gcc all-target-libgcc
    log "  Installing GCC stage 1..."
    run_silent make install-gcc install-target-libgcc

    log "GCC stage 1 installed to $PREFIX"
}

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --prefix=DIR    Install to DIR (default: \$HOME/microos-toolchain)"
    echo "  --jobs=N        Build with N parallel jobs (default: $(nproc))"
    echo "  --clean         Remove build directories after building"
    echo "  --help          Show this help message"
    echo ""
    exit 0
}

parse_args() {
    for arg in "$@"; do
        case "$arg" in
            --prefix=*)
                PREFIX="${arg#*=}"
                SYSROOT="$PREFIX/$TARGET"
                ;;
            --jobs=*)
                JOBS="${arg#*=}"
                ;;
            --clean)
                SHOULD_CLEAN=yes
                ;;
            --help|-h)
                usage
                ;;
            *)
                warn "Unknown option: $arg"
                ;;
        esac
    done
}

main() {
    parse_args "$@"
    create_dirs
    
    log "Building MicroOS cross-compiler toolchain"
    log "Target: $TARGET"
    log "Prefix: $PREFIX"
    log ""
    
    download_sources
    extract_sources
    apply_patches
    build_binutils
    deploy_headers
    build_gcc_stage1

    if [ "$SHOULD_CLEAN" = "yes" ]; then
        log "Cleaning build directories..."
        rm -rf "$BUILD_LOG" "$BUILD_DIR" "$SRC_DIR"
    fi

}

main "$@"