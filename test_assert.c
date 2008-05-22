/* test_assert.c
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Tests the AssertEqual(a,b) style tests from ctest_Assert.h
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

#include <string.h>

#include "ct_assert.h"


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
	
	ctest_start_inverted(0) {
		AssertEQ(a,b);
		AssertNE(a,c);
		AssertGT(a,c);
		AssertGT(b,c);
		AssertGE(b,a);
		AssertLT(c,a);
		AssertLT(c,b);
		AssertLE(a,b);
	}

	AssertZero(z);
	AssertNonzero(a);
	ctest_start_inverted(0) {
		AssertZero(a);
		AssertNonzero(z);
	}

	AssertPositive(a);
	ctest_start_inverted(0) {
		AssertPositive(z)
		AssertPositive(n)
	}

	AssertNonPositive(n);
	AssertNonPositive(z);
	ctest_start_inverted(0) {
		AssertNonPositive(a)
	}

	AssertNegative(n);
	ctest_start_inverted(0) {
		AssertNegative(z)
		AssertNegative(a)
	}

	AssertNonNegative(a);
	AssertNonNegative(z);
	ctest_start_inverted(0) {
		AssertNonNegative(n)
	}
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

	ctest_start_inverted(0) {
		AssertHexEQ(a,b);
		AssertHexNE(a,c);
		AssertHexGT(a,c);
		AssertHexGT(b,c);
		AssertHexGE(b,a);
		AssertHexLT(c,a);
		AssertHexLT(c,b);
		AssertHexLE(a,b);
	}

	AssertHexZero(z);
	AssertHexNonzero(a);
	ctest_start_inverted(0) {
		AssertHexZero(a);
		AssertHexNonzero(z);
	}

	AssertHexPositive(a);
	ctest_start_inverted(0) {
		AssertHexPositive(z);
		AssertHexPositive(n);
	}

	AssertHexNonPositive(n);
	AssertHexNonPositive(z);
	ctest_start_inverted(0) {
		AssertHexNonPositive(a);
	}

	AssertHexNegative(n);
	ctest_start_inverted(0) {
		AssertHexNegative(z);
		AssertHexNegative(a);
	}	

	AssertHexNonNegative(a);
	AssertHexNonNegative(z);
	ctest_start_inverted(0) {
		AssertHexNonNegative(n);
	}
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

	ctest_start_inverted(0) {
		AssertPtr(n);
		AssertNull(ap);
	}

	AssertPtrEQ(ap,cp);
	AssertPtrNE(ap,bp);
	AssertPtrGT(ap,bp);
	AssertPtrGE(ap,bp);
	AssertPtrGE(ap,cp);
	AssertPtrLT(bp,ap);
	AssertPtrLE(bp,ap);
	AssertPtrLE(cp,ap);

	ctest_start_inverted(0) {
		AssertPtrEQ(ap,bp);
		AssertPtrNE(ap,cp);
		AssertPtrGT(ap,cp);
		AssertPtrGT(bp,cp);
		AssertPtrGE(bp,ap);
		AssertPtrLT(cp,ap);
		AssertPtrLT(cp,bp);
		AssertPtrLE(ap,bp);
	}
}


void test_assert_float()
{
	float a=0.0004f, b=0.0003f, c=0.0004f;

	AssertFloatEQ(a,c);
	AssertFloatNE(a,b);
	AssertFloatGT(a,b);
	AssertFloatGE(a,b);
	AssertFloatGE(a,c);
	AssertFloatLT(b,a);
	AssertFloatLE(b,a);
	AssertFloatLE(c,a);

	ctest_start_inverted(0) {
		AssertFloatEQ(a,b);
		AssertFloatNE(a,c);
		AssertFloatGT(a,c);
		AssertFloatGT(b,c);
		AssertFloatGE(b,a);
		AssertFloatLT(c,a);
		AssertFloatLT(c,b);
		AssertFloatLE(a,b);
	}
	
	AssertDoubleEQ(a,c);
	AssertDoubleNE(a,b);
	AssertDoubleGT(a,b);
	AssertDoubleGE(a,b);
	AssertDoubleGE(a,c);
	AssertDoubleLT(b,a);
	AssertDoubleLE(b,a);
	AssertDoubleLE(c,a);

	ctest_start_inverted(0) {
		AssertDoubleEQ(a,b);
		AssertDoubleNE(a,c);
		AssertDoubleGT(a,c);
		AssertDoubleGT(b,c);
		AssertDoubleGE(b,a);
		AssertDoubleLT(c,a);
		AssertDoubleLT(c,b);
		AssertDoubleLE(a,b);
	}
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

	ctest_start_inverted(0) {
		AssertStrEQ(a,b);
		AssertStrNE(a,c);
		AssertStrGT(a,c);
		AssertStrGT(b,c);
		AssertStrGE(b,a);
		AssertStrLT(c,a);
		AssertStrLT(c,b);
		AssertStrLE(a,b);
	}

	AssertStrEmpty(e);
	ctest_start_inverted(0) {
		AssertStrEmpty(a);
		AssertStrEmpty(n);
	}

	AssertStrNonEmpty(a);
	ctest_start_inverted(0) {
		AssertStrNonEmpty(e);
		AssertStrNonEmpty(n);
	}
}


void ctest_test_asserts()
{
	ctest_start("AssertInt") {
		test_assert_int();
	}
	
	ctest_start("AssertHex") {
		test_assert_hex();
	}
	
	ctest_start("AssertPtr") {
		test_assert_ptr();
	}
	
	ctest_start("AssertFloat") {
		test_assert_float();
	}
	
	ctest_start("AssertStr") {
		test_assert_strings();
	}
};
