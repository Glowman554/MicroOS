mkdir -p foxtail
python3 $3 pkg.json foxtail/install.c
make -f $1 -C foxtail PROGRAM=install.elf LIBS_ZIP=$2 extract_libs prog