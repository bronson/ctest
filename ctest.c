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
#include "ctest_test.h"


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

// Metrics
static int tests_run = 0;					///< The number of tests that we have attempted.
static int test_successes = 0;				///< The number of successful tests run
static int test_failures = 0;				///< The number of failed tests run.
static int assertions_run = 0;				///< The number of assertions that have been attempted.
static int assertion_successes = 0;		///< The number of assertions that have passed (if inverted, failed assertions are counted as having succeeded).
static int assertion_failures = 0;			///< The number of assertions that have failed (if inverted, successful assertions are counted as failures).

// Options
static int show_failures = 0; 	///< Set this to 1 to print the failures.  This allows you to view the output of each failure to ensure it looks OK.
static int verbose = 0;



struct test {
	struct test *next;			///< usesd to maintain singly linked list of tests off ::test_head.
	struct ctest_jmp_wrapper jmp;	///< used to longjump out of the current test if an assertion fails.
	const char *name;			///< the name of the test or NULL if none was supplied
	int finished;				///< True if we've already run the test in its entirety, false if not.  Needed because ctest_internal_test_finished() must be called twice: once when the test block is entered, and once when the block is exited.  We're only interested in the exit.
	int inverted;				///< True if we should treat a failure as success and vice-versa (for testing ctest itself)
	int impromptou;				///< True if the test struct was created due to assert being called outside of ctest_start.  The test should be disposed when the assert completes.
};
struct test *test_head;					///< tests are listed of this list head, from most nested to least nested.

static void test_push(struct test *test)
{
	// link the test into the stack of running tests
	test->next = test_head;
	test_head = test;
}

static void test_pop()
{
	struct test *test = test_head;
	test_head = test->next;
	free(test);
}

static void test_print(const char *fmt, ...)
{
	va_list ap;
	struct test *test;
	
	if(!verbose) {
		return;
	}
	
	// first, print indentation
	for(test=test_head; test; test=test->next) {
		fprintf(stderr, "  ");
	}
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}



struct assert {
	struct assert *next;
	const char *file;	///< the filename of the current assertion (as given by the prepare function)
	int line;			///< the line number of the current assertion
	const char *func;	///< the name of the function that contains the current assertion
};
struct assert *assert_head;	  ///< assertions are listed off this list head, from most nested to least nested.


/** Add an assert to the list of asserts currently being checked.
 *
 *  Why do asserts nest?  Because sometimes you want to create an
 *  assert based on other asserts.
 */

static void assert_push(struct assert *assert)
{
	// link the test into the stack of running tests
	assert->next = assert_head;
	assert_head = assert;
}

static void assert_pop()
{
	struct assert *assert = assert_head;
	assert_head = assert->next;
	free(assert);
}

void ctest_assert_prepare(const char *file, int line,
		const char *func, const char *assertion)
{
	struct assert* assert;

	if(!test_head) {
		// We were called without a surrounding ctest_start block
		if(setjmp(ctest_internal_start_test(0, file, line, func, 1)->jmp)) {
                        ctest_internal_test_jumped();
			// we'll exit if any assertion fails, just like assert().
			exit(0);
                }
		
		test_head->impromptou = 1;
		// fake the first call to internal_test_finished.
		// The real call will come when the assert resolves.
		ctest_internal_test_finished();
	}

	assert = malloc(sizeof(struct assert));
	if(!assert) {
		fprintf(stderr, "Out of memory allocating struct assert!\n");
		exit(244);
	}
	
	assert->file = file;
	assert->line = line;
	assert->func = func;
	
	assert_push(assert);
	
	assertions_run += 1;
	test_print("%d. checking %s at %s:%d in %s()\n",
			assertions_run, assertion, file, line, func);
}



void ctest_assert_failed(const char *msg, ...)
{
	va_list ap;
	
	if(!assert_head) {
		fprintf(stderr, "assert failed without ctest_assert_prepare being called first!\n");
		exit(243);
	}
	
	if(!test_head->inverted || show_failures) {
		fprintf(stderr, "%s:%d: In %s, assert ",
				assert_head->file, assert_head->line, assert_head->func);
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
		fputc('\n', stderr);
	}
	
	assert_pop();

	if(!test_head) {
		fprintf(stderr, "assert_failed with a nil test_head.  This should be impossible!\n");
		exit(242);
	}
	
	if(test_head->inverted) {
		// test was inverted and it failed so return normally!
		// don't need to worry about impromptou tests since it's impossible to invert them.
		assertion_successes += 1;
		return;
	}
	
	// we need to jump out of this start_test block
	assertion_failures += 1;
	longjmp(test_head->jmp.jmp, 1);
}


void ctest_assert_succeeded()
{
	if(!assert_head) {
		fprintf(stderr, "assert succeeded without ctest_assert_prepare being called first!\n");
		exit(241);
	}
	
	assert_pop();
	
	if(!test_head) {
		fprintf(stderr, "assert_succeeded with a nil test_head.  This should be impossible!\n");
		exit(240);
	}

	if(test_head->inverted) {
		assertion_failures += 1;
		longjmp(test_head->jmp.jmp, 1);
	}
	
	assertion_successes += 1;

	if(test_head->impromptou) {
                ctest_internal_test_finished();
	}
}


static void ctest_start_test(const char *name, const char *file, int line,
	const char *func, const char *inv, int impromptou)
{
	struct test* test = malloc(sizeof(struct test));
	if(!test) {
		fprintf(stderr, "Out of memory allocating struct test!\n");
		exit(239);
	}
	
	if(!name || !name[0]) {
		name = "(unnamed)";
	}
	
	if(assert_head) {
		fprintf(stderr, "Previous assertion at %s:%d in %s() hasn't reported a result!\n",
				assert_head->file, assert_head->line, assert_head->func);
		fprintf(stderr, "Unable to start a new test %s at %s:%d in %s().\n",
				name, file, line, func);
		exit(238);
	}
	
	test->name = name;
	test->finished = 0;
	test->inverted = 0;
	test->impromptou = impromptou;
	
	test_push(test);

	tests_run += 1;
	if(!impromptou) {
		test_print("%d. starting %stest %s at %s:%d in %s() {\n", 
				tests_run, inv, name, file, line, func);
	}
}

struct ctest_jmp_wrapper* ctest_internal_start_test(const char *name,
		const char *file, int line, const char *func, int impromptou)
{
	ctest_start_test(name, file, line, func, "inverted ", impromptou);
	test_head->inverted = 0;
	return &test_head->jmp;
}

struct ctest_jmp_wrapper* ctest_internal_start_inverted_test(const char *name,
		const char *file, int line, const char *func)
{
	ctest_start_test(name, file, line, func, "inverted ", 0);
	test_head->inverted = 1;
	return &test_head->jmp;
}


void ctest_internal_test_jumped(const char *name)
{
	if(!test_head) {
		fprintf(stderr, "Internal jump error: somehow ctest_start didn't complete?\n");
		exit(244);
	}
	
	if(test_head->inverted) {
		test_successes += 1;
	} else {
		test_failures += 1;
	}

	if(!test_head->impromptou) {
		test_print("}\n");
	}
	test_pop();
}


/** Called when the test has been completed without longjumping.
 * 
 * Note that this does not automatically mean that the test has succeeded!
 * If the sense of the test has been inverted, and if it then proceeds to
 * completion, that should be considered a failure.
 */

int ctest_internal_test_finished(const char *name)
{
	if(!test_head) {
		// how could we end up here without a test_head??
		fprintf(stderr, "Internal finish error: somehow ctest_start didn't complete?\n");
		exit(244);
	}
	
	if(!test_head->finished) {
		// we haven't run the test yet, so run it.
		test_head->finished = 1;
		return 1;
	}
	
	// Test has run, check the result.
	test_successes += 1;

	if(!test_head->impromptou) {
		test_print("}\n");
	}
	test_pop();
	return 0;
}


void print_ctest_results()
{
	if(test_failures == 0) {
		printf("All OK.  %d test%s run, %d successe%s (%d assertion%s).\n",
				tests_run, (tests_run == 1 ? "" : "s"),
				test_successes, (test_successes == 1 ? "" : "s"),
				assertions_run, (assertions_run == 1 ? "" : "s"));
		return;
	}

	printf("ERROR: %d failure%s in %d test%s run!\n",
			test_failures, (test_failures == 1 ? "" : "s"), 
			tests_run, (tests_run == 1 ? "" : "s"));
}


/** You should have already run all tests.  This call will print their
 * results (print_ctest_results()) and exit with the number of tests
 * that failed as the error code (up to 100).
 */

void ctest_exit()
{
	print_ctest_results();
	exit(test_failures < 100 ? test_failures : 100);
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

int ctest_should_run_tests(int argc, char **argv)
{
	if(argc > 1 && strcmp(argv[1],"--run-unit-tests") == 0) {
		return 1;
	}

	return 0;
}


void ctest_show_failures()
{
	show_failures = 1;
}

