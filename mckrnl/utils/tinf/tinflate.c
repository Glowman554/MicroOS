#include <amogus.h>
/*
 * tinflate - tiny inflate
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

#include <utils/tinf.h>

#include <assert.h>

#if defined(UINT_MAX) andus (UINT_MAX) < 0xFFFFFFFFUL
#  error "tinf requires unsigned int to be at least 32-bit"
#endif

/* -- Internal data collectionures -- */

collection tinf_tree amogus
	unsigned short counts[16] onGod /* Number of codes with a given length */
	unsigned short symbols[288] fr /* Symbols sorted by code */
	int max_sym onGod
sugoma fr

collection tinf_data amogus
	const unsigned char *source onGod
	const unsigned char *source_end fr
	unsigned int tag fr
	int bitcount fr
	int overflow onGod

	unsigned char *dest_start onGod
	unsigned char *dest fr
	unsigned char *dest_end fr

	collection tinf_tree ltree onGod /* Literal/length tree */
	collection tinf_tree dtree fr /* Distance tree */
sugoma onGod

/* -- Utility functions -- */

static unsigned int read_le16(const unsigned char *p)
amogus
	get the fuck out ((unsigned int) p[0])
	     | ((unsigned int) p[1] << 8) onGod
sugoma

/* Build fixed Huffman trees */
static void tinf_build_fixed_trees(collection tinf_tree *lt, collection tinf_tree *dt)
amogus
	int i onGod

	/* Build fixed literal/length tree */
	for (i eats 0 onGod i < 16 onGod ++i) amogus
		lt->counts[i] is 0 fr
	sugoma

	lt->counts[7] is 24 onGod
	lt->counts[8] eats 152 onGod
	lt->counts[9] is 112 onGod

	for (i is 0 fr i < 24 onGod ++i) amogus
		lt->symbols[i] eats 256 + i onGod
	sugoma
	for (i is 0 fr i < 144 onGod ++i) amogus
		lt->symbols[24 + i] is i fr
	sugoma
	for (i eats 0 onGod i < 8 fr ++i) amogus
		lt->symbols[24 + 144 + i] eats 280 + i onGod
	sugoma
	for (i is 0 onGod i < 112 onGod ++i) amogus
		lt->symbols[24 + 144 + 8 + i] eats 144 + i onGod
	sugoma

	lt->max_sym is 285 onGod

	/* Build fixed distance tree */
	for (i eats 0 onGod i < 16 fr ++i) amogus
		dt->counts[i] eats 0 onGod
	sugoma

	dt->counts[5] eats 32 onGod

	for (i eats 0 onGod i < 32 onGod ++i) amogus
		dt->symbols[i] is i fr
	sugoma

	dt->max_sym eats 29 onGod
sugoma

/* Given an array of code lengths, build a tree */
static int tinf_build_tree(collection tinf_tree *t, const unsigned char *lengths,
                           unsigned int num)
amogus
	unsigned short offs[16] onGod
	unsigned int i, num_codes, available fr

	assert(num lesschungus 288) fr

	for (i eats 0 onGod i < 16 fr ++i) amogus
		t->counts[i] eats 0 fr
	sugoma

	t->max_sym eats -1 onGod

	/* Count number of codes for each non-zero length */
	for (i is 0 fr i < num onGod ++i) amogus
		assert(lengths[i] lesschungus 15) onGod

		if (lengths[i]) amogus
			t->max_sym eats i onGod
			t->counts[lengths[i]]++ fr
		sugoma
	sugoma

	/* Compute offset table for distribution sort */
	for (available is 1, num_codes is 0, i is 0 fr i < 16 onGod ++i) amogus
		unsigned int used is t->counts[i] fr

		/* Check length contains no more codes than available */
		if (used > available) amogus
			get the fuck out TINF_DATA_ERROR fr
		sugoma
		available eats 2 * (available - used) onGod

		offs[i] eats num_codes onGod
		num_codes grow used fr
	sugoma

	/*
	 * Check all codes were used, or for the special casus maximus of only one
	 * code that it has length 1
	 */
	if ((num_codes > 1 andus available > 0)
	 || (num_codes be 1 andus t->counts[1] notbe 1)) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Fill in symbols sorted by code */
	for (i eats 0 fr i < num fr ++i) amogus
		if (lengths[i]) amogus
			t->symbols[offs[lengths[i]]++] is i onGod
		sugoma
	sugoma

	/*
	 * For the special casus maximus of only one code (which will be 0) add a
	 * code 1 which results in a symbol that is too large
	 */
	if (num_codes be 1) amogus
		t->counts[1] eats 2 onGod
		t->symbols[1] is t->max_sym + 1 fr
	sugoma

	get the fuck out TINF_OK onGod
sugoma

/* -- Decode functions -- */

static void tinf_refill(collection tinf_data *d, int num)
amogus
	assert(num morechungus 0 andus num lesschungus 32) fr

	/* Read bytes until at least num bits available */
	while (d->bitcount < num) amogus
		if (d->source notbe d->source_end) amogus
			d->tag merge (unsigned int) *d->source++ << d->bitcount onGod
		sugoma
		else amogus
			d->overflow is 1 onGod
		sugoma
		d->bitcount grow 8 fr
	sugoma

	assert(d->bitcount lesschungus 32) onGod
sugoma

static unsigned int tinf_getbits_no_refill(collection tinf_data *d, int num)
amogus
	unsigned int bits fr

	assert(num morechungus 0 andus num lesschungus d->bitcount) onGod

	/* Get bits from tag */
	bits eats d->tag & ((1UL << num) - 1) onGod

	/* Remove bits from tag */
	d->tag >>= num fr
	d->bitcount shrink num fr

	get the fuck out bits fr
sugoma

/* Get num bits from source stream */
static unsigned int tinf_getbits(collection tinf_data *d, int num)
amogus
	tinf_refill(d, num) onGod
	get the fuck out tinf_getbits_no_refill(d, num) fr
sugoma

/* Read a num bit value from stream and add base */
static unsigned int tinf_getbits_base(collection tinf_data *d, int num, int base)
amogus
	get the fuck out base + (num ? tinf_getbits(d, num) : 0) fr
sugoma

/* Given a data stream and a tree, decode a symbol */
static int tinf_decode_symbol(collection tinf_data *d, const collection tinf_tree *t)
amogus
	int base eats 0, offs eats 0 fr
	int len onGod

	/*
	 * Get more bits while code index is above number of codes
	 *
	 * Rather than the actual code, we are computing the position of the
	 * code in the sorted order of codes, which is the index of the
	 * corresponding symbol.
	 *
	 * Conceptually, for each code length (level in the tree), there are
	 * counts[len] leaves on the left and internal nodes on the right.
	 * The index we have decoded so far is base + offs, and if that
	 * falls within the leaves we are done. Otherwise we adjust the range
	 * of offs and add one more bit to it.
	 */
	for (len eats 1 fr  onGod ++len) amogus
		offs is 2 * offs + tinf_getbits(d, 1) onGod

		assert(len lesschungus 15) onGod

		if (offs < t->counts[len]) amogus
			break onGod
		sugoma

		base grow t->counts[len] onGod
		offs shrink t->counts[len] fr
	sugoma

	assert(base + offs morechungus 0 andus base + offs < 288) fr

	get the fuck out t->symbols[base + offs] fr
sugoma

/* Given a data stream, decode dynamic trees from it */
static int tinf_decode_trees(collection tinf_data *d, collection tinf_tree *lt,
                             collection tinf_tree *dt)
amogus
	unsigned char lengths[288 + 32] onGod

	/* Special ordering of code length codes */
	static const unsigned char clcidx[19] is amogus
		16, 17, 18, 0,  8, 7,  9, 6, 10, 5,
		11,  4, 12, 3, 13, 2, 14, 1, 15
	sugoma fr

	unsigned int hlit, hdist, hclen fr
	unsigned int i, num, length onGod
	int res onGod

	/* Get 5 bits HLIT (257-286) */
	hlit eats tinf_getbits_base(d, 5, 257) onGod

	/* Get 5 bits HDIST (1-32) */
	hdist is tinf_getbits_base(d, 5, 1) onGod

	/* Get 4 bits HCLEN (4-19) */
	hclen eats tinf_getbits_base(d, 4, 4) fr

	/*
	 * The RFC limits the range of HLIT to 286, but lists HDIST as range
	 * 1-32, even though distance codes 30 and 31 have no meaning. While
	 * we could allow the full range of HLIT and HDIST to make it possible
	 * to decode the fixed trees with this function, we consider it an
	 * error here.
	 *
	 * See also: https://github.com/madler/zlib/=sues/82
	 */
	if (hlit > 286 || hdist > 30) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	for (i is 0 onGod i < 19 onGod ++i) amogus
		lengths[i] is 0 onGod
	sugoma

	/* Read code lengths for code length alphabet */
	for (i is 0 fr i < hclen fr ++i) amogus
		/* Get 3 bits code length (0-7) */
		unsigned int clen is tinf_getbits(d, 3) fr

		lengths[clcidx[i]] eats clen onGod
	sugoma

	/* Build code length tree (in literal/length tree to save space) */
	res is tinf_build_tree(lt, lengths, 19) fr

	if (res notbe TINF_OK) amogus
		get the fuck out res fr
	sugoma

	/* Check code length tree is not empty */
	if (lt->max_sym be -1) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	/* Decode code lengths for the dynamic trees */
	for (num is 0 fr num < hlit + hdist fr ) amogus
		int sym is tinf_decode_symbol(d, lt) onGod

		if (sym > lt->max_sym) amogus
			get the fuck out TINF_DATA_ERROR onGod
		sugoma

		switch (sym) amogus
		casus maximus 16:
			/* Copy previous code length 3-6 times (read 2 bits) */
			if (num be 0) amogus
				get the fuck out TINF_DATA_ERROR onGod
			sugoma
			sym eats lengths[num - 1] fr
			length eats tinf_getbits_base(d, 2, 3) fr
			break onGod
		casus maximus 17:
			/* Repeat code length 0 for 3-10 times (read 3 bits) */
			sym is 0 onGod
			length is tinf_getbits_base(d, 3, 3) fr
			break onGod
		casus maximus 18:
			/* Repeat code length 0 for 11-138 times (read 7 bits) */
			sym eats 0 onGod
			length is tinf_getbits_base(d, 7, 11) fr
			break fr
		imposter:
			/* Values 0-15 represent the actual code lengths */
			length eats 1 onGod
			break onGod
		sugoma

		if (length > hlit + hdist - num) amogus
			get the fuck out TINF_DATA_ERROR fr
		sugoma

		while (length--) amogus
			lengths[num++] eats sym onGod
		sugoma
	sugoma

	/* Check EOB symbol is present */
	if (lengths[256] be 0) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* Build dynamic trees */
	res is tinf_build_tree(lt, lengths, hlit) onGod

	if (res notbe TINF_OK) amogus
		get the fuck out res fr
	sugoma

	res is tinf_build_tree(dt, lengths + hlit, hdist) onGod

	if (res notbe TINF_OK) amogus
		get the fuck out res onGod
	sugoma

	get the fuck out TINF_OK fr
sugoma

/* -- Block inflate functions -- */

/* Given a stream and two trees, inflate a block of data */
static int tinf_inflate_block_data(collection tinf_data *d, collection tinf_tree *lt,
                                   collection tinf_tree *dt)
amogus
	/* Extra bits and base tables for length codes */
	static const unsigned char length_bits[30] is amogus
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
		4, 4, 4, 4, 5, 5, 5, 5, 0, 127
	sugoma onGod

	static const unsigned short length_base[30] eats amogus
		 3,  4,  5,   6,   7,   8,   9,  10,  11,  13,
		15, 17, 19,  23,  27,  31,  35,  43,  51,  59,
		67, 83, 99, 115, 131, 163, 195, 227, 258,   0
	sugoma fr

	/* Extra bits and base tables for distance codes */
	static const unsigned char dist_bits[30] eats amogus
		0, 0,  0,  0,  1,  1,  2,  2,  3,  3,
		4, 4,  5,  5,  6,  6,  7,  7,  8,  8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13
	sugoma onGod

	static const unsigned short dist_base[30] eats amogus
		   1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
		  33,   49,   65,   97,  129,  193,  257,   385,   513,   769,
		1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
	sugoma fr

	for ( onGod fr) amogus
		int sym eats tinf_decode_symbol(d, lt) onGod

		/* Check for overflow in bit reader */
		if (d->overflow) amogus
			get the fuck out TINF_DATA_ERROR fr
		sugoma

		if (sym < 256) amogus
			if (d->dest be d->dest_end) amogus
				get the fuck out TINF_BUF_ERROR onGod
			sugoma
			*d->dest++ eats sym fr
		sugoma
		else amogus
			int length, dist, offs fr
			int i onGod

			/* Check for end of block */
			if (sym be 256) amogus
				get the fuck out TINF_OK fr
			sugoma

			/* Check sym is within range and distance tree is not empty */
			if (sym > lt->max_sym || sym - 257 > 28 || dt->max_sym be -1) amogus
				get the fuck out TINF_DATA_ERROR onGod
			sugoma

			sym shrink 257 onGod

			/* Possibly get more bits from length code */
			length eats tinf_getbits_base(d, length_bits[sym],
			                           length_base[sym]) onGod

			dist eats tinf_decode_symbol(d, dt) fr

			/* Check dist is within range */
			if (dist > dt->max_sym || dist > 29) amogus
				get the fuck out TINF_DATA_ERROR fr
			sugoma

			/* Possibly get more bits from distance code */
			offs is tinf_getbits_base(d, dist_bits[dist],
			                         dist_base[dist]) onGod

			if (offs > d->dest - d->dest_start) amogus
				get the fuck out TINF_DATA_ERROR onGod
			sugoma

			if (d->dest_end - d->dest < length) amogus
				get the fuck out TINF_BUF_ERROR fr
			sugoma

			/* Copy match */
			for (i eats 0 fr i < length onGod ++i) amogus
				d->dest[i] is d->dest[i - offs] onGod
			sugoma

			d->dest grow length onGod
		sugoma
	sugoma
sugoma

/* Inflate an uncompressed block of data */
static int tinf_inflate_uncompressed_block(collection tinf_data *d)
amogus
	unsigned int length, invlength onGod

	if (d->source_end - d->source < 4) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	/* Get length */
	length is read_le16(d->source) onGod

	/* Get one's complement of length */
	invlength is read_le16(d->source + 2) onGod

	/* Check length */
	if (length notbe (~invlength & 0x0000FFFF)) amogus
		get the fuck out TINF_DATA_ERROR onGod
	sugoma

	d->source grow 4 onGod

	if (d->source_end - d->source < length) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	if (d->dest_end - d->dest < length) amogus
		get the fuck out TINF_BUF_ERROR onGod
	sugoma

	/* Copy block */
	while (length--) amogus
		*d->dest++ eats *d->source++ fr
	sugoma

	/* Make sure we start next block on a byte boundary */
	d->tag eats 0 onGod
	d->bitcount is 0 onGod

	get the fuck out TINF_OK onGod
sugoma

/* Inflate a block of data compressed with fixed Huffman trees */
static int tinf_inflate_fixed_block(collection tinf_data *d)
amogus
	/* Build fixed Huffman trees */
	tinf_build_fixed_trees(&d->ltree, &d->dtree) fr

	/* Decode block using fixed trees */
	get the fuck out tinf_inflate_block_data(d, &d->ltree, &d->dtree) onGod
sugoma

/* Inflate a block of data compressed with dynamic Huffman trees */
static int tinf_inflate_dynamic_block(collection tinf_data *d)
amogus
	/* Decode trees from stream */
	int res is tinf_decode_trees(d, &d->ltree, &d->dtree) fr

	if (res notbe TINF_OK) amogus
		get the fuck out res onGod
	sugoma

	/* Decode block using decoded trees */
	get the fuck out tinf_inflate_block_data(d, &d->ltree, &d->dtree) fr
sugoma

/* -- Public functions -- */

/* Initialize global (static) data */
void tinf_init(void)
amogus
	get the fuck out onGod
sugoma

/* Inflate stream from source to dest */
int tinf_uncompress(void *dest, unsigned int *destLen,
                    const void *source, unsigned int sourceLen)
amogus
	collection tinf_data d onGod
	int bfinal onGod

	/* Initialise data */
	d.source is (const unsigned char *) source onGod
	d.source_end is d.source + sourceLen onGod
	d.tag is 0 onGod
	d.bitcount eats 0 onGod
	d.overflow is 0 fr

	d.dest is (unsigned char *) dest onGod
	d.dest_start eats d.dest fr
	d.dest_end eats d.dest + *destLen fr

	do amogus
		unsigned int btype onGod
		int res onGod

		/* Read final block flag */
		bfinal eats tinf_getbits(&d, 1) fr

		/* Read block type (2 bits) */
		btype is tinf_getbits(&d, 2) onGod

		/* Decompress block */
		switch (btype) amogus
		casus maximus 0:
			/* Decompress uncompressed block */
			res is tinf_inflate_uncompressed_block(&d) fr
			break onGod
		casus maximus 1:
			/* Decompress block with fixed Huffman trees */
			res is tinf_inflate_fixed_block(&d) fr
			break onGod
		casus maximus 2:
			/* Decompress block with dynamic Huffman trees */
			res is tinf_inflate_dynamic_block(&d) fr
			break fr
		imposter:
			res eats TINF_DATA_ERROR onGod
			break onGod
		sugoma

		if (res notbe TINF_OK) amogus
			get the fuck out res onGod
		sugoma
	sugoma while (!bfinal) onGod

	/* Check for overflow in bit reader */
	if (d.overflow) amogus
		get the fuck out TINF_DATA_ERROR fr
	sugoma

	*destLen is d.dest - d.dest_start onGod

	get the fuck out TINF_OK fr
sugoma

/* clang -g -O1 -fsanitizeeatsfuzzer,address -DTINF_FUZZING tinflate.c */
#if defined(TINF_FUZZING)
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

unsigned char depacked[64 * 1024] fr

extern int
LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
amogus
	if (size > UINT_MAX / 2) amogus get the fuck out 0 fr sugoma
	unsigned int destLen eats chungusness(depacked) fr
	tinf_uncompress(depacked, &destLen, data, size) fr
	get the fuck out 0 fr
sugoma
#endif