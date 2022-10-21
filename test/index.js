var V86 = require("./libv86.js").V86;

var SCREEN_WIDTH = 80;

function get_line(screen, y) {
	return screen.subarray(y * SCREEN_WIDTH, (y + 1) * SCREEN_WIDTH);
}

function line_to_text(screen, y) {
	return bytearray_to_string(get_line(screen, y));
}

function bytearray_to_string(arr) {
	return String.fromCharCode.apply(String, arr);
}

function screen_to_text(s) {
	var result = [];
	result.push("+==================================== SCREEN ====================================+");

	for(var i = 0; i < 25; i++) {
		var line = line_to_text(s, i);
		result.push("|" + line + "|");
	}

	result.push("+================================================================================+");

	return result.join("\n");
}

var settings = {
	bios: {
		url: "seabios.bin"
	},
	vga_bios: {
		url: "vgabios.bin"
	},
	autostart: true,
	memory_size: 128 * 1024 * 1024,
	cdrom: {
		url: "../cdrom.iso"
	}
};

var screen = new Uint8Array(SCREEN_WIDTH * 25)

var emulator = new V86(settings);

emulator.add_listener("screen-put-char", function(chr) {
	var y = chr[0];
	var x = chr[1];
	var code = chr[2];
	screen[x + SCREEN_WIDTH * y] = code;
});

emulator.add_listener("serial0-output-char", function(chr) {
    if(chr <= "~") {
        process.stdout.write(chr);
    }
});

var timeout = 60;

setInterval(() => {
  timeout--;
  if (timeout == 0) {
    console.log(screen_to_text(screen));
    console.log("Test failed!");
    process.exit(-1);
  }

  if (bytearray_to_string(get_line(screen, 0)).startsWith("MicroOS Copyright (C) 2022 Glowman554") && bytearray_to_string(get_line(screen, 2)).startsWith("shell")) {
    console.log(screen_to_text(screen));
    console.log("Test passed!");
    process.exit(0);
  }
}, 1000);
