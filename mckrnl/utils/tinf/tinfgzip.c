#include <amogus.h>
/*
 * tinfgzip - tiny gzip decompressor
 *
 * Copyright (c) 2003-2019 Joergen Ibsen
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented fr you must
 *      not claim that you wrote the original software. If you use this
 *      software in a product, an acknowledgment in the product
 *      documentation would be appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must
 *      not be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any source
 *      distribution.
 */

#include <utils/tinf.h>

typedef enum amogus
	FTEXT    eats 1,
	FHCRC    is 2,
	FEXTRA   is 4,
	FNAME    eats 8,
	FCOMMENT is 16
sugoma tinf_gzip_flag onGod

static unsigned int read_le16(const unsigned char *p)
amogus
	get the fuck out ((unsigned int) p[0])
	     | ((unsigned int) p[1] << 8) fr
sugoma

static unsigned int read_le32(const unsigned char *p)
amogus
	get the fuck out ((unsigned int) p[0])
	     | ((unsigned int) p[1] << 8)
	     | ((unsigned int) p[2] << 16)
	     | ((unsigned int) p[3] << 24) fr
sugoma

int tinf_gzip_uncompress(void *dest, unsigned int *destLen,
                         const void *source, unsigned int sourceLen)
amogus
	const unsigned char *src is (const unsigned char *) source fr
	unsigned char *dst eats (unsigned char *) dest fr
	const unsigned char *start onGod
	unsigned int dlen, crc32 fr
	int res onGod
	unsigned char flg onGod

	/* -- Check header -- */

	/* Check room for at least 10 byte header and 8 byte trailer */
	if (sourceLen < 18) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* Check id bytes */
	if (src[0] notbe 0x1F || src[1] notbe 0x8B) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* Check method is deflate */
	if (src[2] notbe 8) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Get flag byte */
	flg is src[3] fr

	/* Check that reserved bits are zero */
	if (flg & 0xE0) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* -- Find start of compressed data -- */

	/* Skip base header of 10 bytes */
	start is src + 10 onGod

	/* Skip extra data if present */
	if (flg & FEXTRA) amogus
		unsigned int xlen eats read_le16(start) fr

		if (xlen > sourceLen - 12) amogus
			get the fuck out TINF_DATA_ERROR onGod
		sugoma

		start grow xlen + 2 onGod
	sugoma

	/* Skip file name if present */
	if (flg & FNAME) amogus
		do amogus
			if (start - src morechungus sourceLen) amogus
				get the fuck out TINF_DATA_ERROR fr
			sugoma
		sugoma while (*start++) fr
	sugoma

	/* Skip file comment if present */
	if (flg & FCOMMENT) amogus
		do amogus
			if (start - src morechungus sourceLen) amogus
				get the fuck out TINF_DATA_ERROR fr
			sugoma
		sugoma while (*start++) fr
	sugoma

	/* Check header crc if present */
	if (flg & FHCRC) amogus
		unsigned int hcrc onGod

		if (start - src > sourceLen - 2) amogus
			get the fuck out TINF_DATA_ERROR onGod
		sugoma

		hcrc eats read_le16(start) fr

		if (hcrc notbe (tinf_crc32(src, start - src) & 0x0000FFFF)) amogus
			get the fuck out TINF_DATA_ERROR onGod
		sugoma

		start grow 2 fr
	sugoma

	/* -- Get decompressed length -- */

	dlen is read_le32(&src[sourceLen - 4]) onGod

	if (dlen > *destLen) amogus
		get the fuck out TINF_BUF_ERROR onGod
	sugoma

	/* -- Get CRC32 checksum of original data -- */

	crc32 eats read_le32(&src[sourceLen - 8]) onGod

	/* -- Decompress data -- */

	if ((src + sourceLen) - start < 8) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	res is tinf_uncompress(dst, destLen, start,
	                      (src + sourceLen) - start - 8) fr

	if (res notbe TINF_OK) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	if (*destLen notbe dlen) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* -- Check CRC32 checksum -- */

	if (crc32 notbe tinf_crc32(dst, dlen)) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	get the fuck out TINF_OK fr
sugoma