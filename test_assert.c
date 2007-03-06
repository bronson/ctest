/* test_Assert.c
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Tests the AssertEqual(a,b) style tests from mutest_Assert.h
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */

#include <string.h>

#include "mu_assert.h"


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
			mutest_inversion = &jb;			\
			do { test; } while(0);		\
			mutest_inversion = NULL;			\
			Fail("This test should have failed: " #test);	\
		}								\
		mutest_inversion = NULL;				\
	} while(0)



void test_assert_int()
{
	int a=4, b=3, c=4, z=0, n=-1;

	AssertEQ(a,c);
	AssertNE(a,b);
	AssertGT(a,b);
	AssertGE(a,b);
	AssertGE(a,c);
	AssertLT(b,a);
	AssertLE(b,a);
	AssertLE(c,a);

	test_failure( AssertEQ(a,b) );
	test_failure( AssertNE(a,c) );
	test_failure( AssertGT(a,c) );
	test_failure( AssertGT(b,c) );
	test_failure( AssertGE(b,a) );
	test_failure( AssertLT(c,a) );
	test_failure( AssertLT(c,b) );
	test_failure( AssertLE(a,b) );

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

	AssertHexEQ(a,c);
	AssertHexNE(a,b);
	AssertHexGT(a,b);
	AssertHexGE(a,b);
	AssertHexGE(a,c);
	AssertHexLT(b,a);
	AssertHexLE(b,a);
	AssertHexLE(c,a);

	test_failure( AssertHexEQ(a,b) );
	test_failure( AssertHexNE(a,c) );
	test_failure( AssertHexGT(a,c) );
	test_failure( AssertHexGT(b,c) );
	test_failure( AssertHexGE(b,a) );
	test_failure( AssertHexLT(c,a) );
	test_failure( AssertHexLT(c,b) );
	test_failure( AssertHexLE(a,b) );

	AssertHexZero(z);
	test_failure( AssertHexZero(a) );
	AssertHexNonzero(a);
	test_failure( AssertHexNonzero(z) );

	AssertHexPositive(a);
	test_failure( AssertHexPositive(z) );
	test_failure( AssertHexPositive(n) );

	AssertHexNonPositive(n);
	AssertHexNonPositive(z);
	test_failure( AssertHexNonPositive(a) );

	AssertHexNegative(n);
	test_failure( AssertHexNegative(z) );
	test_failure( AssertHexNegative(a) );

	AssertHexNonNegative(a);
	AssertHexNonNegative(z);
	test_failure( AssertHexNonNegative(n) );
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

	AssertPtrEQ(ap,cp);
	AssertPtrNE(ap,bp);
	AssertPtrGT(ap,bp);
	AssertPtrGE(ap,bp);
	AssertPtrGE(ap,cp);
	AssertPtrLT(bp,ap);
	AssertPtrLE(bp,ap);
	AssertPtrLE(cp,ap);

	test_failure( AssertPtrEQ(ap,bp) );
	test_failure( AssertPtrNE(ap,cp) );
	test_failure( AssertPtrGT(ap,cp) );
	test_failure( AssertPtrGT(bp,cp) );
	test_failure( AssertPtrGE(bp,ap) );
	test_failure( AssertPtrLT(cp,ap) );
	test_failure( AssertPtrLT(cp,bp) );
	test_failure( AssertPtrLE(ap,bp) );
}


void test_assert_float()
{
	float a=0.0004, b=0.0003, c=0.0004;

	AssertFloatEQ(a,c);
	AssertFloatNE(a,b);
	AssertFloatGT(a,b);
	AssertFloatGE(a,b);
	AssertFloatGE(a,c);
	AssertFloatLT(b,a);
	AssertFloatLE(b,a);
	AssertFloatLE(c,a);

	test_failure( AssertFloatEQ(a,b) );
	test_failure( AssertFloatNE(a,c) );
	test_failure( AssertFloatGT(a,c) );
	test_failure( AssertFloatGT(b,c) );
	test_failure( AssertFloatGE(b,a) );
	test_failure( AssertFloatLT(c,a) );
	test_failure( AssertFloatLT(c,b) );
	test_failure( AssertFloatLE(a,b) );

	AssertDoubleEQ(a,c);
	AssertDoubleNE(a,b);
	AssertDoubleGT(a,b);
	AssertDoubleGE(a,b);
	AssertDoubleGE(a,c);
	AssertDoubleLT(b,a);
	AssertDoubleLE(b,a);
	AssertDoubleLE(c,a);

	test_failure( AssertDoubleEQ(a,b) );
	test_failure( AssertDoubleNE(a,c) );
	test_failure( AssertDoubleGT(a,c) );
	test_failure( AssertDoubleGT(b,c) );
	test_failure( AssertDoubleGE(b,a) );
	test_failure( AssertDoubleLT(c,a) );
	test_failure( AssertDoubleLT(c,b) );
	test_failure( AssertDoubleLE(a,b) );
}


void test_assert_strings()
{
	const char *a = "Bogozity";
	const char *b = "Arclamp";
	const char *c = "Bogozity";
	const char *e = "";
	const char *n = NULL;

	AssertStrEQ(a,c);
	AssertStrNE(a,b);
	AssertStrGT(a,b);
	AssertStrGE(a,b);
	AssertStrGE(a,c);
	AssertStrLT(b,a);
	AssertStrLE(b,a);
	AssertStrLE(c,a);

	test_failure( AssertStrEQ(a,b) );
	test_failure( AssertStrNE(a,c) );
	test_failure( AssertStrGT(a,c) );
	test_failure( AssertStrGT(b,c) );
	test_failure( AssertStrGE(b,a) );
	test_failure( AssertStrLT(c,a) );
	test_failure( AssertStrLT(c,b) );
	test_failure( AssertStrLE(a,b) );

	AssertStrEmpty(e);
	test_failure( AssertStrEmpty(a) );
	test_failure( AssertStrEmpty(n) );

	AssertStrNonEmpty(a);
	test_failure( AssertStrNonEmpty(e) );
	test_failure( AssertStrNonEmpty(n) );
}


void mutest_test_assert_flavor()
{
	mutest( test_assert_int() );
	mutest( test_assert_hex() );
	mutest( test_assert_ptr() );
	mutest( test_assert_float() );
	mutest( test_assert_strings() );
};
