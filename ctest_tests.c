/* test_tests.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * Specifies all the tests that should be run by
 * the standalone ctest executable.
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
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
	// Ensure that we can hit asserts without first calling ctest_start.
	AssertEQ(1,1);

	// TODO: write a test to fork and make sure that a failing
	// assert causes the app to exit immediately.

	ctest_test_assert_flavor();
}

