#!/bin/bash

if [ ! -d "src" ]; then
    git clone https://github.com/phillbush/jvm src
    (
        cd src
        git am < ../0001-change-default-classpath.patch
    )
fi

(
    cd src

    make CC=$TOOLCHAIN/bin/i686-microos-gcc

    cd tests
    for test in *.java; do
        if [ -f "${test%.java}.class" ]; then
            continue
        fi
        echo "Compiling $test..."
        javac -d . -source 8 -target 8 $test
    done
)

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/jvm_install.mex