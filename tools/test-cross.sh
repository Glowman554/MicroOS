function test {
    CC=$1/bin/i686-microos-gcc
    echo "Testing $CC"

    cat <<EOF > /tmp/hello.c
#include <stdio.h>

int main() {
    printf("Hello, world!\\n");
    return 0;
}
EOF

    $CC -o $2 /tmp/hello.c
}

test ../.cross ../res/initrd/hello.elf
test ../.cross-newlib ../res/initrd/hello-newlib.elf