/**
 * dmp_test_internals.c
 *
 * Tests that include internal APIs of libdmp
 */

#include "dmp_test.h"
#include "../src/dmp_pool.h"

void test_ranges_0(void)
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
