/* mutest.c
 * Scott Bronson
 * 6 Mar 2006
 * 
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 *
 * Version 0.7,  16 Feb 2007 -- turn dependency tree into functions
 * Version 0.6,  26 Apr 2006 -- first version worth releasing
 * Released under the MIT License.
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
static jmp_buf *inversion;		///< If an assertion fails, and we're inverted, this is where we end up.  This is NULL except when running Zutest's internal unit tests.  See test_fail().
static int show_failures = 0; 	///< Set this to 1 to print the failures.  This allows you to view the output of each failure to ensure it looks OK.


void mutest_fail(const char *file, int line, const char *func, 
		const char *msg, ...)
{
	va_list ap;
	if(!inversion || show_failures) {
		fprintf(stderr, "%s:%d: In %s, assert ", file, line, func);
		va_start(ap, msg);
		vfprintf(stderr, msg, ap);
		va_end(ap);
		fputc('\n', stderr);
	}

	// If inversion is set, then an assert correctly failed.
	if(inversion) {
		longjmp(*inversion, 1);
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



/* This code runs the mutest unit tests to ensure that mutest itself
 * is working properly.
 */


/** This macro is used to reverse the sense of the tests. 
 *
 * To properly test Zutest, we need to ensure that the Assert macros
 * handle failures too.  Therefore, we occasionally want to reverse
 * the sense of the macro, where a failure indicates a successful test
 * and a passing assert means that the test has failed.
 *
 * This macro inverts the sense of the contained assertion.
 * test_failure(AssertEq(a,b)) causes the test to pass
 * only when the assertion fails (i.e. when a != b).
 */

#define test_failure(test) 				\
	do { 								\
		jmp_buf jb; 					\
		int val = setjmp(jb); 			\
		if(val == 0) { 					\
			inversion = &jb;			\
			do { test; } while(0);		\
			inversion = NULL;			\
			Fail("This test should have failed: " #test);	\
		}								\
		inversion = NULL;				\
	} while(0)



void test_assert_int()
{
	int a=4, b=3, c=4, z=0, n=-1;

	AssertEq(a,c);
	AssertNe(a,b);
	AssertGt(a,b);
	AssertGe(a,b);
	AssertGe(a,c);
	AssertLt(b,a);
	AssertLe(b,a);
	AssertLe(c,a);

	test_failure( AssertEq(a,b) );
	test_failure( AssertNe(a,c) );
	test_failure( AssertGt(a,c) );
	test_failure( AssertGt(b,c) );
	test_failure( AssertGe(b,a) );
	test_failure( AssertLt(c,a) );
	test_failure( AssertLt(c,b) );
	test_failure( AssertLe(a,b) );

	AssertZero(z);
	test_failure( AssertZero(a) );
	AssertNonzero(a);
	test_failure( AssertNonzero(z) );

	AssertPositive(a);
	test_failure( AssertPositive(z) );
	test_failure( AssertPositive(n) );

	AssertNonPositive(n);
	AssertNonPositive(z);
	test_failure( AssertNonPositive(a) );

	AssertNegative(n);
	test_failure( AssertNegative(z) );
	test_failure( AssertNegative(a) );

	AssertNonNegative(a);
	AssertNonNegative(z);
	test_failure( AssertNonNegative(n) );
}


void test_assert_hex()
{
	int a=4, b=3, c=4, z=0, n=-1;

	AssertEqHex(a,c);
	AssertNeHex(a,b);
	AssertGtHex(a,b);
	AssertGeHex(a,b);
	AssertGeHex(a,c);
	AssertLtHex(b,a);
	AssertLeHex(b,a);
	AssertLeHex(c,a);

	test_failure( AssertEqHex(a,b) );
	test_failure( AssertNeHex(a,c) );
	test_failure( AssertGtHex(a,c) );
	test_failure( AssertGtHex(b,c) );
	test_failure( AssertGeHex(b,a) );
	test_failure( AssertLtHex(c,a) );
	test_failure( AssertLtHex(c,b) );
	test_failure( AssertLeHex(a,b) );

	AssertZeroHex(z);
	test_failure( AssertZeroHex(a) );
	AssertNonzeroHex(a);
	test_failure( AssertNonzeroHex(z) );

	AssertPositiveHex(a);
	test_failure( AssertPositiveHex(z) );
	test_failure( AssertPositiveHex(n) );

	AssertNonPositiveHex(n);
	AssertNonPositiveHex(z);
	test_failure( AssertNonPositiveHex(a) );

	AssertNegativeHex(n);
	test_failure( AssertNegativeHex(z) );
	test_failure( AssertNegativeHex(a) );

	AssertNonNegativeHex(a);
	AssertNonNegativeHex(z);
	test_failure( AssertNonNegativeHex(n) );
}


void test_assert_ptr()
{
	int a, b;
	int *ap = &a;
	int *bp = &b;
	int *cp = &a;
	int *n = NULL;

	AssertPtr(ap);
	AssertNull(n);

	test_failure( AssertPtr(n) );
	test_failure( AssertNull(ap) );

	AssertPtrEq(ap,cp);
	AssertPtrNe(ap,bp);
	AssertPtrGt(ap,bp);
	AssertPtrGe(ap,bp);
	AssertPtrGe(ap,cp);
	AssertPtrLt(bp,ap);
	AssertPtrLe(bp,ap);
	AssertPtrLe(cp,ap);

	test_failure( AssertPtrEq(ap,bp) );
	test_failure( AssertPtrNe(ap,cp) );
	test_failure( AssertPtrGt(ap,cp) );
	test_failure( AssertPtrGt(bp,cp) );
	test_failure( AssertPtrGe(bp,ap) );
	test_failure( AssertPtrLt(cp,ap) );
	test_failure( AssertPtrLt(cp,bp) );
	test_failure( AssertPtrLe(ap,bp) );
}


void test_assert_float()
{
	float a=0.0004, b=0.0003, c=0.0004;

	AssertFloatEq(a,c);
	AssertFloatNe(a,b);
	AssertFloatGt(a,b);
	AssertFloatGe(a,b);
	AssertFloatGe(a,c);
	AssertFloatLt(b,a);
	AssertFloatLe(b,a);
	AssertFloatLe(c,a);

	test_failure( AssertFloatEq(a,b) );
	test_failure( AssertFloatNe(a,c) );
	test_failure( AssertFloatGt(a,c) );
	test_failure( AssertFloatGt(b,c) );
	test_failure( AssertFloatGe(b,a) );
	test_failure( AssertFloatLt(c,a) );
	test_failure( AssertFloatLt(c,b) );
	test_failure( AssertFloatLe(a,b) );

	AssertDoubleEq(a,c);
	AssertDoubleNe(a,b);
	AssertDoubleGt(a,b);
	AssertDoubleGe(a,b);
	AssertDoubleGe(a,c);
	AssertDoubleLt(b,a);
	AssertDoubleLe(b,a);
	AssertDoubleLe(c,a);

	test_failure( AssertDoubleEq(a,b) );
	test_failure( AssertDoubleNe(a,c) );
	test_failure( AssertDoubleGt(a,c) );
	test_failure( AssertDoubleGt(b,c) );
	test_failure( AssertDoubleGe(b,a) );
	test_failure( AssertDoubleLt(c,a) );
	test_failure( AssertDoubleLt(c,b) );
	test_failure( AssertDoubleLe(a,b) );
}


void test_assert_strings()
{
	const char *a = "Bogozity";
	const char *b = "Arclamp";
	const char *c = "Bogozity";
	const char *e = "";
	const char *n = NULL;

	AssertStrEq(a,c);
	AssertStrNe(a,b);
	AssertStrGt(a,b);
	AssertStrGe(a,b);
	AssertStrGe(a,c);
	AssertStrLt(b,a);
	AssertStrLe(b,a);
	AssertStrLe(c,a);

	test_failure( AssertStrEq(a,b) );
	test_failure( AssertStrNe(a,c) );
	test_failure( AssertStrGt(a,c) );
	test_failure( AssertStrGt(b,c) );
	test_failure( AssertStrGe(b,a) );
	test_failure( AssertStrLt(c,a) );
	test_failure( AssertStrLt(c,b) );
	test_failure( AssertStrLe(a,b) );

	AssertStrEmpty(e);
	test_failure( AssertStrEmpty(a) );
	test_failure( AssertStrEmpty(n) );

	AssertStrNonEmpty(a);
	test_failure( AssertStrNonEmpty(e) );
	test_failure( AssertStrNonEmpty(n) );
}


void mutest_tests()
{
	mutest( test_assert_int() );
	mutest( test_assert_hex() );
	mutest( test_assert_ptr() );
	mutest( test_assert_float() );
	mutest( test_assert_strings() );
};
