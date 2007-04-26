/* main.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * Specifies a main routine so that mutest can be compiled standalone.
 * Requires all_tests(), a routine that runs all tests for the
 * current project, to be defined in another file.
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

#include "mutest.h"


int main(int argc, char **argv)
{
	if(argc > 1) {
		// "mutest -f" prints all the failures in the mutest unit tests.
		// This allows you to check the output of each macro.
		mutest_show_failures();
	}

	run_mutest_unit_tests();
	mutest_exit();
	
	// this will never be reached
	return 0;
}
