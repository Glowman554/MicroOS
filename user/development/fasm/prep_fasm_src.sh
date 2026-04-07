set -ex
git clone https://github.com/tgrysztar/fasm
(
	cd fasm
	git am < ../0001-fixed-includes.patch
	git am < ../0002-fixed-free-NULL-call.patch
)

fasm fasm/SOURCE/LIBC/FASM.ASM
cp fasm/SOURCE/LIBC/FASM.O .
