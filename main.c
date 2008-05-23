/* main.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * Specifies a main routine so that ctest can be compiled standalone.
 * Requires all_tests(), a routine that runs all tests for the
 * current project, to be defined in another file.
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

#include "ctest.h"

#include "ct_assert.h"


int main(int argc, char **argv)
{
	if(argc > 1) {
		/* "ctest -f" prints all the failures in the ctest unit tests. */
		/* This allows you to check the output of each macro. */
		ctest_show_failures();
	}

	/* Ensure that we can hit asserts without first calling ctest_start. */
	AssertEQ(1,1);

	/* TODO: write a test to fork and make sure that a failing
	 * assert causes the app to exit immediately.
	 */

	/* Run unit tests for all assert files we know about */
	ctest_test_asserts();

	ctest_exit();
	
	/* this will never be reached */
	return 0;
}
