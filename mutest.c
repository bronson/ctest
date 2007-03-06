/* mutest.c
 * Scott Bronson
 * 6 Mar 2006
 *
 * This file contains the code needed to find and launch mutests.
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "mutest.h"


/** @file mutest.c
 *
 * This file contains all of the test mechanisms provided by the
 * Zutest unit testing framework.
 *
 * A single function is called a test.  If any of the asserts fail
 * within a test, the test itself is stopped and printed as a failure
 * but all other tests in the current test suite, and all other test
 * suites, will still be run.
 * 
 * You need to ensure that it's impossible for Assert macros to be
 * called without first ensuring that test_enter is called.
 * 
 * A single test is typically:
 *   
 *   Generally each step in a test must be processed in order
 * (later steps depend on earlier).  This is why tmtest bails
 * on the entire test when the first assert fails -- chances are
 * that the failure will prevent any further processing in this
 * test from producing meaningful results, and is probably not
 * unlikely to cause a full crash.  Therefore when an assert fails,
 * the current test is stopped and the next test is started.
 *
 * Q: Why not collect tests in data structures like other unit test
 * program do?
 * A: Zutest used to do this.  However, you can't call a data structure
 * with different parameters.  For instance, ensuring a suite of tests
 * works with any sized buffer:
 *     test_buffer(1024);
 *     test_buffer(13);
 *     test_buffer(0);
 * And you can't set a breakpoint on a data structure or step through
 * it in a debugger.  So, while it might be conceptually cleaner to
 * specify your tests purely declarative, in practice it's significantly
 * less convenient.
 *
 * TODO: move test setup into the tests themselves.
 * TODO: print test results, test suites, etc as they run.
 *    Add a quiet flag that will suppress printing unless a test fails.
 *    quiet=0, full printing
 *    quiet=1, test results not printed
 *    quiet=2, suite results not printed
 *    quiet=3, summary not printed.
 */


int mutest_assertions = 0;		///< A goofy statistic, updated by the assertion macros
int mutest_tests_run = 0;		///< The number of tests that we have run.  successes+failures==tests_run (if not, then there's a bug somewhere).
int mutest_successes = 0;		///< The number of successful tests run
int mutest_failures = 0;		///< The number of failed tests run.
jmp_buf mutest_test_bail;		///< If an assertion fails, and we're not inverted, this is where we end up.
jmp_buf *mutest_inversion;		///< If an assertion fails, and we're inverted, this is where we end up.  This is NULL except when running Zutest's internal unit tests.  See test_fail().

static int show_failures = 0; 	///< Set this to 1 to print the failures.  This allows you to view the output of each failure to ensure it looks OK.


void mutest_fail(const char *file, int line, const char *func, 
		const char *msg, ...)
{
	va_list ap;
	if(!mutest_inversion || show_failures) {
		fprintf(stderr, "%s:%d: In %s, assert ", file, line, func);
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
		fputc('\n', stderr);
	}

	// If mutest_inversion is set, then an assert correctly failed.
	if(mutest_inversion) {
		longjmp(*mutest_inversion, 1);
	}

	longjmp(mutest_test_bail, 1);
}


void print_mutest_results()
{
	if(mutest_failures == 0) {
		printf("All OK.  %d test%s run, %d successe%s (%d assertion%s).\n",
				mutest_successes, (mutest_successes == 1 ? "" : "s"),
				mutest_successes, (mutest_successes == 1 ? "" : "s"),
				mutest_assertions, (mutest_assertions == 1 ? "" : "s"));
		return;
	}

	printf("ERROR: %d failure%s in %d test%s run!\n",
			mutest_failures, (mutest_failures == 1 ? "" : "s"), 
			mutest_tests_run, (mutest_tests_run == 1 ? "" : "s"));
}


/** Runs all the unit tests in all the passed-in test suites.
 */

void run_unit_tests(mutest_proc proc)
{
	(*proc)();
	print_mutest_results();
	exit(mutest_failures < 100 ? mutest_failures : 100);
}


void run_unit_tests_showing_failures(mutest_proc proc)
{
	show_failures = 1;
	run_unit_tests(proc);
}


/**
 * Examines the command-line arguments.  If "--run-unit-tests" is
 * the first argument, then it runs the unit tests (further arguments
 * may affect how the tests are processed).  This routine exits with
 * a nonzero result code if any test fails; otherwise it exits with 0.
 * It never returns.
 *
 * If --run-unit-tests is not on the command line, this routine returns
 * without doing anything.
 */

void unit_test_check(int argc, char **argv, mutest_proc proc)
{
	if(argc > 1 && strcmp(argv[1],"--run-unit-tests") == 0) {
		run_unit_tests(proc);
	}
}
