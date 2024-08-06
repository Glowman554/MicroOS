function strToUint8(str) {
	const arr = new Uint8Array(str.length + 1);
	for (let i = 0; i < str.length; i++) {
		arr[i] = str.charCodeAt(i);
	}
	arr[str.length] = 0;
	return arr;
}

function mergeUint8Array(arrayOne, arrayTwo) {
	const mergedArray = new Uint8Array(arrayOne.length + arrayTwo.length);
	mergedArray.set(arrayOne);
	mergedArray.set(arrayTwo, arrayOne.length);
	return mergedArray;
}

function figlNew() {
	return {
		magic: new Uint32Array([0xf181babe]),
		offsets: new Uint32Array(0xff),

		data: new Uint8Array(0x10000),
		dataPtr: 0,
	};
}

function figlRegister(char, charStr, figl) {
	figl.data.set(strToUint8(charStr), figl.dataPtr);
	figl.offsets[char] = figl.dataPtr;
	figl.dataPtr += charStr.length + 1;
}

function figlSave(filename, figl) {
	const newArr = mergeUint8Array(
		mergeUint8Array(
			new Uint8Array(figl.magic.buffer),
			new Uint8Array(figl.offsets.buffer),
		),
		figl.data.slice(0, figl.dataPtr),
	);
	console.log("Saving " + filename);
	Deno.writeFileSync(filename, newArr);
}

function convert(fontName) {
	fetch(
		"https://github.com/scottgonzalez/figlet-js/raw/master/fonts/" +
			fontName + ".flf",
	).then((res) =>
		res.text().then((contents) => {
			const lines = contents.split("\n");
			const header = lines[0].split(" ");
			const hardblank = header[0].charAt(header[0].length - 1);
			const height = +header[1];
			const comments = +header[5];

			const font = {
				defn: lines.slice(comments + 1),
				hardblank: hardblank,
				height: height,
			};

			const chars = [];

			for (let c = 32; c < 127; c++) {
				const height = font.height;
				const start = (c - 32) * height;
				const charDefn = [];
				for (let i = 0; i < height; i++) {
					charDefn[i] = font.defn[start + i].replace(/@/g, "")
						.replace(
							RegExp("\\" + font.hardblank, "g"),
							" ",
						);
				}

				chars[c] = charDefn.join("\n");
			}

			const figl = figlNew();

			for (let c = 0; c < chars.length; c++) {
				if (chars[c]) {
					figlRegister(c, chars[c] + "\n" || "", figl);
				}
			}

			figlSave("initrd/fonts/" + fontName + ".figl", figl);
		})
	);
}

function main() {
	const fonts = ["speed"];
	for (let i = 0; i < fonts.length; i++) {
		convert(fonts[i]);
	}
}

main();
