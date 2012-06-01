/**
 * dmp.h
 *
 * C language implementation of Google Diff, Match, and Patch library
 *
 * Original library is Copyright (c) 2006 Google Inc.
 * http://code.google.com/p/google-diff-match-patch/
 *
 * Copyright (c) 2012 Russell Belfer <rb@github.com>
 * https://github.com/arrbee/google-diff-match-patch-c/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef INCLUDE_H_dmp
#define INCLUDE_H_dmp

#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
	DMP_DIFF_DELETE = -1,
	DMP_DIFF_EQUAL = 0,
	DMP_DIFF_INSERT = 1
} dmp_operation_t;

typedef struct {
    /* Number of seconds to map a diff before giving up (0 for infinity). */
    float timeout; /* = 1.0 */

	/* Cost of an empty edit operation in terms of edit characters. */
    int edit_cost; /* = 4 */

	/* At what point is no match declared (0.0 = perfection, 1.0 = very
	 * loose).
	 */
    float match_threshold; /* = 0.5 */

	/* How far to search for a match (0 = exact location, 1000+ = broad match).
	 * A match this many characters away from the expected location will add
	 * 1.0 to the score (0.0 is a perfect match).
	 */
    float match_distance; /* = 1000 */

	/* When deleting a large block of text (over ~64 characters), how close
	 * do the contents have to be to match the expected contents. (0.0 =
	 * perfection, 1.0 = very loose).  Note that match_threshold controls
	 * how closely the end points of a delete need to match.
	 */
    float patch_delete_threshold; /* = 0.5 */

	/* Chunk size for context length. */
    int patch_margin; /* = 4 */

	/* The number of bits in an int.
	 * Python has no maximum, thus to disable patch splitting set to 0.
	 * However to avoid long patches in certain pathological cases, use 32.
	 * Multiple short patches (using native ints) are much faster than long
	 * ones.
	 */
    int match_maxbits; /* = 32 */

	/* Should diff run an initial line-level diff to identify changed areas?
	 * Running initial diff will be slightly faster but slightly less optimal.
	 */
	int check_lines; /* = 1 */

	/* Should the diff trim the common prefix? */
	int trim_common_prefix; /* = 1 */

	/* Should the diff trim the common suffix? */
	int trim_common_suffix; /* = 1 */
} dmp_options;

typedef struct dmp_diff dmp_diff;

typedef struct dmp_patch dmp_patch;

typedef int (*dmp_diff_callback)(
	void *cb_ref, dmp_operation_t op, const void *data, uint32_t len);

/**
 * Initialize options structure to default values
 */
extern int dmp_options_init(dmp_options *opts);

/**
 * Calculate the diff between two texts
 */
extern int dmp_diff_new(
	dmp_diff **diff,
	const dmp_options *options,
	const char *text1,
	uint32_t    len1,
	const char *text2,
	uint32_t    len2);

extern int dmp_diff_from_strs(
	dmp_diff **diff,
	const dmp_options *options,
	const char *text1,
	const char *text2);

/**
 * Free the diff structure
 */
extern void dmp_diff_free(dmp_diff *diff);

/**
 * Iterate over changes in a diff list
 */
extern int dmp_diff_foreach(
	const dmp_diff *diff,
	dmp_diff_callback cb,
	void *cb_ref);

extern uint32_t dmp_diff_hunks(const dmp_diff *diff);

extern void dmp_diff_print_raw(FILE *fp, const dmp_diff *diff);

extern int dmp_patch_new(
	dmp_patch     **patch,
	const char      *text1,
	uint32_t         len1,
	const dmp_diff *diff);

extern void dmp_patch_free(dmp_patch *patch);

/*
 * Utility functions
 */

extern uint32_t dmp_common_prefix(
	const char *t1, uint32_t l1, const char *t2, uint32_t l2);

extern uint32_t dmp_common_suffix(
	const char *t1, uint32_t l1, const char *t2, uint32_t l2);

extern int dmp_has_prefix(
	const char *text, uint32_t tlen, const char *pfx, uint32_t plen);

extern int dmp_has_suffix(
	const char *text, uint32_t tlen, const char *sfx, uint32_t slen);

extern int dmp_strcmp(
	const char *t1, uint32_t l1, const char *t2, uint32_t l2);

extern const char *dmp_strstr(
	const char *haystack, uint32_t lh, const char *needle, uint32_t ln);

extern void dmp_build_texts_from_diff(
	char **t1, uint32_t *l1, char **t2, uint32_t *l2, const dmp_diff *diff);

#endif
