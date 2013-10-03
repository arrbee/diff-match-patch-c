/**
 * dmp_pool.h
 *
 * Utilities for maintaining a Linked list of diff records
 *
 * Copyright (c) Russell Belfer <rb@github.com>
 * https://github.com/arrbee/google-diff-match-patch-c/
 *
 * See included LICENSE file for license details.
 */
#ifndef INCLUDE_H_dmp_pool
#define INCLUDE_H_dmp_pool

#include <stdint.h>

typedef int dmp_pos;

typedef struct {
	const char *text;
	uint32_t len;
	int op;
	dmp_pos next;
} dmp_node;

typedef struct {
	dmp_pos start, end;
} dmp_range;

typedef struct {
	dmp_node *pool;
	uint32_t pool_size, pool_used;
	dmp_pos free_list;
	int error;
} dmp_pool;

extern int dmp_pool_alloc(dmp_pool *pool, uint32_t start_pool);

extern void dmp_pool_free(dmp_pool *list);

extern dmp_pos dmp_range_init(
	dmp_pool *list, dmp_range *run,
	int op, const char *data, uint32_t offset, uint32_t len);

extern dmp_pos dmp_range_insert(
	dmp_pool *list, dmp_range *run, dmp_pos pos,
	int op, const char *data, uint32_t offset, uint32_t len);

extern void dmp_range_splice(
	dmp_pool *list, dmp_range *onto, dmp_pos pos, dmp_range *from);

extern int dmp_range_len(dmp_pool *pool, dmp_range *run);

/* remove all 0-length nodes and advance 'end' to actual end */
extern void dmp_range_normalize(dmp_pool *pool, dmp_range *range);

extern void dmp_node_release(dmp_pool *pool, dmp_pos idx);

#define dmp_node_at(POOL,POS)   (&((POOL)->pool[(POS)]))

#define dmp_node_pos(POOL,NODE) ((dmp_pos)((NODE) - (POOL)->pool))

#define dmp_range_foreach(POOL, RANGE, IDX, PTR) \
	for (IDX = (RANGE)->start; IDX >= 0; IDX = (PTR)->next)	\
		if (((PTR) = dmp_node_at((POOL),IDX))->len > 0)

#endif
