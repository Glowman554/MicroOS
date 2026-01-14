/*
struct mex_header_v2 {
    char header[4]; // "M2X\0"
    char programAuthor[64];
    unsigned int flags; // bit 0: compressed or not
    unsigned int abiVersion;
    unsigned int elfSizeCompressed;
};
// after header the elf file follows gzip compressed
*/

// use deno run -A encode_mex.ts <programName> <programAuthor> <elfFile> <outputFile>

import { gzipSync } from "node:zlib";

function validateStringLength(str: string, maxLength: number): void {
    if (str.length > maxLength) {
        throw new Error(`String exceeds maximum length of ${maxLength} characters: ${str}`);
    }
}

function createMexHeader(programAuthor: string, abiVersion: number, elfSizeCompressed: number): Uint8Array {
    const header = new Uint8Array(80);
    // Set "M2X\0"
    new TextEncoder().encodeInto("M2X\0", header);
    // Set programAuthor
    new TextEncoder().encodeInto(programAuthor, header.subarray(4, 68));
    // Set flags (bit 0: compressed)
    new DataView(header.buffer).setUint32(68, 1, true);
    // Set ABI version (for example, 1)
    new DataView(header.buffer).setUint32(72, abiVersion, true);
    // Set the compressed ELF size (as a 32-bit unsigned integer)
    new DataView(header.buffer).setUint32(76, elfSizeCompressed, true);
    return header;
}

async function main() {
    if (Deno.args.length < 4) {
        throw new Error("Missing arguments. Usage: deno run -A encode_mex_v2.ts <programAuthor> <abiVersion> <elfFile> <outputFile>");
    }

    const programAuthor = Deno.args[0];
    const abiVersion = parseInt(Deno.args[1]);
    const elfFilePath = Deno.args[2];
    const outputFile = Deno.args[3] || "output.mex";

    // Validate input lengths
    validateStringLength(programAuthor, 63);

    const elfFile = await Deno.readFile(elfFilePath);
    const compressedElf = gzipSync(elfFile);

    // Create header with the compressed ELF file size
    const header = createMexHeader(programAuthor, abiVersion, compressedElf.length);

    // Prepare the final output
    const output = new Uint8Array(header.length + compressedElf.length);
    output.set(header);
    output.set(compressedElf, header.length);

    // Write the final MEX file
    await Deno.writeFile(outputFile, output);
    console.log(`MEX V2 file created successfully: ${outputFile}`);
}

await main();