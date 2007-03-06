/* mutest.h
 * Scott Bronson
 * 6 Mar 2006
 */

/* @file mutest.h
 * 
 * This file provides routines to be called by unit tests during
 * testing. It is intended to be used by mutest flavor files (mu_*.h).
 * It is not meant to be included by your application directly.
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */


#ifndef MUTEST_TEST_H
#define MUTEST_TEST_H

// TODO: try to eradicate this #include.
#include <setjmp.h>


/** Causes the current test to fail.
 *
 * This function may only be called from within a ::mutest_proc.
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

void mutest_fail(const char *file, int line, const char *func, const char *msg, ...);
		

#define mutest(test) do { mutest_tests_run += 1; 	\
		if(!setjmp(mutest_test_bail)) { 			\
			do { test; } while(0); 					\
			mutest_successes += 1; 					\
		} else { 									\
			mutest_failures += 1; 					\
		} } while(0)
		

extern jmp_buf *mutest_inversion;

extern int mutest_assertions;
extern int mutest_successes;
extern int mutest_failures;
extern int mutest_tests_run;
extern jmp_buf mutest_test_bail;

#endif
