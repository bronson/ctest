/* ctest_test.h
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

/* @file ctest_test.h
 * 
 * This file provides routines to be called by unit tests during testing.
 */


#ifndef CTEST_TEST_H
#define CTEST_TEST_H

#include <setjmp.h>
#include <stdio.h>


/// This is very sad: because there are setjmp implementations that
/// store jmp_buf as an array instead of a struct, I have to wrap
/// it in a struct to be able to pass it between functions.
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

// The for loop is so that ctest_internal_test_finished() called
// when the flow of control exits the block that follows ctest_start.
#define ctest_start(name) 									\
		if(setjmp(ctest_internal_start_test(name, __FILE__,__LINE__,0)->jmp)) {	\
			ctest_internal_test_jumped();							\
		} else for(; ctest_internal_test_finished(); )


/** Starts a new test with reversed results.
 * 
 * i.e. if the assertion succeeds, the test fails because
 * the assertion was actually supposed to fail.  And if the
 * assertion fails, the test succeeds.
 * 
 * This is only used for testing ctest itself.  You should never have
 * a need to call this routine directly.  That would be perverse.
 */

#define ctest_start_inverted(name) 									\
		if(setjmp(ctest_internal_start_inverted_test(name, __FILE__,__LINE__)->jmp)) {	\
			ctest_internal_test_jumped();									\
		} else for(; ctest_internal_test_finished(); )


/** Prepares ctest to check an assertion
 * 
 * Mostly it just increments the numer of assertions counter
 * and prints the assertion if verbose is enabled.
 * 
 * You must make sure that ctest_assert_succeeded() or ctest_assert_failed()
 * will be called before ctest_assert_prepare() or ctest_start() can possibly
 * be called.
 */

void ctest_assert_prepare(const char *file, int line, const char *assertion);


/** Causes the current assertion to fail.
 *
 * You must have previously called ctest_assert_prepare().
 * Pass the message that should be printed as a printf format string.
 */

void ctest_assert_failed(const char *msg, ...);


/** Causes the current assertion to finish with success.
 * 
 * You must have previously called ctest_assert_prepare().
 */

void ctest_assert_succeeded();


// The following routines are not meant to be called directly; they are used
// by the mutest_start() macro and subject to change.
struct ctest_jmp_wrapper* ctest_internal_start_test(const char *name, const char *file, int line, int impromptou);		///< only used by ctest_start()
struct ctest_jmp_wrapper* ctest_internal_start_inverted_test(const char *name, const char *file, int line);		///< only used by ctest_start();
void ctest_internal_test_jumped();				///< only used by ctest_start()
int ctest_internal_test_finished();			///< only used by ctest_start()

#endif
