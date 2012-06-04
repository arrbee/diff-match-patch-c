Port of google-diff-match-patch to C
====================================

This is a C language port of Neil Fraser's google-diff-match-patch code.
His original code is available at:

  http://code.google.com/p/google-diff-match-patch/

That original code is Copyright (c) 2006 Google Inc. and licensed
under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).
Please see the APACHE-LICENSE-2.0 file included with this code
for details.

This code is available at:

  https://github.com/arrbee/diff-match-patch-c/

It is Copyright (c) 2012 Russell Belfer <rb@github.com> and licensed
under the MIT License.  See the included LICENSE file.

Example Usage
-------------

All functions and structures used in this library are prefixed with
`dmp_`.  To generate a diff, you use a function to create a `dmp_diff`
object which you can then access and manipulate via other functions.

Here is a silly little example that counts the total length of the
"equal" runs from the diff.
```c
	{
		dmp_diff *diff;
		int eq = 0;

		if (dmp_diff_from_strs(&diff, NULL, "string 1", "string 2") != 0)
			handle_error();

		dmp_diff_foreach(diff, how_equal, &eq);
		printf("Strings had %d equal bytes\n", eq);

		dmp_diff_free(diff);
	}

	int how_equal(
		void *ref, dmp_operation_t op, const void *data, uint32_t len)
	{
		int *sum = ref;
		if (op == DMP_DIFF_EQUAL)
			(*sum) += len;
		return 0;
	}
```

This shows the basic pattern of diff API usage:
1. Generate a diff
2. Process the diff in some way
3. Free the diff

Diff API
--------

All public functions in the library that could fail return an `int`
and will return 0 for success or -1 for failure.  Functions which
cannot fail will either have a void return or will return a specific
other data type if they are simple data lookups.

Here are the main functions for generating and accessing diffs:

```c
/**
 * Public: Calculate the diff between two texts.
 *
 * This will allocate and populate a new `dmp_diff` object with records
 * describing how to transform `text1` into `text2`.  This returns a diff
 * with byte-level differences between the two texts.  You can use one of
 * the diff transformation functions below to modify the diffs to word or
 * line level diffs, or to align diffs to UTF-8 boundaries or the like.
 *
 * diff - Pointer to a `dmp_diff` pointer that will be allocated.  You must
 *        call `dmp_diff_free()` on this pointer when done.
 * options - `dmp_options` structure to control diff, or NULL to use defaults.
 * text1 - The FROM text for the left side of the diff.
 * len1 - The number of bytes of data in `text1`.
 * text2 - The TO text for the right side of the diff.
 * len2 - The number of bytes of data in `text2`.
 *
 * Returns 0 if the diff was successfully generated, -1 on failure.  The
 * only current failure scenario would be a failed allocation.  Otherwise,
 * some sort of diff should be generated..
 */
extern int dmp_diff_new(
	dmp_diff **diff,
	const dmp_options *options,
	const char *text1,
	uint32_t    len1,
	const char *text2,
	uint32_t    len2);

/**
 * Public: Generate diff from NUL-terminated strings.
 *
 * This is a convenience function when you know that you are diffing
 * NUL-terminated strings.  It simply calls `strlen()` and passes the
 * results along to `dmp_diff_new` (plus it deals correctly with NULL
 * strings, passing them in a zero-length texts).
 *
 * diff - Pointer to a `dmp_diff` pointer that will be allocated.  You must
 *        call `dmp_diff_free()` on this pointer when done.
 * options - `dmp_options` structure to control diff, or NULL to use defaults.
 * text1 - The FROM string for the left side of the diff.  Must be a regular
 *         NUL-terminated C string.
 * text2 - The TO string for the right side of the diff.  Must be a regular
 *         NUL-terminated C string.
 *
 * Returns 0 if the diff was successfully generated, -1 on failure.  The
 * only current failure scenario would be a failed allocation.  Otherwise,
 * some sort of diff should be generated..
 */
extern int dmp_diff_from_strs(
	dmp_diff **diff,
	const dmp_options *options,
	const char *text1,
	const char *text2);

/**
 * Public: Free the diff structure.
 *
 * Call this when you are done with the diff data.
 *
 * diff - The `dmp_diff` object to be freed.
 */
extern void dmp_diff_free(dmp_diff *diff);

/**
 * Public: Iterate over changes in a diff list.
 *
 * Invoke a callback on each hunk of a diff.
 *
 * diff - The `dmp_diff` object to iterate over.
 * cb - The callback function to invoke on each hunk.
 * cb_ref - A reference pointer that will be passed to callback.
 *
 * Returns 0 if iteration completed successfully, or any non-zero value
 * that was returned by the `cb` callback function to terminate iteration.
 */
extern int dmp_diff_foreach(
	const dmp_diff *diff,
	dmp_diff_callback cb,
	void *cb_ref);
```

Status
------

At this point, the basic diff code works, although I haven't implemented all
of the optimizations yet.  I haven't written any of the diff formatting
helpers from the original library yet, nor have I started on the match or
patch related code yet.

Copyright and License
---------------------

The original Google Diff, Match and Patch Library is licensed under
the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).
The full terms of that license are included here in the
`APACHE-LICENSE-2.0` file.

Diff, Match and Patch Library

  Written by Neil Fraser
  Copyright (c) 2006 Google Inc.
  <http://code.google.com/p/google-diff-match-patch/>

This C version of Diff, Match and Patch Library is licensed under
the [MIT License](http://www.opensource.org/licenses/MIT) (a.k.a.
the Expat License) which is included here in the `LICENSE` file.

C version of Diff, Match and Patch Library

  Copyright (c) 2012 Russell Belfer <rb@github.com>
  <http://github.com/arrbee/google-diff-match-patch-c/>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


