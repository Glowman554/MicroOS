export PROGRAM_MK=$(realpath $PWD/build/program.mk)
export LIBRARY_MK=$(realpath $PWD/build/library.mk)
export LIBS_ZIP=$(realpath $PWD/../libs.zip)
export INSTALLER_PY=$(realpath $PWD/installer.py)
export TOOLCHAIN=$(realpath $PWD/.toolchain)

if [ ! -d $TOOLCHAIN ]; then
	curl -SL https://github.com/Glowman554/MicroOS-ports/releases/download/toolchain-latest/toolchain.tar.gz -o /tmp/toolchain.tar.gz
	mkdir -p $TOOLCHAIN
	tar -xzf /tmp/toolchain.tar.gz -C $TOOLCHAIN --strip-components=1
fi

function build_dir {
	echo "Building $1"
	(
		cd $1
		bash build.sh
	)
}

if [ -d "pkgs" ]; then
	rm -rvf pkgs
fi

mkdir -p pkgs

(
	build_dir brainasm &
	build_dir phoenix_tools &
	build_dir phoenixv2_tools &
	build_dir nyanmbr &
	build_dir doom &
	build_dir saf &
	build_dir nextfs &
	build_dir doom_desktop &
	build_dir flc &
	build_dir jvm &
	build_dir lua &
	build_dir mujs &
	build_dir nasm &
	wait
)

(
	cd pkgs
	echo "#!/bin/terminal.mex" > all.msh
	echo "cd \$ROOT_FS/pkgs" >> all.msh
	echo export ACCEPT_ALL=1 >> all.msh
	ls *.elf >> all.msh
	ls *.mex >> all.msh
)

mkdir ../res/initrd/pkgs -p
cp pkgs/* ../res/initrd/pkgs -v
