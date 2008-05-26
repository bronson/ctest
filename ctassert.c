/* test_assert.c
 * Scott Bronson
 * 6 Mar 2006
 * 
 * Tests the AssertEqual(a,b) style tests from ctest_Assert.h
 *
 * This file is 100% ct_assert unit tests.  Don't include it in your
 * app unless you want to call the unit tests yourself.
 * 
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */

#include "ctassert.h"
#include <string.h>


/** just a stupid little trick to potentially make the test more readable. */
#define ctest_invert while((ctest_preferences.inverted = !ctest_preferences.inverted) != 0)


/* used to ensure assert macros don't evaluate their arguments more than once */
int ctest_multi_calls;


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
	
	ctest_invert {
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
	ctest_invert {
		AssertZero(a);
		AssertNonzero(z);
	}

	AssertPositive(a);
	ctest_invert {
		AssertPositive(z)
		AssertPositive(n)
	}

	AssertNonPositive(n);
	AssertNonPositive(z);
	ctest_invert {
		AssertNonPositive(a)
	}

	AssertNegative(n);
	ctest_invert {
		AssertNegative(z)
		AssertNegative(a)
	}

	AssertNonNegative(a);
	AssertNonNegative(z);
	ctest_invert {
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

	ctest_invert {
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
	ctest_invert {
		AssertHexZero(a);
		AssertHexNonzero(z);
	}

	AssertHexPositive(a);
	ctest_invert {
		AssertHexPositive(z);
		AssertHexPositive(n);
	}

	AssertHexNonPositive(n);
	AssertHexNonPositive(z);
	ctest_invert {
		AssertHexNonPositive(a);
	}

	AssertHexNegative(n);
	ctest_invert {
		AssertHexNegative(z);
		AssertHexNegative(a);
	}	

	AssertHexNonNegative(a);
	AssertHexNonNegative(z);
	ctest_invert {
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

	ctest_invert {
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

	ctest_invert {
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

	ctest_invert {
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

	ctest_invert {
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

	ctest_invert {
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
	ctest_invert {
		AssertStrEmpty(a);
		AssertStrEmpty(n);
	}

	AssertStrNonEmpty(a);
	ctest_invert {
		AssertStrNonEmpty(e);
		AssertStrNonEmpty(n);
	}
}


static void* multi_ptr()
{
	ctest_multi_calls += 1;
	return ctest_multi_calls == 1 ? &ctest_multi_calls : NULL;
}


static void* multi_null()
{
	ctest_multi_calls += 1;
	return ctest_multi_calls == 1 ? NULL : &ctest_multi_calls;
}


static const char *multi_str()
{
	ctest_multi_calls += 1;
	return ctest_multi_calls == 1 ? "yep" : "";
}


static const char* multi_str_empty()
{
	ctest_multi_calls += 1;
	return ctest_multi_calls == 1 ? "" : "nope";
}


/* This test makes sure that assert macros only evaluate their arguments once. */
void test_assert_args()
{
	int i;

	/* This takes care of everything handled by AssertExpType */
	i=1;
	AssertEQ(i++, 1);
	i=1;
	AssertEQ(++i, 2);

	ctest_multi_calls = 0;
	AssertPtr(multi_ptr());
	AssertEQ(ctest_multi_calls, 1);

	ctest_multi_calls = 0;
	AssertNull(multi_null());
	AssertEQ(ctest_multi_calls, 1);

	ctest_multi_calls = 0;
	AssertStrEQ(multi_str(), "yep");
	AssertEQ(ctest_multi_calls, 1);

	ctest_multi_calls = 0;
	AssertStrEmpty(multi_str_empty());
	AssertEQ(ctest_multi_calls, 1);

	ctest_multi_calls = 0;
	AssertStrNonEmpty(multi_str());
	AssertEQ(ctest_multi_calls, 1);
}


static int nested_assert()
{
	AssertEqual(12, 12);
	return 42;
}


void run_ctassert_tests()
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

	ctest_start("AssertArgs") {
		test_assert_args();
	}

	ctest_start("AssertNesting") {
		/* This makes sure that we can assert on a function result,
		 * even if that function makes other assert calls.
		 * (this is why ctest used to support nested asserts) */
		AssertEqual(nested_assert(), nested_assert());
	}
}

