/* mutest.h
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

/* @file mutest.h
 * 
 * This file declares the routines used by your application to
 * cause your unit tests to be run.
 */


/* @file mutest.h
 *
 * This file contains the declarations and all the Assert macros
 * required to use mutest in your own applications.
 *
 * To compile mutest to run its own unit tests, do this:
 * 
 * <pre>
 * 	$ make
 *  cc -g -Wall -Werror mutest.c main.c mutest_tests.c -o mutest
 * 	$ ./mutest
 * 	4 tests run, 4 successes (132 assertions).
 * </pre>
 *
 * If your non-gcc compiler complains about a missing __func__ macro,
 * add -D__func__='"test"' to the compiler's command line.
 *
 * See ::mutest_tests for instructions on how to add mutest's
 * built-in unit tests to your application's test suite.
 */


#ifndef MUTEST_H
#define MUTEST_H


/** Determines if user wants to run unit tests.  Returns 1 if unit
 * tests should be run, 0 if not.  Feel free to ignore this routine
 * and make your own decision on whether to run the unit tests.
 *
 * If the first arg passed to your program is "--run-unit-tests", this 
 * function return true.  It may also parse subsequent argumets to
 * determine how to run the tests and set run-time flags appropriately.
 */

int mutest_should_run_tests(int argc, char **argv);


/** Prints the results of a test run.
 */

void print_mutest_results();


/** Run your unit tests, then call mutest_exit to print the result and quit.
 */

void mutest_exit();


/** mutest's built-in test suite.
 *
 * This allows you to add the mutest unit test suite to your application's
 * test suites.  This way, you can ensure that mutest's unit tests pass
 * before running your application's.  This is for the especially pedantic. :)
 *
 * Unfortunately, there is one test that cannot be run if you do this:
 * ensuring that mutest properly handles empty test suites.
 * Other than this one test, adding mutest_tests
 * to your application's test suite is equivalent to causing mutest to
 * compile and run its unit tests as described in mutest.h.
 */

void run_mutest_unit_tests();


// TODO: this is a hack.  Handle it better.
void mutest_show_failures();

#endif
