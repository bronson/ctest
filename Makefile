# Makefile
# 28 Feb 2007
#
# Makefile to compile a standalone version of mutest.
#
# This file is released under the MIT License.
# See http://en.wikipedia.org/wiki/MIT_License for more.

COPTS=-g -Wall -Werror

all: mutest

mutest: mutest.c main.c test_assert.c mutest.h mutest_tests.c Makefile
	$(CC) $(COPTS) mutest.c main.c test_assert.c mutest_tests.c -o mutest

clean:
	rm -f mutest
