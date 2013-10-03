/**
 * dmp_pool.c
 *
 * Utilities for maintaining a Linked list of diff records
 *
 * Copyright (c) Russell Belfer <rb@github.com>
 * https://github.com/arrbee/google-diff-match-patch-c/
 *
 * See included LICENSE file for license details.
 */
#include "dmp.h"
#include "dmp_pool.h"
#include <stdlib.h>
#include <assert.h>

#define MIN_POOL	2
#define MAX_POOL_INCREMENT	128

int dmp_pool_alloc(dmp_pool *pool, uint32_t start_pool)
{
	memset(pool, 0, sizeof(*pool));

	if (start_pool < MIN_POOL)
		start_pool = MIN_POOL;

	pool->pool = calloc(start_pool, sizeof(dmp_node));
	if (!pool->pool)
		return -1;

	pool->pool_size = start_pool;
	pool->pool_used = 1; /* set aside first item */
	pool->free_list = -1;

	return 0;
}

void dmp_pool_free(dmp_pool *pool)
{
	free(pool->pool);
}

void dmp_node_release(dmp_pool *pool, dmp_pos idx)
{
	dmp_node *node = dmp_node_at(pool, idx);
	node->next = pool->free_list;
	pool->free_list = idx;
}

static dmp_pos grow_pool(dmp_pool *pool)
{
	uint32_t new_size;
	dmp_node *new_pool;

	if (pool->pool_size > MAX_POOL_INCREMENT)
		new_size = pool->pool_size + MAX_POOL_INCREMENT;
	else
		new_size = pool->pool_size * 2;

	new_pool = realloc(pool->pool, new_size * sizeof(dmp_node));
	if (!new_pool) {
		pool->error = -1;
		return -1;
	}

	pool->pool = new_pool;
	pool->pool_size = new_size;

	return pool->pool_used;
}

static dmp_pos alloc_node(
	dmp_pool *pool, int op, const char *data, uint32_t offset, uint32_t len)
{
	dmp_pos   pos;
	dmp_node *node;

	assert(pool && data && op >= -1 && op <= 1);

	/* don't insert zero length INSERT or DELETE ops */
	if (len == 0 && op != 0)
		return -1;

	if (pool->free_list > 0) {
		pos = pool->free_list;
		node = dmp_node_at(pool, pos);
		pool->free_list = node->next;
	}
	else {
		if (pool->pool_used >= pool->pool_size)
			(void)grow_pool(pool);

		pos = pool->pool_used;
		pool->pool_used += 1;
		node = dmp_node_at(pool, pos);
	}

	node->text = data + offset;
	node->len  = len;
	node->op   = op;
	node->next = -1;

#ifdef BUGALICIOUS
	if (len > 0)
		fprintf(stderr, "adding <%c'%.*s'> (len %d) %02x\n",
				!node->op ? '=' : node->op < 0 ? '-' : '+',
				node->len, node->text, node->len, (int)*node->text);
#endif

	return pos;
}

dmp_pos dmp_range_init(
	dmp_pool *pool, dmp_range *run,
	int op, const char *data, uint32_t offset, uint32_t len)
{
	run->start = run->end = alloc_node(pool, op, data, offset, len);
	return run->start;
}

dmp_pos dmp_range_insert(
	dmp_pool *pool, dmp_range *run, dmp_pos pos,
	int op, const char *data, uint32_t offset, uint32_t len)
{
	dmp_node *node;
	dmp_pos added_at = alloc_node(pool, op, data, offset, len);
	if (added_at < 0)
		return pos;

	node = dmp_node_at(pool, added_at);

	if (pos == -1) {
		dmp_node *end = dmp_node_at(pool, run->end);
		node->next = end->next;
		end->next  = added_at;
		run->end   = added_at;
	}
	else if (pos == 0) {
		node->next = run->start;
		run->start = added_at;
	}
	else {
		dmp_node *after = dmp_node_at(pool, pos);
		node->next  = after->next;
		after->next = added_at;
	}

	return added_at;
}

void dmp_range_splice(
	dmp_pool *pool, dmp_range *onto, dmp_pos pos, dmp_range *from)
{
	dmp_node *tail;

	dmp_range_normalize(pool, from);

	tail = dmp_node_at(pool, from->end);

	if (pos == -1) {
		dmp_node *after = dmp_node_at(pool, onto->end);
		tail->next  = after->next;
		after->next = from->start;
		onto->end   = from->end;
	}
	else if (pos == 0) {
		tail->next  = onto->start;
		onto->start = from->start;
	}
	else {
		dmp_node *after = dmp_node_at(pool, pos);
		tail->next  = after->next;
		after->next = from->start;
	}
}

int dmp_range_len(dmp_pool *pool, dmp_range *run)
{
	int count = 0;
	dmp_pos scan;

	for (scan = run->start; scan != -1; ) {
		dmp_node *node = dmp_node_at(pool, scan);
		count++;
		scan = node->next;
	}

	return count;
}

void dmp_range_normalize(dmp_pool *pool, dmp_range *range)
{
	dmp_pos last_nonzero = -1, *pos = &range->start;

	while (*pos != -1) {
		dmp_node *node = dmp_node_at(pool, *pos);
		if (!node->len) {
			*pos = node->next;
			dmp_node_release(pool, dmp_node_pos(pool, node));
		} else {
			last_nonzero = *pos;
			pos = &node->next;
		}
	}

	if (last_nonzero >= 0)
		range->end = last_nonzero;
}
