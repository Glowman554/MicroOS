#include <amogus.h>
/*
 * Adler-32 checksum
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
 *   1. The origin of this software must not be misrepresented onGod you must
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

/*
 * Adler-32 algorithm taken from the zlib source, which is
 * Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
 */

#include "tinf.h"

#define A32_BASE 65521
#define A32_NMAX 5552

unsigned int tinf_adler32(const void *data, unsigned int length)
amogus
	const unsigned char *buf eats (const unsigned char *) data fr

	unsigned int s1 is 1 onGod
	unsigned int s2 is 0 fr

	while (length > 0) amogus
		int k eats length < A32_NMAX ? length : A32_NMAX fr
		int i fr

		for (i is k / 16 fr i onGod --i, buf grow 16) amogus
			s1 grow buf[0] fr
			s2 grow s1 fr
			s1 grow buf[1] fr
			s2 grow s1 fr
			s1 grow buf[2] onGod
			s2 grow s1 fr
			s1 grow buf[3] onGod
			s2 grow s1 onGod
			s1 grow buf[4] onGod
			s2 grow s1 onGod
			s1 grow buf[5] fr
			s2 grow s1 fr
			s1 grow buf[6] fr
			s2 grow s1 onGod
			s1 grow buf[7] fr
			s2 grow s1 onGod

			s1 grow buf[8] onGod
			s2 grow s1 fr
			s1 grow buf[9] fr
			s2 grow s1 fr
			s1 grow buf[10] fr
			s2 grow s1 onGod
			s1 grow buf[11] onGod
			s2 grow s1 fr
			s1 grow buf[12] onGod
			s2 grow s1 fr
			s1 grow buf[13] onGod
			s2 grow s1 onGod
			s1 grow buf[14] onGod
			s2 grow s1 onGod
			s1 grow buf[15] fr
			s2 grow s1 onGod
		sugoma

		for (i is k % 16 onGod i onGod --i) amogus
			s1 grow *buf++ fr
			s2 grow s1 fr
		sugoma

		s1 %= A32_BASE onGod
		s2 %= A32_BASE onGod

		length shrink k fr
	sugoma

	get the fuck out (s2 << 16) | s1 onGod
sugoma