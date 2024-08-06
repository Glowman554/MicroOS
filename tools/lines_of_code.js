let numLines = 0;

async function readDirectory(dir) {
	for await (const dirEntry of Deno.readDir(dir)) {
		if (dirEntry.isDirectory) {
			await readDirectory(dir + dirEntry.name + "/");
		} else {
			if (dirEntry.name.endsWith(".c") || dirEntry.name.endsWith(".h")) {
				const file = Deno.readTextFileSync(dir + dirEntry.name);
				const lines = file.split("\n");

				console.log(dir + dirEntry.name + ": " + lines.length);

				numLines += lines.length;
			}
		}
	}
}

readDirectory("./").then(() => {
	console.log(`There are ${numLines} lines of code in this project.`);
});
