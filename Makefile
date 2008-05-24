# Makefile
# 28 Feb 2007
#
# Makefile to compile a standalone version of ctest.
#
# Copyright (C) 2007 Scott Bronson
# This file is released under the MIT License.
# See http://www.opensource.org/licenses/mit-license.php

COPTS=-g -Wall -Werror
COPTS+=-ansi -pedantic

CSRC=main.c ctest.c ct_assert.c
CHDR=ctest.h ct_assert.h

all: ctest

ctest: $(CSRC) $(CHDR) Makefile
	$(CC) $(COPTS) $(CSRC) -o ctest

# This uses the tmtest command to perform some functional testing.
# You can ignore it if you don't have tmtest installed.
test: ctest
	./ctest
	tmtest

clean:
	rm -f ctest
