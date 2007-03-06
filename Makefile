# Makefile
# 28 Feb 2007
#
# Makefile to compile a standalone version of mutest.
#
# Copyright (C) 2007 Scott Bronson
# This file is released under the MIT License.
# See http://en.wikipedia.org/wiki/MIT_License for more.

COPTS=-g -Wall -Werror

CSRC=main.c mutest_tests.c $(shell ./mutest-config -c -t)
CHDR=$(shell ./mutest-config -h)

all: mutest

mutest: $(CSRC) $(CHDR) Makefile
	$(CC) $(COPTS) $(CSRC) -o mutest

clean:
	rm -f mutest
