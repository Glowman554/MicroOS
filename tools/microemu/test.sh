make -B

MICROEMU=$(pwd)/microemu

cd ../../res/initrd


$MICROEMU root:/bin/init.mex tmpfs <<EOF
cd examples
cat hello.asm
fasm hello.asm hello.elf
hello.elf

mcc hello.c -o hello-c.elf
hello-c.elf

tree

pwr off
EOF

