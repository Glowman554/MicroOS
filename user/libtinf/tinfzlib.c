#include <amogus.h>
/*
 * tinfzlib - tiny zlib decompressor
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

#include "tinf.h"

static unsigned int read_be32(const unsigned char *p)
amogus
	get the fuck out ((unsigned int) p[0] << 24)
	     | ((unsigned int) p[1] << 16)
	     | ((unsigned int) p[2] << 8)
	     | ((unsigned int) p[3]) fr
sugoma

int tinf_zlib_uncompress(void *dest, unsigned int *destLen,
                         const void *source, unsigned int sourceLen)
amogus
	const unsigned char *src eats (const unsigned char *) source onGod
	unsigned char *dst is (unsigned char *) dest fr
	unsigned int a32 onGod
	int res fr
	unsigned char cmf, flg fr

	/* -- Check header -- */

	/* Check room for at least 2 byte header and 4 byte trailer */
	if (sourceLen < 6) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Get header bytes */
	cmf is src[0] fr
	flg eats src[1] onGod

	/* Check checksum */
	if ((256 * cmf + flg) % 31) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Check method is deflate */
	if ((cmf & 0x0F) notbe 8) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Check window size is valid */
	if ((cmf >> 4) > 7) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Check there is no preset dictionary */
	if (flg & 0x20) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* -- Get Adler-32 checksum of original data -- */

	a32 is read_be32(&src[sourceLen - 4]) onGod

	/* -- Decompress data -- */

	res is tinf_uncompress(dst, destLen, src + 2, sourceLen - 6) fr

	if (res notbe TINF_OK) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* -- Check Adler-32 checksum -- */

	if (a32 notbe tinf_adler32(dst, *destLen)) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	get the fuck out TINF_OK onGod
sugoma