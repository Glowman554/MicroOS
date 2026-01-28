set -ex

source tools/filething.sh

UPLOAD_SERVER="https://filething.toxicfox.de"
UPLOAD_AUTH_TOKEN="$MICROOS_UPLOAD_TOKEN"

function applyPreset {
	deno run -A config/config.ts --clean --auto --load config/presets/$1.kernel.json config/kernel.json
	deno run -A config/config.ts --clean --auto --load config/presets/$1.libc.json config/libc.json
	deno run -A config/config.ts --clean --auto --mode makefile --load config/presets/$1.build.json config/build.json
}

function screenshot {
    make run_vnc  &>/dev/null & disown
    sleep 45
    vncsnapshot localhost:1 $1
    killall qemu-system-i386
}

function release {
    deno run -A release.ts screenshot::$(upload_file microos.jpg) name::$1 cdrom::$(upload_file cdrom.iso) cdromMinimal::$(upload_file cdrom.minimal.iso) libs::$(upload_file libs.zip) message::"$2" kernel::$(upload_file mckrnl/core/mckrnl.elf) symbols::$(upload_file mckrnl/core/mckrnl.syms) initrd::$(upload_file res/initrd.saf)
}

applyPreset $1

make iso libs.zip

cp cdrom.iso cdrom.minimal.iso
(
    cd pkgs
    bash build.sh
    bash clean.sh
)

make iso

screenshot microos.jpg

release $1 "$2"