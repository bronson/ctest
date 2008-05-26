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
 * To run ctest's unit tests:
 * 
 * <pre>
 * 	$ make
 *  cc -g -Wall -Werror ctest.c main.c ctest_tests.c -o ctest
 * 	$ ./ctest
 * 	4 tests run, 4 successes (132 assertions).
 * </pre>
 *
 * See ::ctest_tests for instructions on how to add ctest's
 * built-in unit tests to your application's test suite.
 */


#ifndef CTEST_H
#define CTEST_H

#include <setjmp.h>
#include <stdio.h>


/** You can change ctest's run-time behavior at any time by modifying
 *  this structure.  For instance, ctest_preferences.verbosity = 4;
 */
extern struct ctest_preferences {
	/** 0=normal verbosity (show failures and a one-line summary at the end.
         *  Increase this number to increase the amount of information printed. */
	int verbosity;
	/** Set this to 1 to print the failures.  This tells ctest to display the
         *  output of each inverted failure to ensure it looks OK. */
	int show_failures;
	/** Used to test the unit test suite itself.  It inverts the sense of the test:
	 * a failure is actually the expected behavior and gets counted as a success. */
	int inverted;
} ctest_preferences;


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


/* This is sad: because there are setjmp implementations that store jmp_buf
 * as an array instead of a struct, I have to wrap it in a struct to be able
 * to reliably pass the whole thing between functions.
 */
struct ctest_jmp_wrapper {
        jmp_buf jmp;
};


/** Starts a new test.
 * 
 * This call nests just fine.  A test failure always unwinds to the
 * most recent test_start call.  Therefore, if you have a lengthly
 * test and don't want a failure in the middle of it to bail all the
 * way out, just wrap the failing code in its own test_start.
 * 
 * Example:
 * <pre>
 *   ctest_start("mytest") {
 *       AssertEqual(1,1);
 *       AssertEqual(1,2);
 *   }
 * </pre>
 */

/* The for loop is so that ctest_internal_test_finished() called
 * when the flow of control exits the block that follows ctest_start.
 */
#define ctest_start(name) \
	if(setjmp(ctest_internal_start_test(name, __FILE__,__LINE__)->jmp)) { \
		ctest_internal_finish_test(1); \
	} else for(; ctest_internal_finish_test(0); )


/** Indicates that an assertion has been run with the given result.
 */

void ctest_assert(int result, const char *file, int line, const char *msg);
void ctest_assert_fmt(int result, const char *file, int line, const char *msg, ...);


/* The following routines are not meant to be called directly; they are used
 * by the ctest_start() macro and always subject to change.
 */
struct ctest_jmp_wrapper* ctest_internal_start_test(const char *name, const char *file, int line);
int ctest_internal_finish_test(int failure);

#endif
