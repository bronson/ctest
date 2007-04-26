/* mutest_tests.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * Specifies all the tests that should be run by
 * the standalone mutest executable.
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

#include "mu_assert.h"
#include "mutest.h"


void run_mutest_unit_tests()
{
	mutest_test_assert_flavor();
}

