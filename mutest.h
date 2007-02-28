/* mutest.h
 * Scott Bronson
 * 6 Mar 2006
 *
 * TODO: make tests self-documenting.  The test name is the same as the
 * function name, but they should also have a short and long description.
 * TODO: make mutest suites able to be arranged in a hierarchy.
 * 
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 * 
 * Version 0.7,  16 Feb 2007 -- turn dependency tree into functions
 * Version 0.62, 22 Jan 2007 -- clean up failure messages
 * Version 0.61, 30 Apr 2006 -- first version worth releasing
 */


/* @file mutest.h
 *
 * This file contains the declarations and all the Assert macros
 * required to use Zutest in your own applications.
 *
 * Zutest is a ground-up rewrite of Asim Jalis's "CuTest" library.
 *
 * To compile Zutest to run its own unit tests, do this:
 * 
 * <pre>
 * 	$ cc -DZUTEST_MAIN mutest.c -o mutest
 * 	$ ./mutest
 * 	4 tests run, 4 successes (132 assertions).
 * </pre>
 *
 * If your non-gcc compiler complains about a missing __func__ macro,
 * add -D__func__='"test"' to the compiler's command line.
 *
 * See ::mutest_tests for instructions on how to add mutest's
 * built-in unit tests to your application's test suite.
 */


#ifndef ZUTEST_H
#define ZUTEST_H

// This include is unfortunate...  TODO: try to get this out of here.
#include <setjmp.h>


//#define ZUTBECAUSE " failed because "
#define ZUTBECAUSE " failed. "

// Note that Fail doesn't increment mutest_assertions (the number of assertions
// that have been made) because it doesn't assert anything.  It only fails.
// If you call fail, you might want to increment mutest_assertions
// manually if you care about this number.  Normally you won't care.
#define Fail(...) mutest_fail(__FILE__, __LINE__, __func__, __VA_ARGS__)

// If the expression returns false, it is printed in the failure message.
#define Assert(x) do { mutest_assertions++; \
		if(!(x)) { Fail(#x); } } while(0)

// If the expression returns false, the given format string is printed.
// This is the same as Assert, just with much more helpful error messages.
// For instance: AssertFmt(isdigit(x), "isdigit but x=='%c'", x);
#define AssertFmt(x,...) do { mutest_assertions++; \
		if(!(x)) { Fail(__VA_ARGS__); } } while(0)

// integers, longs, chars...
#define AssertEq(x,y) AssertOp(x,==,y)
#define AssertNe(x,y) AssertOp(x,!=,y)
#define AssertGt(x,y) AssertOp(x,>,y)
#define AssertGe(x,y) AssertOp(x,>=,y)
#define AssertLt(x,y) AssertOp(x,<,y)
#define AssertLe(x,y) AssertOp(x,<=,y)

#define AssertZero(x) AssertOpToZero(x,==)
#define AssertNonzero(x) AssertOpToZero(x,!=)
#define AssertNonZero(x) AssertNonzero(x)
#define AssertPositive(x) AssertOpToZero(x,>);
#define AssertNegative(x) AssertOpToZero(x,<);
#define AssertNonNegative(x) AssertOpToZero(x,>=);
#define AssertNonPositive(x) AssertOpToZero(x,<=);

// Also integers but failure values are printed in hex rather than decimal.
#define AssertEqHex(x,y) AssertHexOp(x,==,y)
#define AssertNeHex(x,y) AssertHexOp(x,!=,y)
#define AssertGtHex(x,y) AssertHexOp(x,>,y)
#define AssertGeHex(x,y) AssertHexOp(x,>=,y)
#define AssertLtHex(x,y) AssertHexOp(x,<,y)
#define AssertLeHex(x,y) AssertHexOp(x,<=,y)

#define AssertZeroHex(x) AssertHexOpToZero(x,==)
#define AssertNonzeroHex(x) AssertHexOpToZero(x,!=)
#define AssertNonZeroHex(x) AssertNonzeroHex(x)
#define AssertPositiveHex(x) AssertHexOpToZero(x,>);
#define AssertNegativeHex(x) AssertHexOpToZero(x,<);
#define AssertNonNegativeHex(x) AssertHexOpToZero(x,>=);
#define AssertNonPositiveHex(x) AssertHexOpToZero(x,<=);

// Pointers...
#define AssertPtr(p)  AssertFmt(p != NULL, \
		#p" != NULL" ZUTBECAUSE #p"==0x%lX!", (unsigned long)p)
#define AssertNull(p) AssertFmt(p == NULL, \
		#p" == NULL" ZUTBECAUSE #p"==0x%lX!", (unsigned long)p)
#define AssertNonNull(p) AssertPtr(p)

#define AssertPtrNull(p) AssertNull(p)
#define AssertPtrNonNull(p) AssertNonNull(p)
#define AssertPtrEq(x,y) AssertPtrOp(x,==,y)
#define AssertPtrNe(x,y) AssertPtrOp(x,!=,y)
#define AssertPtrGt(x,y) AssertPtrOp(x,>,y)
#define AssertPtrGe(x,y) AssertPtrOp(x,>=,y)
#define AssertPtrLt(x,y) AssertPtrOp(x,<,y)
#define AssertPtrLe(x,y) AssertPtrOp(x,<=,y)

// These work with floats and doubles
// (everything is handled internally as double)
#define AssertFloatEq(x,y) AssertFloatOp(x,==,y)
#define AssertFloatNe(x,y) AssertFloatOp(x,!=,y)
#define AssertFloatGt(x,y) AssertFloatOp(x,>,y)
#define AssertFloatGe(x,y) AssertFloatOp(x,>=,y)
#define AssertFloatLt(x,y) AssertFloatOp(x,<,y)
#define AssertFloatLe(x,y) AssertFloatOp(x,<=,y)
// supply Doubles so people don't worry about precision when they see Float
#define AssertDoubleEq(x,y) AssertFloatOp(x,==,y)
#define AssertDoubleNe(x,y) AssertFloatOp(x,!=,y)
#define AssertDoubleGt(x,y) AssertFloatOp(x,>,y)
#define AssertDoubleGe(x,y) AssertFloatOp(x,>=,y)
#define AssertDoubleLt(x,y) AssertFloatOp(x,<,y)
#define AssertDoubleLe(x,y) AssertFloatOp(x,<=,y)

// Strings (uses strcmp)...
#define AssertStrEq(x,y) AssertStrOp(x,eq,==,y)
#define AssertStrNe(x,y) AssertStrOp(x,ne,!=,y)
#define AssertStrGt(x,y) AssertStrOp(x,gt,>,y)
#define AssertStrGe(x,y) AssertStrOp(x,ge,>=,y)
#define AssertStrLt(x,y) AssertStrOp(x,lt,<,y)
#define AssertStrLe(x,y) AssertStrOp(x,le,<=,y)

// ensures a string is non-null but zero-length
#define AssertStrEmpty(p) do { mutest_assertions++; \
		if(!(p)) { Fail(#p" is empty" ZUTBECAUSE #p " is NULL!"); } \
		if((p)[0]) { Fail(#p" is empty" ZUTBECAUSE #p " is: %s",p); } \
	} while(0)
// ensures a string is non-null and non-zero-length
#define AssertStrNonEmpty(p) do { mutest_assertions++; \
		if(!(p)) { Fail(#p" is nonempty" ZUTBECAUSE #p " is NULL!"); } \
		if(!(p)[0]) { Fail(#p" is nonempty" ZUTBECAUSE #p"[0] is 0!"); } \
	} while(0)

// I think that "Equal" looks better than "Eq".
// This is probably proof that these macros need to be totally overhauled...
#define AssertEqual(x,y) AssertEq(x,y)
#define AssertEqualHex(x,y) AssertHexEq(x,y)
#define AssertPtrEqual(x,y) AssertPtrEq(x,y)
#define AssertFloatEqual(x,y) AssertFloatEq(x,y)
#define AssertDoubleEqual(x,y) AssertFloatEq(x,y)
#define AssertStrEqual(x,y) AssertStrEq(x,y)

//
// helper macros, not intended to be called directly.
//

#define AssertExpType(x,op,y,type,fmt) \
	AssertFmt((type)x op (type)y, #x" "#op" "#y ZUTBECAUSE \
	#x"=="fmt" and "#y"=="fmt"!", (type)x,(type)y)
// The failure "x==0 failed because x==1 and 0==0" s too wordy so we'll
// special-case checking against 0: x==0 failed because x==1).
#define AssertExpToZero(x,op,type,fmt) \
	AssertFmt((type)x op 0,#x" "#op" 0" ZUTBECAUSE #x"=="fmt"!", (type)x)

#define AssertOp(x,op,y) AssertExpType(x,op,y,long,"%ld")
#define AssertHexOp(x,op,y) AssertExpType(x,op,y,long,"0x%lX")
#define AssertOpToZero(x,op) AssertExpToZero(x,op,long,"%ld")
#define AssertHexOpToZero(x,op) AssertExpToZero(x,op,long,"0x%lX")
#define AssertPtrOp(x,op,y) AssertExpType(x,op,y,unsigned long,"0x%lX")
#define AssertFloatOp(x,op,y) AssertExpType(x,op,y,double,"%lf")
#define AssertStrOp(x,opn,op,y) AssertFmt(strcmp(x,y) op 0, \
	#x" "#opn" "#y ZUTBECAUSE #x" is \"%s\" and "#y" is \"%s\"!",x,y)




/** Fails the current test.
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

void mutest_fail(const char *file, int line, const char *func,
		const char *msg, ...);
		
		
#define mutest(test) do { mutest_tests_run += 1; 	\
		if(!setjmp(mutest_test_bail)) { 			\
			do { test; } while(0); 					\
			mutest_successes += 1; 					\
		} else { 									\
			mutest_failures += 1; 					\
		} } while(0)
		

/* above this line is stuff only needed within the tests */
/* ------------------ */
/* below this line is stuff only needed to run the tests */




/** Keeps track of how many assertions have been made.
 * This needs to be updated manually each time an assertion
 * is made.  The Zutest built-in assertion macros all
 * update this variable properly.
 */

extern int mutest_assertions;
extern int mutest_tests_run;
extern int mutest_successes;
extern int mutest_failures;
extern jmp_buf mutest_test_bail;

typedef void (*mutest_proc)();

/** Runs all the tests in a suite. */
void run_mutest_suite(mutest_proc proc);
/** Runs all the tests in all the suites passed in. */
void run_mutest_suites(mutest_proc proc);

void print_mutest_results();


/** 
 *
 * Call this on the very first line of your application.  If the user
 * ran your program with the first arg of "--run-unit-tests", this will
 * run the tests and exit.  Otherwise your program will run as normal.
 * If you would rather create a dedicated executable, just call
 * run_mutest_suites() directly.
 */

void unit_test_check(int argc, char **argv, mutest_proc proc);

/**
 *
 * This runs all the unit tests supplied and then exits.  Use this
 * if you want to handle the arguments yourself.
 */

void run_unit_tests(mutest_proc proc);
void run_unit_tests_showing_failures(mutest_proc proc);


/** Zutest's built-in test suite.
 *
 * This allows you to add the Zutest unit test suite to your application's
 * test suites.  This way, you can ensure that Zutest's unit tests pass
 * before running your application's.  This is for the especially pedantic. :)
 *
 * Unfortunately, there is one test that cannot be run if you do this:
 * ensuring that mutest properly handles empty test suites.
 * Other than this one test, adding mutest_tests
 * to your application's test suite is equivalent to causing mutest to
 * compile and run its unit tests as described in mutest.h.
 */

void mutest_tests();


#endif
