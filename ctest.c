/* ctest.c
 * Scott Bronson
 * 6 Mar 2006
 *
 * This file contains the code needed to find and launch C unit tests.
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ctest.h"


/** @file ctest.c
 *
 * This file contains all of the test mechanisms provided by the
 * C unit testing framework.
 *
 * A single function is called a test.  If any of the asserts fail
 * within a test, the test itself is stopped and printed as a failure
 * but all other tests in the current test suite, and all other test
 * suites, will still be run.
 * 
 * If an assert fails outside of a test_enter block, a message is
 * printed and the testing program is exited.
 * 
 *
 * A single test is typically:
 *   
 * Generally each step in a test must be processed in order
 * (later steps depend on earlier).  This is why tmtest bails
 * on the entire test when the first assert fails -- chances are
 * that the failure will prevent any further processing in this
 * test from producing meaningful results, and is probably not
 * unlikely to cause a full crash.  Therefore when an assert fails,
 * the current test is stopped and the next test is started.
 *
 * Q: Why not collect tests in data structures like other unit test
 * program do?
 * A: Zutest used to do this (back when it was called ZuTest).
 * However, you can't call a data structure
 * with different parameters.  For instance, what if you wanted to
 * call the same test with different parameters?  You can't do this:
 *     test_buffer(1024);
 *     test_buffer(13);
 *     test_buffer(0);
 * And you can't set a breakpoint on a data structure or step through
 * it in a debugger.  So, while it might be conceptually cleaner to
 * specify your tests purely declaratively, in practice it's just
 * more hassle.
 */


static struct {
	/** The number of tests that we have attempted. */
	int tests_run;
	/** The number of successful tests run. */
	int test_successes;
	/** The number of failed tests run. */
	int test_failures;
	/** The number of assertions that have been attempted. */
	int assertions_run;
} metrics;


struct ctest_preferences ctest_preferences;


struct test {
	/** usesd to maintain singly linked list of tests off ::test_head. */
	struct test *next;
	/** used to longjump out of the current test if an assertion fails. */
	struct ctest_jmp_wrapper jmp;
	/** the name of the test or NULL if none was supplied */
	const char *name;
	/** True if we've already run the test in its entirety, false if not.  Needed because ctest_internal_test_finished() must be called twice: once when the test block is entered, and once when the block is exited.  We're only interested in the exit. */
	int finished;
	/** true if the sense of the assertions should be reversed. */
	int inverted;
};
/** tests are listed off this list head, from most nested to least nested. */
struct test *test_head;


/**
 * Link the given test into the stack of running tests.
 * Tests are linked from most nested to least nested off test_head.
 */

static void test_push(struct test *test)
{
	test->next = test_head;
	test_head = test;
}


/**
 * Dispose of the current test_head moving the next test in the chain into its place.
 */

static void test_pop()
{
	struct test *test = test_head;
	test_head = test->next;
	free(test);
}


static void print_test_indentation()
{
	struct test *test;

	for(test=test_head; test; test=test->next) {
		printf("  ");
	}
}


void ctest_assert(int success, const char *file, int line, const char *msg)
{
	if(!success && ctest_preferences.show_failures) {
		fprintf(stderr, "%s:%d: assert failed: %s!\n", file, line, msg);
	}

	if(test_head && test_head->inverted)
		success = !success;

	metrics.assertions_run += 1;
	if(success) {
		if(ctest_preferences.verbosity >= 2) {
			print_test_indentation();
			printf("%d. %sassert %s at %s:%d: success\n",
				metrics.assertions_run, 
				test_head && test_head->inverted ? "inverted " : "",
				msg, file, line);
		}
	} else {
		if(test_head) {
			if(test_head->inverted) {
				fprintf(stderr, "%s:%d: inverted assert was not expected to succeed: %s!\n", file, line, msg);
			}
			/* longjump to abort this test */
			longjmp(test_head->jmp.jmp, 1);
		} else {
			exit(1); /* 1 because a single test failed */
		}
	}
}


void ctest_assert_fmt(int success, const char *file, int line, const char *msg, ...)
{
	va_list ap;
	char buf[BUFSIZ];

	va_start(ap, msg);
	/* Thanks to awful standardization, I don't get to use vsnprintf.
         * For now we have an extremely unlikely but potential buffer overflow.
         * Longer term, should probably figure out how to get rid of varargs entirely. */
	vsprintf(buf, msg, ap);
	va_end(ap);

	ctest_assert(success, file, line, buf);
}


struct ctest_jmp_wrapper* ctest_internal_start_test(const char *name, const char *file, int line)
{
	struct test* test = malloc(sizeof(struct test));
	if(!test) {
		fprintf(stderr, "Out of memory allocating struct test!\n");
		exit(239);
	}
	
	if(!name || !name[0]) {
		name = "(unnamed)";
	}
	
	test->name = name;
	test->finished = 0;
	test->inverted = 0;
	
	metrics.tests_run += 1;
	if(ctest_preferences.verbosity >= 1) {
		print_test_indentation();
		printf("%d. Running %s at %s:%d%s\n",
			metrics.tests_run, name, file, line,
			ctest_preferences.verbosity >= 2 ? " {" : "");
	}

	test_push(test);
	return &test_head->jmp;
}


int ctest_internal_finish_test(int success)
{
	if(!test_head) {
		/* how could we end up here without a test_head?? */
		fprintf(stderr, "Internal finish error: somehow ctest_start didn't complete?\n");
		exit(243);
	}
	
	if(!test_head->finished) {
		/* we haven't run the test yet, so run it. */
		test_head->finished = 1;
		return 1;
	}
	
	if(success || test_head->inverted) {
		metrics.test_successes += 1;
	} else {
		metrics.test_failures += 1;
	}

	test_pop();

	if(ctest_preferences.verbosity >= 2) {
		print_test_indentation();
		printf("}\n");
	}

	return 0;
}


void print_ctest_results()
{
	if(metrics.test_failures == 0) {
		printf("All OK.  %d test%s run, %d successe%s (%d assertion%s).\n",
			metrics.tests_run, (metrics.tests_run == 1 ? "" : "s"),
			metrics.test_successes, (metrics.test_successes == 1 ? "" : "s"),
			metrics.assertions_run, (metrics.assertions_run == 1 ? "" : "s"));
	} else {
		printf("ERROR: %d failure%s in %d test%s run!\n",
			metrics.test_failures, (metrics.test_failures == 1 ? "" : "s"), 
			metrics.tests_run, (metrics.tests_run == 1 ? "" : "s"));
	}
}


/** You should have already run all tests.  This call will print their
 * results (print_ctest_results()) and exit with the number of tests
 * that failed as the error code (up to 100).
 */

void ctest_exit()
{
	print_ctest_results();
	exit(metrics.test_failures < 100 ? metrics.test_failures : 100);
}


/** Normally, a failed assert causes the currently running test to be aborted.
 *  But how do unit tests test their assert macros?  Invert the sense.
 *  That way, if the assertion fails, that's what we expected, and the
 *  test actually succeeds.  To start or stop inverting, call this routine.
 */
int ctest_toggle_inversion()
{
	if(!test_head) {
		fprintf(stderr, "Called ctest_toggle_inversion without having started a test!\n");
		exit(240);
	}
	test_head->inverted = !test_head->inverted;
	return test_head->inverted;
}


/** Parses command-line arguments into ctest_preferences. 
 * 
 * Intended to be called before your application's command-line handling.
 *
 * Returns true if the user specified that unit tests should be run,
 * false if not.  Feel free to ignore the function result and implement
 * your own way of deciding whether or not to run the tests.
 *
 * Arguments:
 *  * --ctest, --run-unit-tests: run unit tests and exit instead of starting the app.
 *  * -v: be more verbose.  Specify multiple to increase the verbosity.
 *  * --show-failures: print the output when inverted tests fail.
 *        Allows you to see ctest's output for failing tests.
 *
 * NOTE: this routine does not display any errors.  If you mis-type, the
 * argument will be silently ignored.
 */

int ctest_read_args(int argc, char **argv)
{
	char *curarg;
	int return_value = 0;

	/* Don't use getopt because it's not on very many platforms.
	 * Just do something super-simple.
	 */
	while(argc--) {
		curarg = *argv++;

		if(strcmp(curarg, "--ctest") == 0 ||
		   strcmp(curarg, "--run-unit-tests") == 0) {
			return_value = 1;
		} else if(strcmp(curarg, "--show-failures") == 0) {
			ctest_preferences.show_failures = 1;
		} else if(*curarg == '-') {
			switch(*++curarg) {
			case 'v':
				while(*curarg++ == 'v')
					ctest_preferences.verbosity += 1;
				break;
			default: ; /* do nothing */
			}
		} else {
			/* unknown argument */
		}
	}

	return return_value;
}

