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
#include "mutest_test.h"


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
	struct mutest_jmp_wrapper jmp;	///< used to longjump out of the current test if an assertion fails.
	const char *name;			///< the name of the test or NULL if none was supplied
	const char *description;	///< the longer description of the current test or NULL if none was supplied.
	int finished;				///< True if we've already run the test in its entirety, false if not.
	int inverted;				///< True if we should treat a failure as success and vice-versa (for testing mutest itself)
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

void mutest_assert_prepare(const char *file, int line,
		const char *func, const char *assertion)
{
	struct assert* assert = malloc(sizeof(struct assert));
	if(!assert) {
		fprintf(stderr, "Out of memory allocating struct assert!");
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



void mutest_assert_failed(const char *msg, ...)
{
	va_list ap;
	
	if(!assert_head) {
		fprintf(stderr, "assert failed without mutest_assert_prepare being called first!");
		exit(0);
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
		test_print("assertion was not wrapped by start_test, mutest must exit!\n");
		exit(244);
	}
	
	if(test_head->inverted) {
		// test was inverted and it failed so return normally!
		assertion_successes += 1;
		return;
	}
	
	// we need to jump out of this start_test block
	assertion_failures += 1;
	longjmp(test_head->jmp.jmp, 1);
}


void mutest_assert_succeeded()
{
	if(!assert_head) {
		fprintf(stderr, "assert succeeded without mutest_assert_prepare being called first!");
		exit(0);
	}
	
	assert_pop();
	
	if(test_head->inverted) {
		assertion_failures += 1;
		longjmp(test_head->jmp.jmp, 1);
	}
	
	assertion_successes += 1;
	return;
}


static void mutest_start_test(const char *name, const char *desc,
		const char *file, int line, const char *func, const char *inv)
{
	struct test* test = malloc(sizeof(struct test));
	if(!test) {
		fprintf(stderr, "Out of memory allocating struct test!");
		exit(244);
	}
	
	if(!name || !name[0]) {
		name = "(unnamed)";
	}
	
	if(assert_head) {
		fprintf(stderr, "Previous assertion at %s:%d in %s() hasn't reported a result!\n",
				assert_head->file, assert_head->line, assert_head->func);
		fprintf(stderr, "Unable to start a new test %s at %s:%d in %s().\n",
				name, file, line, func);
		exit(244);
	}
	
	tests_run += 1;
	test_print("%d. starting %stest %s at %s:%d in %s() {\n", 
			tests_run, inv, name, file, line, func);
	
	test->name = name;
	test->description = desc;
	test->finished = 0;
	test->inverted = 0;
	
	test_push(test);
}

struct mutest_jmp_wrapper* mutest_internal_start_test(const char *name, const char *desc,
		const char *file, int line, const char *func)
{
	mutest_start_test(name, desc, file, line, func, "inverted ");
	test_head->inverted = 0;
	return &test_head->jmp;
}

struct mutest_jmp_wrapper* mutest_internal_start_inverted_test(const char *name, const char *desc,
		const char *file, int line, const char *func)
{
	mutest_start_test(name, desc, file, line, func, "inverted ");
	test_head->inverted = 1;
	return &test_head->jmp;
}


void mutest_internal_test_jumped(const char *name, const char *desc)
{
	if(!test_head) {
		fprintf(stderr, "Internal jump error: somehow mutest_start didn't complete?");
		exit(244);
	}
	
	if(test_head->inverted) {
        test_successes += 1;
	} else {
		test_failures += 1;
	}

	test_pop();
	test_print("}\n");
}


/** Called when the test has been completed without longjumping.
 * 
 * Note that this does not automatically mean that the test has succeeded!
 * If the sense of the test has been inverted, if the test proceeds to
 * completion, that should be considered a failure.
 */

int mutest_internal_test_finished(const char *name, const char *desc)
{
	if(!test_head) {
		// how could we end up here without a test_head??
		fprintf(stderr, "Internal finish error: somehow mutest_start didn't complete?");
		exit(244);
	}
	
	if(!test_head->finished) {
		// we haven't run the test yet, so run it.
		test_head->finished = 1;
		return 1;
	}
	
	// Test has run, check the result.
	test_successes += 1;

	test_pop();
	test_print("}\n");
	return 0;
}


void print_mutest_results()
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


/** Runs all the unit tests in all the passed-in test suites.
 */

void run_unit_tests(mutest_proc proc)
{
	(*proc)();
	print_mutest_results();
	exit(test_failures < 100 ? test_failures : 100);
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
