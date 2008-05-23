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


/** The number of tests that we have attempted. */
static int tests_run = 0;
/** The number of successful tests run. */
static int test_successes = 0;
/** The number of failed tests run. */
static int test_failures = 0;
/** The number of assertions that have been attempted. */
static int assertions_run = 0;
/** The number of assertions that have passed. */
static int assertion_successes = 0;
/** The number of assertions that have failed. */
static int assertion_failures = 0;

/** Set this to 1 to print the failures.  This allows you to view the output of each failure to ensure it looks OK. */
static int show_failures = 0;
static int verbose = 0;


struct test {
	/** usesd to maintain singly linked list of tests off ::test_head. */
	struct test *next;
	/** used to longjump out of the current test if an assertion fails. */
	struct ctest_jmp_wrapper jmp;
	/** the name of the test or NULL if none was supplied */
	const char *name;
	/** True if we've already run the test in its entirety, false if not.  Needed because ctest_internal_test_finished() must be called twice: once when the test block is entered, and once when the block is exited.  We're only interested in the exit. */
	int finished;
	/** True if we should treat a failure as success and vice-versa (for testing ctest itself) */
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


/**
 * Print verbose information about the running tests and assertions.
 */

static void test_print(const char *fmt, ...)
{
	va_list ap;
	struct test *test;
	
	if(!verbose) {
		return;
	}
	
	/* print indentation */
	for(test=test_head; test; test=test->next) {
		fprintf(stderr, "  ");
	}
	
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}



struct assert {
	/** Links to the next assertion in the chain */
	struct assert *next;
	/** the filename of the current assertion (as given by the prepare function) */
	const char *file;
	/** the line number of the current assertion */
	int line;
};
/** assertions are listed off this list head, from most nested to least nested. */
struct assert *assert_head;


/** Add an assert to the list of asserts currently being checked.
 *
 *  Why do asserts nest?  I guess because sometimes you want to create an
 *  assert based on other asserts.  TODO: can nesting asserts be removed?
 */

static void assert_push(struct assert *assert)
{
	assert->next = assert_head;
	assert_head = assert;
}

/** Pop the topmost assert and dispose of it, move the next one into place */
static void assert_pop()
{
	struct assert *assert = assert_head;
	assert_head = assert->next;
	free(assert);
}


/** Prepare to test an assertion.  This does all the common work
 *  like ensuring a test is ready to go and printing the status..
 */

void ctest_assert_prepare(const char *file, int line, const char *assertion)
{
	struct assert* assert;

	assert = malloc(sizeof(struct assert));
	if(!assert) {
		fprintf(stderr, "Out of memory allocating struct assert!\n");
		exit(244);
	}
	
	assert->file = file;
	assert->line = line;
	
	assert_push(assert);
	
	assertions_run += 1;
	test_print("%d. checking %s at %s:%d\n",
			assertions_run, assertion, file, line);
}


/** This gets called if the assertion set up with ctest_assert_prepare()
 *  failed.  It bails out of the current test.
 */

void ctest_assert_failed(const char *msg, ...)
{
	va_list ap;
	
	if(!assert_head) {
		fprintf(stderr, "assert failed without ctest_assert_prepare being called first!\n");
		exit(243);
	}
	
	if(!(test_head && test_head->inverted) || show_failures) {
		fprintf(stderr, "%s:%d: assert ", assert_head->file, assert_head->line);
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
		fputc('\n', stderr);
	}
	
	assert_pop();

	if(!test_head) {
		/* Assert failed and wasn't wrapped in a test.  Bail immediatley. */
		exit(1);
	}
	
	if(test_head->inverted) {
		/* test was inverted and it failed so return normally! */
		assertion_successes += 1;
		return;
	}
	
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
	
	if(test_head && test_head->inverted) {
		assertion_failures += 1;
		longjmp(test_head->jmp.jmp, 1);
	}
	
	assertion_successes += 1;
}


static void ctest_start_test(const char *name, const char *file, int line, const char *inv)
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
		fprintf(stderr, "Previous assertion at %s:%d hasn't reported a result!\n",
				assert_head->file, assert_head->line);
		fprintf(stderr, "Unable to start a new test %s at %s:%d.\n",
				name, file, line);
		exit(238);
	}
	
	test->name = name;
	test->finished = 0;
	test->inverted = 0;
	
	tests_run += 1;
	test_print("t%d. starting %stest %s at %s:%d {\n",
			tests_run, inv, name, file, line);

	test_push(test);
}

struct ctest_jmp_wrapper* ctest_internal_start_test(const char *name,
		const char *file, int line)
{
	ctest_start_test(name, file, line, "");
	test_head->inverted = 0;
	return &test_head->jmp;
}

struct ctest_jmp_wrapper* ctest_internal_start_inverted_test(const char *name,
		const char *file, int line)
{
	ctest_start_test(name, file, line, "inverted ");
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

	test_pop();
	test_print("}\n");
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
		/* how could we end up here without a test_head?? */
		fprintf(stderr, "Internal finish error: somehow ctest_start didn't complete?\n");
		exit(244);
	}
	
	if(!test_head->finished) {
		/* we haven't run the test yet, so run it. */
		test_head->finished = 1;
		return 1;
	}
	
	test_successes += 1;
	test_pop();
	test_print("}\n");

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

