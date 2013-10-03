# Silly Makefile to build libdmp.a and test_dmp executable

PLATFORM=$(shell uname -s)

MINGW=0
ifneq (,$(findstring MINGW32,$(PLATFORM)))
	MINGW=1
endif
ifneq (,$(findstring mingw,$(CROSS_COMPILE)))
	MINGW=1
endif

rm=rm -f
AR=ar cq
RANLIB=ranlib

LIBNAME=libdmp.a

ifeq ($(MINGW),1)
	CC=gcc
else
	CC=cc
endif

INCLUDES=-Isrc -Iinclude

DEFINES= $(INCLUDES) -DSTDC -D_GNU_SOURCE $(EXTRA_DEFINES)
CFLAGS= -g $(DEFINES) -Wall -Wextra -Wno-missing-field-initializers -std=c99 -O2 $(EXTRA_CFLAGS)

SRCS = $(wildcard src/*.c)

ifeq ($(MINGW),1)
	DEFINES += -DWIN32 -D_WIN32_WINNT=0x0501 -D__USE_MINGW_ANSI_STDIO=1
else
	CFLAGS += -fPIC
endif

OBJS = $(patsubst %.c,%.o,$(SRCS))

%.c.o:
	$(CC) $(CFLAGS) -c $*.c

default: $(LIBNAME)

$(LIBNAME): $(OBJS)
	$(rm) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

TESTSRCS = $(wildcard test/*.c)

test: dmp_test

dmp_test: $(LIBNAME) include/dmp.h $(TESTSRCS)
	$(CC) -o dmp_test $(CFLAGS) $(TESTSRCS) -L. -ldmp

clean:
	$(rm) -rf $(OBJS) $(LIBNAME) dmp_test *.dSYM
