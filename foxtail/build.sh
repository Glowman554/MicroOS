export PROGRAM_MK=$PWD/build/program.mk
export LIBS_ZIP=$PWD/../libs.zip
export FOXTAIL_PY=$PWD/foxtail.py

function build_dir {
	echo "Building $1"
	(
		cd $1
		bash build.sh $PROGRAM_MK $LIBS_ZIP $FOXTAIL_PY
	)
}

if [ -d "pkgs" ]; then
	rm -rvf pkgs
fi

mkdir -p pkgs

build_dir brainasm
build_dir phoenix_tools
build_dir phoenixv2_tools
build_dir nyanmbr

(
	cd pkgs
	echo export ACCEPT_ALL=1 > all.msh
	ls *.elf >> all.msh
)

mkdir ../res/initrd/pkgs -p
cp pkgs/* ../res/initrd/pkgs -v
