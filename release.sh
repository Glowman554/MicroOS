set -ex

source tools/filething.sh

UPLOAD_SERVER="https://filething.toxicfox.de"
UPLOAD_AUTH_TOKEN="$MICROOS_UPLOAD_TOKEN"
WEBHOOK_URL="$MESSAGE_WEBHOOK"
BUILD_TOKEN="$MICROOS_BUILD_TOKEN"

function do_release {
    if [[ "$#" -lt 9 ]]; then
        echo "Usage: release <name> <cdrom> <cdromMinimal> <libs> <message> <screenshot> <kernel> <symbols> <initrd>"
        exit 1
    fi

    local name="$1"
    local cdrom="$2"
    local cdromMinimal="$3"
    local libs="$4"
    local message="$5"
    local screenshot="$6"
    local kernel="$7"
    local symbols="$8"
    local initrd="$9"

    echo "Releasing: $name"

    send_webhook "$name" "$cdrom" "$cdromMinimal" "$libs" "$message" "$screenshot" "$kernel" "$symbols" "$initrd"
    trigger_build "$name" "$kernel" "$symbols" "$initrd"
}

function send_webhook {
    local name="$1"
    local cdrom="$2"
    local cdromMinimal="$3"
    local libs="$4"
    local message="$5"
    local screenshot="$6"
    local kernel="$7"
    local symbols="$8"
    local initrd="$9"

    curl -s -X POST "$WEBHOOK_URL" \
        -H "Content-Type: application/json" \
        -d @- <<EOF
{
  "username": "Website",
  "embeds": [
    {
      "title": "New release (${name})",
      "fields": [
        { "name": "cdrom", "value": "${cdrom}" },
        { "name": "cdrom (no packages)", "value": "${cdromMinimal}" },
        { "name": "libs", "value": "${libs}" },
        { "name": "message", "value": "${message}" },
        { "name": "screenshot", "value": "${screenshot}" },
        { "name": "kernel", "value": "${kernel}" },
        { "name": "symbols", "value": "${symbols}" },
        { "name": "initrd", "value": "${initrd}" }
      ],
      "image": { "url": "${screenshot}" }
    }
  ]
}
EOF
}

function trigger_build {
    local name="$1"
    local kernel="$2"
    local symbols="$3"
    local initrd="$4"

    response=$(curl -s -w "\n%{http_code}" -X POST \
        "https://toxicfox.de/api/v1/microos/build" \
        -H "Content-Type: application/json" \
        -H "Authentication: ${BUILD_TOKEN}" \
        -d @- <<EOF
{
  "preset": "${name}",
  "kernel": "${kernel}",
  "symbols": "${symbols}",
  "initrd": "${initrd}"
}
EOF
)

  body=$(echo "$response" | head -n1)
  status=$(echo "$response" | tail -n1)

  echo "Build trigger response: $status $body"
}




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
    # deno run -A release.ts screenshot::$(upload_file microos.jpg) name::$1 cdrom::$(upload_file cdrom.iso) cdromMinimal::$(upload_file cdrom.minimal.iso) libs::$(upload_file libs.zip) message::"$2" kernel::$(upload_file mckrnl/core/mckrnl.elf) symbols::$(upload_file mckrnl/core/mckrnl.syms) initrd::$(upload_file res/initrd.saf)
    local cdrom=$(upload_file "$2")
    local cdrom_minimal=$(upload_file "cdrom.minimal.iso")
    local libs=$(upload_file "libs.zip")
    local screenshot=$(upload_file "microos.jpg")
    local kernel=$(upload_file "mckrnl/core/mckrnl.elf")
    local symbols=$(upload_file "mckrnl/core/mckrnl.syms")
    local initrd=$(upload_file "res/initrd.saf")

    do_release "$1" "$cdrom" "$cdrom_minimal" "$libs" "$2" "$screenshot" "$kernel" "$symbols" "$initrd"
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