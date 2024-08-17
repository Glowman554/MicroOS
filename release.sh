set -ex

function applyPreset {
	deno run -A config/config.ts --clean --auto --load config/presets/$1.kernel.json config/kernel.json
	deno run -A config/config.ts --clean --auto --load config/presets/$1.libc.json config/libc.json
}

function screenshot {
    make run_vnc  &>/dev/null & disown
    sleep 45
    vncsnapshot localhost:1 $1
    killall qemu-system-i386
}

function uploadFile {
    local url=$(curl -F "reqtype=fileupload" -F "fileToUpload=@$1" https://catbox.moe/user/api.php)
    echo $url
}

function release {
    deno run -A release.ts screenshot::$(uploadFile microos.jpg) name::$1 cdrom::$(uploadFile cdrom.iso) cdromMinimal::$(uploadFile cdrom.minimal.iso) libs::$(uploadFile libs.zip) message::"$2"
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