/* ct_assert.h
 * Scott Bronson
 * 6 Mar 2006
 *
 * Copyright (C) 2007 Scott Bronson
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 */


/* @file ct_assert.h
 *
 * This is a ctest flavor file.
 *
 * It provides CamelCase Assert macros:
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

#include "ctest_test.h"

#if WIN32
// Microsoft's compiler doesn't support __func__.
// TODO: is there a better way to test for non-existence of __func__ than "WIN32"?
#define __func__ "test"
#endif

//
//      Macros to test your app:
//

// integers, longs, chars...
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

// Also integers but failure values are printed in hexadecimal rather than decimal.
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

// Pointers...
#define AssertPtr(p)  AssertFmt(p != (void*)0, \
		#p" != NULL" CTBECAUSE #p" is NULL!")
#define AssertNull(p) AssertFmt(p == (void*)0, \
		#p" == NULL" CTBECAUSE #p"==0x%lX!", p)
#define AssertNonNull(p) AssertPtr(p)

#define AssertPtrNull(p) AssertNull(p)
#define AssertPtrNonNull(p) AssertNonNull(p)
#define AssertPtrEQ(x,y) AssertPtrOp(x,==,y)
#define AssertPtrNE(x,y) AssertPtrOp(x,!=,y)
#define AssertPtrGT(x,y) AssertPtrOp(x,>,y)
#define AssertPtrGE(x,y) AssertPtrOp(x,>=,y)
#define AssertPtrLT(x,y) AssertPtrOp(x,<,y)
#define AssertPtrLE(x,y) AssertPtrOp(x,<=,y)

// These work with floats and doubles
// (everything is handled internally as double)
#define AssertFloatEQ(x,y) AssertFloatOp(x,==,y)
#define AssertFloatNE(x,y) AssertFloatOp(x,!=,y)
#define AssertFloatGT(x,y) AssertFloatOp(x,>,y)
#define AssertFloatGE(x,y) AssertFloatOp(x,>=,y)
#define AssertFloatLT(x,y) AssertFloatOp(x,<,y)
#define AssertFloatLE(x,y) AssertFloatOp(x,<=,y)
// supply Doubles so people don't worry about precision when they see Float
#define AssertDoubleEQ(x,y) AssertFloatOp(x,==,y)
#define AssertDoubleNE(x,y) AssertFloatOp(x,!=,y)
#define AssertDoubleGT(x,y) AssertFloatOp(x,>,y)
#define AssertDoubleGE(x,y) AssertFloatOp(x,>=,y)
#define AssertDoubleLT(x,y) AssertFloatOp(x,<,y)
#define AssertDoubleLE(x,y) AssertFloatOp(x,<=,y)

// Strings (uses strcmp)...
#define AssertStrEQ(x,y) AssertStrOp(x,eq,==,y)
#define AssertStrNE(x,y) AssertStrOp(x,ne,!=,y)
#define AssertStrGT(x,y) AssertStrOp(x,gt,>,y)
#define AssertStrGE(x,y) AssertStrOp(x,ge,>=,y)
#define AssertStrLT(x,y) AssertStrOp(x,lt,<,y)
#define AssertStrLE(x,y) AssertStrOp(x,le,<=,y)

// ensures a string is non-null but zero-length
#define AssertStrEmpty(p) do { \
		ctest_assert_prepare(__FILE__, __LINE__, __func__, #p " is empty"); \
		if(!(p)) { ctest_assert_failed(#p" is empty" CTBECAUSE #p " is NULL!"); } \
		else if((p)[0]) { ctest_assert_failed(#p" is empty" CTBECAUSE #p " is: %s",p); } \
		else ctest_assert_succeeded(); \
	} while(0)

// ensures a string is non-null and non-zero-length
#define AssertStrNonEmpty(p) do { \
		ctest_assert_prepare(__FILE__, __LINE__, __func__, #p " is non-empty"); \
		if(!(p)) { ctest_assert_failed(#p" is nonempty" CTBECAUSE #p " is NULL!"); } \
		else if(!(p)[0]) { ctest_assert_failed(#p" is nonempty" CTBECAUSE #p"[0] is 0!"); } \
		else ctest_assert_succeeded(); \
	} while(0)



// Now let's spell some of those macros out...

// I think that "Equal" looks better than "EQ".
#define AssertEqual(x,y) AssertEQ(x,y)
#define AssertHexEqual(x,y) AssertHexEQ(x,y)
#define AssertPtrEqual(x,y) AssertPtrEQ(x,y)
#define AssertFloatEqual(x,y) AssertFloatEQ(x,y)
#define AssertDoubleEqual(x,y) AssertFloatEQ(x,y)
#define AssertStrEqual(x,y) AssertStrEQ(x,y)

#define AssertNotEqual(x,y) AssertNE(x,y)
#define AssertHexNotEqual(x,y) AssertHexNE(x,y)
#define AssertPtrNotEqual(x,y) AssertPtrNE(x,y)
#define AssertFloatNotEqual(x,y) AssertFloatNE(x,y)
#define AssertDoubleNotEqual(x,y) AssertFloatNE(x,y)
#define AssertStrNotEqual(x,y) AssertStrNE(x,y)

// And GreaterThan looks a little better than GT
#define AssertGreaterThan(x,y) AssertGT(x,y)
#define AssertHexGreaterThan(x,y) AssertHexGT(x,y)
#define AssertPtrGreaterThan(x,y) AssertPtrGT(x,y)
#define AssertFloatGreaterThan(x,y) AssertFloatGT(x,y)
#define AssertDoubleGreaterThan(x,y) AssertFloatGT(x,y)
#define AssertStrGreaterThan(x,y) AssertStrGT(x,y)
#define AssertLessThan(x,y) AssertLT(x,y)
#define AssertHexLessThan(x,y) AssertHexLT(x,y)
#define AssertPtrLessThan(x,y) AssertPtrLT(x,y)
#define AssertFloatLessThan(x,y) AssertFloatLT(x,y)
#define AssertDoubleLessThan(x,y) AssertFloatLT(x,y)
#define AssertStrLessThan(x,y) AssertStrLT(x,y)

// GreaterThanOrEqualTo starts getting silly but we'll provide it
#define AssertGreaterThanOrEqualTo(x,y) AssertGE(x,y)
#define AssertHexGreaterThanOrEqualTo(x,y) AssertHexGE(x,y)
#define AssertPtrGreaterThanOrEqualTo(x,y) AssertPtrGE(x,y)
#define AssertFloatGreaterThanOrEqualTo(x,y) AssertFloatGE(x,y)
#define AssertDoubleGreaterThanOrEqualTo(x,y) AssertFloatGE(x,y)
#define AssertStrGreaterThanOrEqualTo(x,y) AssertStrGE(x,y)
#define AssertLessThanOrEqualTo(x,y) AssertLE(x,y)
#define AssertHexLessThanOrEqualTo(x,y) AssertHexLE(x,y)
#define AssertPtrLessThanOrEqualTo(x,y) AssertPtrLE(x,y)
#define AssertFloatLessThanOrEqualTo(x,y) AssertFloatLE(x,y)
#define AssertDoubleLessThanOrEqualTo(x,y) AssertFloatLE(x,y)
#define AssertStrLessThanOrEqualTo(x,y) AssertStrLE(x,y)

// Some people don't like the "To" on the end.  No problem.
#define AssertGreaterThanOrEqual(x,y) AssertGE(x,y)
#define AssertHexGreaterThanOrEqual(x,y) AssertHexGE(x,y)
#define AssertPtrGreaterThanOrEqual(x,y) AssertPtrGE(x,y)
#define AssertFloatGreaterThanOrEqual(x,y) AssertFloatGE(x,y)
#define AssertDoubleGreaterThanOrEqual(x,y) AssertFloatGE(x,y)
#define AssertStrGreaterThanOrEqual(x,y) AssertStrGE(x,y)
#define AssertLessThanOrEqual(x,y) AssertLE(x,y)
#define AssertHexLessThanOrEqual(x,y) AssertHexLE(x,y)
#define AssertPtrLessThanOrEqual(x,y) AssertPtrLE(x,y)
#define AssertFloatLessThanOrEqual(x,y) AssertFloatLE(x,y)
#define AssertDoubleLessThanOrEqual(x,y) AssertFloatLE(x,y)
#define AssertStrLessThanOrEqual(x,y) AssertStrLE(x,y)


//
// helper macros, not intended to be called directly.
//

#ifndef CTBECAUSE
//#define CTBECAUSE " failed because "
#define CTBECAUSE " failed. "
#endif


// If the expression returns false, it is printed in the failure message.
#define Assert(x) do { \
		ctest_assert_prepare(__FILE__, __LINE__, __func__, #x); \
		if(x) { ctest_assert_succeeded(); } else { ctest_assert_failed(#x); } } while(0)

// If the expression returns false, the given format string is printed.
// This is the same as Assert, just with much more helpful error messages.
// For instance: AssertFmt(isdigit(x), "isdigit but x=='%c'", x);
#define AssertFmt(x,...) do { \
		ctest_assert_prepare(__FILE__, __LINE__, __func__, #x); \
		if(x) { ctest_assert_succeeded(); } else { ctest_assert_failed(__VA_ARGS__); } } while(0)


#define AssertExpType(x,op,y,type,fmt) \
	AssertFmt((type)(x) op (type)(y), #x" "#op" "#y CTBECAUSE \
	#x"=="fmt" and "#y"=="fmt"!", (type)(x),(type)(y))
// The failure "x==0 failed because x==1 and 0==0" s too wordy so we'll
// special-case checking against 0: "x==0 failed because x==1"
#define AssertExpToZero(x,op,type,fmt) \
	AssertFmt((type)(x) op 0,#x" "#op" 0" CTBECAUSE #x"=="fmt"!", (type)(x))


#define AssertOp(x,op,y) AssertExpType(x,op,y,long,"%ld")
#define AssertHexOp(x,op,y) AssertExpType(x,op,y,long,"0x%lX")
#define AssertOpToZero(x,op) AssertExpToZero(x,op,long,"%ld")
#define AssertHexOpToZero(x,op) AssertExpToZero(x,op,long,"0x%lX")
#define AssertPtrOp(x,op,y) AssertExpType(x,op,y,void*,"0x%lX")		// can't use %p because some libc print "0x" first and some don't
#define AssertFloatOp(x,op,y) AssertExpType(x,op,y,double,"%lf")
#define AssertStrOp(x,opn,op,y) AssertFmt(strcmp(x,y) op 0, \
	#x" "#opn" "#y CTBECAUSE #x" is \"%s\" and "#y" is \"%s\"!",x,y)


// If you want your app to run the unit tests for this test flavor,
// add the following test to your test deck.
extern void ctest_test_assert_flavor();


#endif
