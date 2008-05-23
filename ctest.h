/* ctest.h
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

/* @file ctest.h
 * 
 * This file declares the routines used by your application to
 * cause your unit tests to be run.
 */


/* @file ctest.h
 *
 * This file contains the declarations and all the Assert macros
 * required to use ctest in your own applications.
 *
 * To compile ctest to run its own unit tests, do this:
 * 
 * <pre>
 * 	$ make
 *  cc -g -Wall -Werror ctest.c main.c ctest_tests.c -o ctest
 * 	$ ./ctest
 * 	4 tests run, 4 successes (132 assertions).
 * </pre>
 *
 * If your non-gcc compiler complains about a missing __func__ macro,
 * add -D__func__='"test"' to the compiler's command line.
 *
 * See ::ctest_tests for instructions on how to add ctest's
 * built-in unit tests to your application's test suite.
 */


#ifndef CTEST_H
#define CTEST_H


/** Call your unit test routines, then call ctest_exit
 *  to print the results and quit.  Testing is that easy!
 */

void ctest_exit();


/** Determines if user wants to run unit tests.  Returns 1 if unit
 * tests should be run, 0 if not.  Feel free to ignore this routine
 * and make your own decision on whether to run the unit tests.
 *
 * If the first arg passed to your program is "--run-unit-tests", this 
 * function return true.  It may also parse subsequent argumets to
 * determine how to run the tests and set run-time flags appropriately.
 */

int ctest_should_run_tests(int argc, char **argv);



/** ctest's built-in test suite.
 *
 * This allows you to call ctest's unit test suite from your application's
 * test suites.  This lets you ensure that ctest's unit tests pass
 * before running your application's.
 *
 * Well, there is one test that cannot be run if you do this:
 * ensuring that ctest properly handles empty test suites.
 */

void ctest_run_unit_tests();


/* TODO: this is a hack.  Handle it better. */
void ctest_show_failures();

#endif
