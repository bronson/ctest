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
#include "ctassert.h"
#include <string.h>


int main(int argc, char **argv)
{
	if(ctest_read_args(argc, argv)) {
		printf("ctest_read_args returned true!\n");
	}

	/* Check for special-purpose tests */
	for(;*argv;argv++) {
		if(strcmp(*argv,"--fail-assert") == 0) {
			/* intentionally fail an assert */
			AssertEQ(1,0);
		}
		if(strcmp(*argv,"--fail-test") == 0) {
			/* intentionally fail a test */
			ctest_start("FailTest") {
				AssertEQ(1,0);
			}
			ctest_exit();
			return 0;
		}
	}

	/* Ensure that we can hit asserts without first calling ctest_start. */
	AssertEQ(1,1);

	/* Run unit tests for all assert files we know about */
	run_ctassert_tests();

	ctest_exit();
	
	/* this will never be reached */
	return 0;
}
