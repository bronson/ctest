/* mutest_test.h
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

/* @file mutest_testT.h
 * 
 * This file provides routines to be called by unit tests during
 * testing. It is intended to be used by mutest flavor files (mu_*.h).
 * and is not meant to be used by your application directly.
 */


#ifndef MUTEST_TEST_H
#define MUTEST_TEST_H

#include <setjmp.h>
#include <stdio.h>


/// This is very sad: because there are setjmp implementations that
/// store jmp_buf as an array instead of a struct, I have to wrap
/// it in a struct to be able to pass it between functions.
struct mutest_jmp_wrapper {
        jmp_buf jmp;
};


/** Starts a new test.
 * 
 * You must call mutest_start before making any assertions.
 * It's fine to call subroutines that themselves may make assertions.
 * You just need to ensure that somehow, up the call chain,
 * mutest_start has been called.
 * 
 * This call nests just fine.  A test failure always unwinds to the
 * most recent test_start call.  Therefore, if you have a lengthly
 * test and don't want a failure in the middle of it to bail all the
 * way out, just wrap the failing code in its own test_start.
 * 
 * Example:
 * <pre>
 *   mutest_start("mytest", "Tests a few elementary things") {
 *       AssertEqual(1,2);
 *   }
 * </pre>
 */

#define mutest_start(name, desc) 									\
		if(setjmp(mutest_internal_start_test(name, desc,__FILE__,__LINE__,__func__)->jmp)) {	\
			mutest_internal_test_jumped();							\
		} else for(; mutest_internal_test_finished(); )


/** Starts a new test with reversed results.
 * 
 * i.e. if the assertion succeeds, the test fails because
 * the assertion was actually supposed to fail.  And if the
 * assertion fails, the test succeeds.
 * 
 * This is only used for testing mutest itself.  You should never have
 * a need to call this routine directly.  That would be perverse.
 */

#define mutest_start_inverted(name, desc) 									\
		if(setjmp(mutest_internal_start_inverted_test(name, desc,__FILE__,__LINE__,__func__)->jmp)) {	\
			mutest_internal_test_jumped();									\
		} else for(; mutest_internal_test_finished(); )


/** Prepares mutest to check an assertion
 * 
 * Mostly it just increments the numer of assertions counter
 * and prints the assertion if verbose is enabled.
 * 
 * You must eventually call either mutest_assert_succeeded()
 * or mutest_assert_failed() before the next 
 * mutest_assert_prepare() or mutest_start().
 */

void mutest_assert_prepare(const char *file, int line, const char *func, const char *assertion);


/** Causes the current assertion to fail.
 *
 * This function may only be called within a mutest_start() block.
 * Also, you must have previously called mutest_assert_prepare().
 *
 * If none of the built-in Assert macros fit your fancy, you can do the
 * check on your own and call mutest_fail in the event that it fails.
 * 
 * Example:
 * 
 * <pre>
 * if(my_error) {
 *    mutest_fail(__FILE__, __LINE__, __func__, "Error Message %d", 1);
 * }
 * </pre>
 *
 * But, really, it's easier just to call the Fail() macro.
 */

void mutest_assert_failed(const char *msg, ...);


/** Causes the current assertion to succeed.
 * 
 * You must first call mutest_assert_prepare().
 */

void mutest_assert_succeeded();


struct mutest_jmp_wrapper* mutest_internal_start_test(const char *name, const char *description, const char *file, int line, const char *func);		///< only used by mutest_start()
struct mutest_jmp_wrapper* mutest_internal_start_inverted_test(const char *name, const char *desc, const char *file, int line, const char *func);		///< only used by mutest_start();
void mutest_internal_test_jumped();				///< only used by mutest_start()
int mutest_internal_test_finished();			///< only used by mutest_start()

#endif
