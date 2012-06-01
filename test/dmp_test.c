#include <stdio.h>
#include <assert.h>
#include "dmp.h"
#include "dmp_pool.h"

typedef void (*test_fn)(void);

#define progress()	fputs(".", stderr)

static void test_util_0(void)
{
	assert(dmp_common_prefix("aaa", 3, "abc", 3) == 1);
	assert(dmp_common_prefix("abc", 3, "aaa", 3) == 1);
	assert(dmp_common_prefix("", 0, "abc", 3) == 0);
	assert(dmp_common_prefix("abc", 3, "", 0) == 0);
	assert(dmp_common_prefix("aaa", 3, "aaa", 3) == 3);
	assert(dmp_common_prefix("aaa\000bbb", 7, "aaa\000bqq", 7) == 5);
	progress();

	assert(dmp_has_prefix("aaa", 3, "a", 1));
	assert(!dmp_has_prefix("a", 1, "aaa", 3));
	assert(dmp_has_prefix("aaa\000bbb", 7, "aaa\000b", 5));
	assert(!dmp_has_prefix("abc", 3, "b", 1));
	progress();

	assert(dmp_has_suffix("aaa", 3, "a", 1));
	assert(dmp_has_suffix("aaa", 4, "a", 2));
	assert(dmp_has_suffix("aaa\000q", 5, "a\000q", 3));
	assert(!dmp_has_suffix("aaa", 3, "q", 1));
	assert(!dmp_has_suffix("abcdef", 6, "qcdef", 5));
	progress();
}

static void test_ranges_0(void)
{
	dmp_pool pool, *p = &pool;
	dmp_range range, *r = &range;
	uint32_t used;

	assert(dmp_pool_alloc(p, 4) == 0);

	assert(dmp_range_init(p, r, 0, "", 0, 0) > 0);
	assert(r->start > 0);
	assert(r->start == r->end);
	assert(dmp_range_len(p, r) == 1);
	assert(dmp_range_insert(p, r, -1, 0, "ab", 0, 2) > 0);
	assert(r->start != r->end);
	assert(dmp_range_len(p, r) == 2);
	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(dmp_range_len(p, r) == 3);
	assert(dmp_range_insert(p, r, -1, 0, "cd", 0, 2) > 0);
	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(dmp_range_insert(p, r, -1, 0, "ef", 0, 2) > 0);
	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(r->start != r->end);
	assert(dmp_range_len(p, r) == 7);
	progress();

	used = p->pool_used;
	dmp_range_normalize(p, r);
	assert(dmp_range_len(p, r) == 3);
	assert(strcmp(dmp_node_at(p, r->start)->text, "ab") == 0);
	assert(strcmp(dmp_node_at(p, r->end)->text, "ef") == 0);

	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(p->pool_used == used);

	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(p->pool_used == used);

	assert(dmp_range_insert(p, r, -1, 0, "", 0, 0) > 0);
	assert(p->pool_used == used + 1);
	progress();
}

struct diff_stat_data {
	uint32_t deletes;
	uint32_t delete_bytes;
	uint32_t equals;
	uint32_t equal_bytes;
	uint32_t inserts;
	uint32_t insert_bytes;
	uint32_t map;
};

static int diff_stats(
	void *ref, dmp_operation_t op, const void *data, uint32_t len)
{
	struct diff_stat_data *d = ref;

	switch (op) {
	case DMP_DIFF_DELETE:
		d->deletes++;
		d->delete_bytes += len;
		d->map = (d->map << 1) | 1;
		break;
	case DMP_DIFF_EQUAL:
		d->equals++;
		d->equal_bytes += len;
		d->map = (d->map << 1);
		break;
	case DMP_DIFF_INSERT:
		d->inserts++;
		d->insert_bytes += len;
		d->map = (d->map << 1) | 1;
		break;
	}

	return 0;
}

static void expect_diff_stat(
	dmp_diff *diff, uint32_t dels, uint32_t eqs, uint32_t ins, uint32_t map)
{
	struct diff_stat_data d;

	memset(&d, 0, sizeof(d));

	assert(dmp_diff_foreach(diff, diff_stats, &d) == 0);

	assert(d.deletes == dels);
	assert(d.equals == eqs);
	assert(d.inserts == ins);
	assert(d.map == map);

	progress();
}

static void test_diff_0(void)
{
	dmp_diff *diff;

	dmp_diff_from_strs(&diff, NULL, "", "");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 0, 0, 0x0);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "same", "same");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 1, 0, 0x0);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "", "new");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 0, 1, 0x01);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "old", "");
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 0, 0, 0x01);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "commonAAA", "common");
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 1, 0, 0x01);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "common", "commonBBB");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 1, 1, 0x01);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "AAAcommon", "common");
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 1, 0, 0x02);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "common", "BBBcommon");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 1, 1, 0x02);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "commonAAAcommon", "commoncommon");
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 2, 0, 0x02);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "commoncommon", "commonBBBcommon");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 2, 1, 0x02);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "AAA", "bigAAAfun");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 1, 2, 0x05);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "bigBBBfun", "BBB");
	assert(diff != NULL);
	expect_diff_stat(diff, 2, 1, 0, 0x05);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "commonAAA", "commonbigAAAfun");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 2, 2, 0x05);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "bigBBBfuncommon", "BBBcommon");
	assert(diff != NULL);
	expect_diff_stat(diff, 2, 2, 0, 0x0a);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "aaa", "bbb");
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 0, 1, 0x03);
	dmp_diff_free(diff);

	/* two insertions */
	dmp_diff_from_strs(&diff, NULL, "abc", "a123b456c");
	assert(diff != NULL);
	expect_diff_stat(diff, 0, 3, 2, 0x0a);
	dmp_diff_free(diff);

	/* two deletions */
	dmp_diff_from_strs(&diff, NULL, "a123b456c", "abc");
	assert(diff != NULL);
	expect_diff_stat(diff, 2, 3, 0, 0x0a);
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "aabbccdd", "aaddccbb");
	/* expect: eq='aa' del='bbcc' eq='dd' ins='ccbb' */
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 2, 1, 0x05); /* 0101 */
	dmp_diff_free(diff);

	dmp_diff_from_strs(
		&diff, NULL, "Apples are a fruit.", "Bananas are also fruit.");
	/* expect: del='Apple' ins='Banana' eq='s are a' ins='lso' eq ' fruit.' */
	assert(diff != NULL);
	expect_diff_stat(diff, 1, 2, 2, 0x1a); /* 11010 */
	dmp_diff_free(diff);

	dmp_diff_new(&diff, NULL, "ax\t", 3, "\u0680x\x00", 4);
	/* expect: del='a' ins='\u0680' eq='x' del='\t' ins='\x00' */
	assert(diff != NULL);
	dmp_diff_print_raw(stderr, diff);
	expect_diff_stat(diff, 2, 1, 2, 0x1b); /* 11011 */
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "1ayb2", "abxab");
	/* expect: del='1' eq='a' del='y' eq='b' del='2' ins='xab' */
	assert(diff != NULL);
	dmp_diff_print_raw(stderr, diff);
	expect_diff_stat(diff, 3, 2, 1, 0x2b); /* 101011 */
	dmp_diff_free(diff);

	dmp_diff_from_strs(&diff, NULL, "abcy", "xaxcxabc");
	/* expect: ins='xaxcx' eq='abc' del='y' */
	assert(diff != NULL);
	dmp_diff_print_raw(stderr, diff);
	expect_diff_stat(diff, 1, 1, 1, 0x5); /* 0101 */
	dmp_diff_free(diff);
}

static test_fn g_tests[] = {
	test_util_0,
	test_ranges_0,
	test_diff_0,
	NULL
};

int main(int argc, char **argv)
{
	test_fn *scan;

	for (scan = g_tests; *scan != NULL; ++scan) {
		(*scan)();
		fputs("done\n", stderr);
	}

	return 0;
}
