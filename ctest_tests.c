/* test_tests.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * Specifies all the tests that should be run by
 * the standalone ctest executable.
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

/** @file mutest_tests.c
 *
 * Specifies all the tests to be run when creating the unitest executable.
 *
 * @see run_mutest_unit_tests()
 */

#include "ct_assert.h"
#include "ctest.h"


void run_ctest_unit_tests()
{
	ctest_test_assert_flavor();
}

