/* main.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 *
 * Specifies a main routine so that mutest can be compiled standalone.
 * Requires all_tests(), a routine that runs all tests for the
 * current project, to be defined in another file.
 */

#include "mutest.h"


int main(int argc, char **argv)
{
	if(argc > 1) {
		// "mutest -f" prints all the failures in the mutest unit tests.
		// This allows you to check the output of each macro.
		run_unit_tests_showing_failures(all_tests);
	} else {
		run_unit_tests(all_tests);
	}
	
	// this will never be reached
	return 0;
}
