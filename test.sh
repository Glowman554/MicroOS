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

function testProfile {
    make clean -i
    applyPreset $1

    make iso
    cp cdrom.iso test_out/$1.iso

    screenshot test_out/$1.jpg
}

mkdir test_out -p

testProfile default
testProfile full_screen_terminal
testProfile network
testProfile network_full_screen_terminal