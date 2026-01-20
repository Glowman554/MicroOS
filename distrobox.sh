#!/bin/bash

DISTROBOX_NAME="microos-build-box"

if distrobox list | grep -q "$DISTROBOX_NAME "; then
    echo "Distrobox '$DISTROBOX_NAME' already exists."
else
    distrobox-create --name $DISTROBOX_NAME --image debian:latest

    distrobox-enter $DISTROBOX_NAME -- /bin/bash -c "
    sudo apt update
    sudo apt install -y xorriso mtools grub-pc-bin zip gcc-i686-linux-gnu qemu-system-x86 nasm curl clang llvm clangd git build-essential

    git config --global user.email \"vossjanick62@gmail.com\"
    git config --global user.name \"Glowman554\"
    git config --global credential.helper 'store --file ~/.my-credentials'

    curl -fsSL \"https://code.visualstudio.com/sha/download?build=stable&os=linux-deb-x64\" -o vscode.deb
    sudo dpkg -i vscode.deb
    sudo apt -f install -y
    rm vscode.deb

    curl -fsSL https://deno.land/install.sh | sh

    code --install-extension llvm-vs-code-extensions.vscode-clangd
    code --install-extension streetsidesoftware.code-spell-checker
    code --install-extension sankooc.pcapviewer
    "
fi

distrobox-enter $DISTROBOX_NAME -- code .
