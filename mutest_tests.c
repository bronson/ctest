/* mutest_tests.c
 * Scott Bronson
 * 28 Feb 2007
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

#include "mu_assert.h"
#include "mutest.h"


void run_mutest_unit_tests()
{
	mutest_test_assert_flavor();
}

