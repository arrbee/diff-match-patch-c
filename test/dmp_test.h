/**
 * dmp_test.h
 *
 * Some common test declarations
 */
#ifndef INCLUDE_dmp_test_h__
#define INCLUDE_dmp_test_h__

#include <stdio.h>
#include <assert.h>
#include <dmp.h>

typedef void (*test_fn)(void);

#define progress()	fputs(".", stderr)

extern void test_util_0(void);
extern void test_ranges_0(void);
extern void test_diff_0(void);

#endif
