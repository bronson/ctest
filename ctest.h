/* ctest.h
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
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


/** Determines if user wants to run unit tests.  Returns 1 if unit
 * tests should be run, 0 if not.  Feel free to ignore this routine
 * and make your own decision on whether to run the unit tests.
 *
 * If the first arg passed to your program is "--run-unit-tests", this 
 * function return true.  It may also parse subsequent argumets to
 * determine how to run the tests and set run-time flags appropriately.
 */

int ctest_should_run_tests(int argc, char **argv);


/** Prints the results of a test run.
 */

void print_ctest_results();


/** Run your unit tests, then call ctest_exit to print the result and quit.
 */

void ctest_exit();


/** ctest's built-in test suite.
 *
 * This allows you to add the ctest unit test suite to your application's
 * test suites.  This way, you can ensure that ctest's unit tests pass
 * before running your application's.  This is for the especially pedantic. :)
 *
 * Unfortunately, there is one test that cannot be run if you do this:
 * ensuring that ctest properly handles empty test suites.
 * Other than this one test, adding ctest_tests
 * to your application's test suite is equivalent to causing ctest to
 * compile and run its unit tests as described in ctest.h.
 */

void run_ctest_unit_tests();


// TODO: this is a hack.  Handle it better.
void ctest_show_failures();

#endif
