= Port of google-diff-match-patch to C

This is a C language port of Neil Fraser's google-diff-match-patch code.
His original code is available at:

  http://code.google.com/p/google-diff-match-patch/

That original code is Copyright (c) 2006 Google Inc. and licensed
under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).
Please see the APACHE-LICENSE-2.0 file included with this code
for details.

This code is available at:

  https://github.com/arrbee/google-diff-match-patch-c/

It is Copyright (c) 2012 Russell Belfer <rb@github.com> and licensed
under the MIT License.  See the included LICENSE file.

== Example Usage

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

== Diff API

```c
	/* Compute a new diff.
	 *
	 * Call this function to calculate a new list of diffs between
	 * two texts.  The texts are NOT NUL-terminated and must be
	 * passed in a pointers to data and lengths.  The diff is stored
	 * as pointers into the original text data, so the texts must
	 * not be freed until you are through with the diff object.
	 *
	 * This will create and pass back a new `dmp_diff` object that
	 * can be processed by other functions in the library.
	 *
	 * Right now you should pass NULL for the `options` parameter
	 * since it is currently ignored.
	 */
	int dmp_diff_new(
		dmp_diff **diff,
		const dmp_options *options,
		const char *text1,
		uint32_t    len1,
		const char *text2,
		uint32_t    len2);

	/* Convenience function to diff NUL-terminated strings */
	int dmp_diff_from_strs(
		dmp_diff **diff,
		const dmp_options *options,
		const char *text1,
		const char *text2);

	/* Free the memory for a diff */
	void dmp_diff_free(dmp_diff *diff);

	/* Get a callback for each span of a diff */
	int dmp_diff_foreach(
		const dmp_diff *diff,
		dmp_diff_callback cb,
		void *cb_ref);
```

== Status

At this point, the basic diff code works, although I haven't
implemented all of the optimizations yet.  I haven't written any
of the diff formatting helpers, nor have I started on the match
or patch related code yet.

== Copyright and License

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


