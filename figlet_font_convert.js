const fs = require("fs");
const https = require("https");

var magic = new Uint32Array([0xf181babe]);
var offsets = new Uint32Array(0xff);

var data = new Uint8Array(0x10000);
var data_ptr = 0;

function str_to_uint8(str) {
	var arr = new Uint8Array(str.length + 1);
	for (var i = 0; i < str.length; i++) {
		arr[i] = str.charCodeAt(i);
	}
	arr[str.length] = 0;
	return arr;
}

function merge_uint8_array(arrayOne, arrayTwo) {
	var mergedArray = new Uint8Array(arrayOne.length + arrayTwo.length);
	mergedArray.set(arrayOne);
	mergedArray.set(arrayTwo, arrayOne.length);
	return mergedArray;
}

function figl_new() {
	return {
		magic: new Uint32Array([0xf181babe]),
		offsets: new Uint32Array(0xff),

		data: new Uint8Array(0x10000),
		data_ptr: 0
	};
}

function figl_register(char, char_str, figl) {
	figl.data.set(str_to_uint8(char_str), figl.data_ptr);
	figl.offsets[char] = figl.data_ptr;
	figl.data_ptr += char_str.length + 1;
}

function figl_save(filename, figl) {
	var new_arr = merge_uint8_array(merge_uint8_array(new Uint8Array(figl.magic.buffer), new Uint8Array(figl.offsets.buffer)), figl.data.slice(0, figl.data_ptr));
	console.log("Saving " + filename);
	fs.writeFileSync(filename, new_arr);
}

function http_get(url) {
	return new Promise(function(resolve, reject) {
		https.get(url, function(response) {
			if (response.statusCode == 302) {
				console.log("Redirecting to " + response.headers.location);
				http_get(response.headers.location).then(resolve);
			} else {
				resolve(response);
			}
		});
	});
}

function http_get_str(url) {
	return new Promise(function(resolve, reject) {
		http_get(url).then(function(data) {
			var contents = "";
			data.on('data', function (chunk) {
				contents += chunk;
			});
			data.on('end', function() {
				resolve(contents);
			});
		});
	});
}

function convert(font_name) {
	http_get_str("https://github.com/scottgonzalez/figlet-js/raw/master/fonts/" + font_name + ".flf").then(function(contents) {
		var lines = contents.split("\n");
		var header = lines[0].split(" ");
		var hardblank = header[0].charAt(header[0].length - 1);
		var height = +header[1];
		var comments = +header[5];
		
		var font = {
			defn: lines.slice(comments + 1),
			hardblank: hardblank,
			height: height,
		};

		var chars = [];

		for (let c = 32; c < 127; c++) {

			var height = font.height;
			var start = (c - 32) * height;
			var charDefn = [];
			for (let i = 0; i < height; i++) {
				charDefn[i] = font.defn[start + i].replace(/@/g, "").replace(RegExp("\\" + font.hardblank, "g"), " ");
			}

			chars[c] = charDefn.join("\n");
		}

		var figl = figl_new();

		for (let c = 0; c < chars.length; c++) {
			figl_register(c, chars[c] + "\n" || "", figl);
		}

		figl_save("initrd/fonts/" + font_name + ".figl", figl);
	});
}

function main() {
	var fonts = ["speed", "3-d", "graffiti"];
	for (let i = 0; i < fonts.length; i++) {
		convert(fonts[i]);
	}
}

main();