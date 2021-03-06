/* ctassert.h
 * Scott Bronson
 * 6 Mar 2006
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://www.opensource.org/licenses/mit-license.php
 */


/* @file ctassert.h
 *
 * This file provides CamelCase Assert macros:
 * 
 *     AssertEqual(a,b);
 *     AssertGT(a,b);  AssumeGreaterThan(a,b);
 *     AssertStringEqual(a,b);
 *     etc.
 *
 * You can use either the abbreviated format, <i>AssertLE(x,y)</i>,
 * or the expanded format <i>AssertLessThanOrEqual(x,y)</i>.
 *
 * Here's an example of what failures look like.  They are very
 * informative, including variable names and their values.
 *
 * <pre>
 * test_assert.c:56: In test_assert_int, assert interval == base failed. interval==4 and base==3!
 * test_assert.c:57: In test_assert_int, assert a != c failed. a==4 and c==4!
 * etc.
 * </pre>
 * 
 * This was ctest's (actually zutest's) original assert format.
 *
 * TODO: is there a way to add a message?
 */


#ifndef CTEST_ASSERT_H
#define CTEST_ASSERT_H

#include "ctest.h"

/*
 *      Use these macros to test your app.
 */

/* integers, longs, chars */
#define AssertEQ(x,y) AssertOp(x,==,y)
#define AssertNE(x,y) AssertOp(x,!=,y)
#define AssertGT(x,y) AssertOp(x,>,y)
#define AssertGE(x,y) AssertOp(x,>=,y)
#define AssertLT(x,y) AssertOp(x,<,y)
#define AssertLE(x,y) AssertOp(x,<=,y)

#define AssertZero(x) AssertOpToZero(x,==)
#define AssertNonzero(x) AssertOpToZero(x,!=)
#define AssertNonZero(x) AssertNonzero(x)
#define AssertPositive(x) AssertOpToZero(x,>);
#define AssertNegative(x) AssertOpToZero(x,<);
#define AssertNonNegative(x) AssertOpToZero(x,>=);
#define AssertNonPositive(x) AssertOpToZero(x,<=);

/* Also integers but failure values are printed in hexadecimal rather than decimal. */
#define AssertHexEQ(x,y) AssertHexOp(x,==,y)
#define AssertHexNE(x,y) AssertHexOp(x,!=,y)
#define AssertHexGT(x,y) AssertHexOp(x,>,y)
#define AssertHexGE(x,y) AssertHexOp(x,>=,y)
#define AssertHexLT(x,y) AssertHexOp(x,<,y)
#define AssertHexLE(x,y) AssertHexOp(x,<=,y)

#define AssertHexZero(x) AssertHexOpToZero(x,==)
#define AssertHexNonzero(x) AssertHexOpToZero(x,!=)
#define AssertHexNonZero(x) AssertNonzeroHex(x)
#define AssertHexPositive(x) AssertHexOpToZero(x,>);
#define AssertHexNegative(x) AssertHexOpToZero(x,<);
#define AssertHexNonNegative(x) AssertHexOpToZero(x,>=);
#define AssertHexNonPositive(x) AssertHexOpToZero(x,<=);

/* Pointers */
#define AssertPtr(p)  do { void* pv = (p); \
	ctest_assert_fmt(pv != (void*)0, __FILE__, __LINE__, "%s != NULL with %s==0x%lX!", #p, #p, pv); \
	} while(0)
#define AssertNull(p) do { void *pv = (p); \
	ctest_assert_fmt(pv == (void*)0, __FILE__, __LINE__, "%s == NULL with %s==0x%lX!", #p, #p, pv); \
	} while(0)
#define AssertNonNull(p) AssertPtr(p)

#define AssertPointer(p) AssertPtr(p)
#define AssertPtrNull(p) AssertNull(p)
#define AssertPtrNonNull(p) AssertNonNull(p)
#define AssertPointerNull(p) AssertNull(p)
#define AssertPointerNonNull(p) AssertNonNull(p)

#define AssertPtrEQ(x,y) AssertPtrOp(x,==,y)
#define AssertPtrNE(x,y) AssertPtrOp(x,!=,y)
#define AssertPtrGT(x,y) AssertPtrOp(x,>,y)
#define AssertPtrGE(x,y) AssertPtrOp(x,>=,y)
#define AssertPtrLT(x,y) AssertPtrOp(x,<,y)
#define AssertPtrLE(x,y) AssertPtrOp(x,<=,y)

#define AssertPointerEQ(x,y) AssertPtrEQ(x,y)
#define AssertPointerNE(x,y) AssertPtrNE(x,y)
#define AssertPointerGT(x,y) AssertPtrGT(x,y)
#define AssertPointerGE(x,y) AssertPtrGE(x,y)
#define AssertPointerLT(x,y) AssertPtrLT(x,y)
#define AssertPointerLE(x,y) AssertPtrLE(x,y)

/* Floats and doubles */
/* (everything is handled internally as double) */
#define AssertFloatEQ(x,y) AssertFloatOp(x,==,y)
#define AssertFloatNE(x,y) AssertFloatOp(x,!=,y)
#define AssertFloatGT(x,y) AssertFloatOp(x,>,y)
#define AssertFloatGE(x,y) AssertFloatOp(x,>=,y)
#define AssertFloatLT(x,y) AssertFloatOp(x,<,y)
#define AssertFloatLE(x,y) AssertFloatOp(x,<=,y)
/* supply Doubles so people don't worry about precision when they see Float */
#define AssertDoubleEQ(x,y) AssertFloatOp(x,==,y)
#define AssertDoubleNE(x,y) AssertFloatOp(x,!=,y)
#define AssertDoubleGT(x,y) AssertFloatOp(x,>,y)
#define AssertDoubleGE(x,y) AssertFloatOp(x,>=,y)
#define AssertDoubleLT(x,y) AssertFloatOp(x,<,y)
#define AssertDoubleLE(x,y) AssertFloatOp(x,<=,y)

/* Strings (uses strcmp) */
#define AssertStrEQ(x,y) AssertStrOp(x,eq,==,y)
#define AssertStrNE(x,y) AssertStrOp(x,ne,!=,y)
#define AssertStrGT(x,y) AssertStrOp(x,gt,>,y)
#define AssertStrGE(x,y) AssertStrOp(x,ge,>=,y)
#define AssertStrLT(x,y) AssertStrOp(x,lt,<,y)
#define AssertStrLE(x,y) AssertStrOp(x,le,<=,y)

#define AssertStringEQ(x,y) AssertStrEQ(x,y)
#define AssertStringNE(x,y) AssertStrNE(x,y)
#define AssertStringGT(x,y) AssertStrGT(x,y)
#define AssertStringGE(x,y) AssertStrGE(x,y)
#define AssertStringLT(x,y) AssertStrLT(x,y)
#define AssertStringLE(x,y) AssertStrLE(x,y)

/* ensures a string is non-null but zero-length */
#define AssertStrEmpty(p) do { char *pv = (void*)(p); \
		if(!(pv)) { ctest_assert_fmt(0, __FILE__, __LINE__, #p" is empty with " #p " set to NULL"); } \
		else if((pv)[0]) { ctest_assert_fmt(0, __FILE__, __LINE__, "%s is empty with %s set to \"%s\"", #p, #p, pv); } \
		else ctest_assert_fmt(1, __FILE__, __LINE__, "%s is empty with %s[0]=0", #p, #p); \
	} while(0)

/* ensures a string is non-null and non-zero-length */
#define AssertStrNonEmpty(p) do { char *pv = (void*)(p); \
		if(!(pv)) { ctest_assert_fmt(0, __FILE__, __LINE__, "%s is nonempty with %s set to NULL", #p, #p); } \
		else if(!(pv)[0]) { ctest_assert_fmt(0, __FILE__, __LINE__, "%s is nonempty with %s[0] set to 0", #p, #p); } \
		else ctest_assert_fmt(1, __FILE__, __LINE__, "%s is empty with %s set to \"%s\"", #p, #p, pv); \
	} while(0)

#define AssertStringEmpty(x) AssertStrEmpty(x)
#define AssertStringNonEmpty(x) AssertStrNonEmpty(x)


/* Now let's spell some of those macros out... */

/* I think that "Equal" looks better than "EQ". */
#define AssertEqual(x,y) AssertEQ(x,y)
#define AssertHexEqual(x,y) AssertHexEQ(x,y)
#define AssertPtrEqual(x,y) AssertPtrEQ(x,y)
#define AssertPointerEqual(x,y) AssertPtrEQ(x,y)
#define AssertFloatEqual(x,y) AssertFloatEQ(x,y)
#define AssertDoubleEqual(x,y) AssertFloatEQ(x,y)
#define AssertStrEqual(x,y) AssertStrEQ(x,y)
#define AssertStringEqual(x,y) AssertStrEQ(x,y)

#define AssertNotEqual(x,y) AssertNE(x,y)
#define AssertHexNotEqual(x,y) AssertHexNE(x,y)
#define AssertPtrNotEqual(x,y) AssertPtrNE(x,y)
#define AssertPointerNotEqual(x,y) AssertPtrNE(x,y)
#define AssertFloatNotEqual(x,y) AssertFloatNE(x,y)
#define AssertDoubleNotEqual(x,y) AssertFloatNE(x,y)
#define AssertStrNotEqual(x,y) AssertStrNE(x,y)
#define AssertStringNotEqual(x,y) AssertStrNE(x,y)

/* And GreaterThan looks a little better than GT */
#define AssertGreaterThan(x,y) AssertGT(x,y)
#define AssertHexGreaterThan(x,y) AssertHexGT(x,y)
#define AssertPtrGreaterThan(x,y) AssertPtrGT(x,y)
#define AssertPointerGreaterThan(x,y) AssertPtrGT(x,y)
#define AssertFloatGreaterThan(x,y) AssertFloatGT(x,y)
#define AssertDoubleGreaterThan(x,y) AssertFloatGT(x,y)
#define AssertStrGreaterThan(x,y) AssertStrGT(x,y)
#define AssertStringGreaterThan(x,y) AssertStrGT(x,y)
#define AssertLessThan(x,y) AssertLT(x,y)
#define AssertHexLessThan(x,y) AssertHexLT(x,y)
#define AssertPtrLessThan(x,y) AssertPtrLT(x,y)
#define AssertPointerLessThan(x,y) AssertPtrLT(x,y)
#define AssertFloatLessThan(x,y) AssertFloatLT(x,y)
#define AssertDoubleLessThan(x,y) AssertFloatLT(x,y)
#define AssertStrLessThan(x,y) AssertStrLT(x,y)
#define AssertStringLessThan(x,y) AssertStrLT(x,y)

/* GreaterThanOrEqualTo starts getting silly but we'll provide it */
#define AssertGreaterThanOrEqualTo(x,y) AssertGE(x,y)
#define AssertHexGreaterThanOrEqualTo(x,y) AssertHexGE(x,y)
#define AssertPtrGreaterThanOrEqualTo(x,y) AssertPtrGE(x,y)
#define AssertPointerGreaterThanOrEqualTo(x,y) AssertPtrGE(x,y)
#define AssertFloatGreaterThanOrEqualTo(x,y) AssertFloatGE(x,y)
#define AssertDoubleGreaterThanOrEqualTo(x,y) AssertFloatGE(x,y)
#define AssertStrGreaterThanOrEqualTo(x,y) AssertStrGE(x,y)
#define AssertStringGreaterThanOrEqualTo(x,y) AssertStrGE(x,y)
#define AssertLessThanOrEqualTo(x,y) AssertLE(x,y)
#define AssertHexLessThanOrEqualTo(x,y) AssertHexLE(x,y)
#define AssertPtrLessThanOrEqualTo(x,y) AssertPtrLE(x,y)
#define AssertPointerLessThanOrEqualTo(x,y) AssertPtrLE(x,y)
#define AssertFloatLessThanOrEqualTo(x,y) AssertFloatLE(x,y)
#define AssertDoubleLessThanOrEqualTo(x,y) AssertFloatLE(x,y)
#define AssertStrLessThanOrEqualTo(x,y) AssertStrLE(x,y)
#define AssertStringLessThanOrEqualTo(x,y) AssertStrLE(x,y)

/* Some people don't like the "To" on the end.  No problem. */
#define AssertGreaterThanOrEqual(x,y) AssertGE(x,y)
#define AssertHexGreaterThanOrEqual(x,y) AssertHexGE(x,y)
#define AssertPtrGreaterThanOrEqual(x,y) AssertPtrGE(x,y)
#define AssertPointerGreaterThanOrEqual(x,y) AssertPtrGE(x,y)
#define AssertFloatGreaterThanOrEqual(x,y) AssertFloatGE(x,y)
#define AssertDoubleGreaterThanOrEqual(x,y) AssertFloatGE(x,y)
#define AssertStrGreaterThanOrEqual(x,y) AssertStrGE(x,y)
#define AssertStringGreaterThanOrEqual(x,y) AssertStrGE(x,y)
#define AssertLessThanOrEqual(x,y) AssertLE(x,y)
#define AssertHexLessThanOrEqual(x,y) AssertHexLE(x,y)
#define AssertPtrLessThanOrEqual(x,y) AssertPtrLE(x,y)
#define AssertPointerLessThanOrEqual(x,y) AssertPtrLE(x,y)
#define AssertFloatLessThanOrEqual(x,y) AssertFloatLE(x,y)
#define AssertDoubleLessThanOrEqual(x,y) AssertFloatLE(x,y)
#define AssertStrLessThanOrEqual(x,y) AssertStrLE(x,y)
#define AssertStringLessThanOrEqual(x,y) AssertStrLE(x,y)


/*
 * helper macros, not intended to be called directly.
 */

/* If the expression returns false, it is printed in the failure message. */
#define Assert(x) \
	ctest_assert((x), __FILE__, __LINE__, #x)

#define AssertExpType(x,op,y,type,fmt) do { type xv = (x); type yv = (y); \
	ctest_assert_fmt(xv op yv, __FILE__, __LINE__, "%s %s %s with %s="fmt" and %s="fmt, #x, #op, #y, #x, xv, #y, yv); \
	} while(0)
/* The failure "x==0 failed because x==1 and 0==0" s too wordy so we'll */
/* special-case checking against 0: "x==0 failed because x==1" */
#define AssertExpToZero(x,op,type,fmt) do { type xv = (type)(x); \
	ctest_assert_fmt(xv op 0, __FILE__, __LINE__, "%s %s 0 with %s="fmt, #x, #op, #x, xv); \
	} while(0)


/* define CTEST_LONG_LONG_ASSERTS before including ctassert.h to have all
 * integer comparisons done using long longs.  Define this if you ever
 * need to assert on 64 bit values on 32 bit systems.  It incurs a
 * small performance penalty and is hardly ever needed. */

#ifdef CTEST_LONG_LONG_ASSERTS
#define CTiLONG long long
#define CTiFMT "%ll"
#else
#define CTiLONG long
#define CTiFMT "%l"
#endif

#define AssertOp(x,op,y) AssertExpType(x,op,y,CTiLONG,CTiFMT"d")
#define AssertHexOp(x,op,y) AssertExpType(x,op,y,CTiLONG,"0x"CTiFMT"X")
#define AssertOpToZero(x,op) AssertExpToZero(x,op,CTiLONG,CTiFMT"d")
#define AssertHexOpToZero(x,op) AssertExpToZero(x,op,CTiLONG,"0x"CTiFMT"X")
#define AssertPtrOp(x,op,y) AssertExpType(x,op,y,void*,"0x%lX")		/* can't use %p because some libc print "0x" first and some don't */
#define AssertFloatOp(x,op,y) AssertExpType(x,op,y,double,"%lf")
#define AssertStrOp(x,opn,op,y) do { char *xv = (void*)(x); char *yv = (void*)(y); \
	ctest_assert_fmt(strcmp(xv,yv) op 0, __FILE__, __LINE__, "%s %s %s with %s=\"%s\" and %s=\"%s\"", #x, #opn, #y, #x, xv, #y, yv); \
	} while(0)


/* If you want to run the unit tests for these asserts before using
 * them in your own test deck, call this function.  That should reduces
 * any chance an upgrade or compiler issues will silently break them.
 */
extern void run_ctassert_tests();


#endif

