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

// This include is unfortunate...  TODO: try to get this out of here.
#include <setjmp.h>
extern jmp_buf *mutest_inversion;

/** Keeps track of how many assertions have been made.
 * This needs to be updated manually each time an assertion
 * is made.  The mutest built-in assertion macros all
 * update this variable properly.
 */

extern int mutest_assertions;
extern int mutest_tests_run;
extern int mutest_successes;
extern int mutest_failures;
extern jmp_buf mutest_test_bail;

typedef void (*mutest_proc)();

/** Runs all the tests in a suite. */
void run_mutest_suite(mutest_proc proc);
/** Runs all the tests in all the suites passed in. */
void run_mutest_suites(mutest_proc proc);

void print_mutest_results();


/** 
 *
 * Call this on the very first line of your application.  If the user
 * ran your program with the first arg of "--run-unit-tests", this will
 * run the tests and exit.  Otherwise your program will run as normal.
 * If you would rather create a dedicated executable, just call
 * run_mutest_suites() directly.
 */

void unit_test_check(int argc, char **argv, mutest_proc proc);


/**
 *
 * This runs all the unit tests supplied and then exits.  Use this
 * if you want to handle the arguments yourself.
 */

void run_unit_tests(mutest_proc proc);
void run_unit_tests_showing_failures(mutest_proc proc);


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

void mutest_tests();


/** A user-supplied function to run all tests in the project.
 * 
 * You must supply this function yourself.  If you want your tests
 * to include mutest's self-consistency checks (and you should),
 * make sure your all_tests() function calls mutest_tests().
 */

void all_tests();


#endif
