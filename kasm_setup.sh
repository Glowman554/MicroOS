sudo apt update
sudo apt upgrade -y
sudo apt install -y xorriso mtools grub-pc-bin zip gcc-i686-linux-gnu qemu-system-x86 vncsnapshot curl nasm clangd
curl -fsSL https://deno.land/install.sh | sh

git config --global user.email "glowman434@gmail.com"
git config --global user.name "Glowman554"
git config --global credential.helper 'store --file ~/.my-credentials'

code --install-extension llvm-vs-code-extensions.vscode-clangd
code --install-extension sankooc.pcapviewer
