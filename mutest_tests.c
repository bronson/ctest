/* mutest_tests.c
 * Scott Bronson
 * 28 Feb 2007
 * 
 * This file is released under the MIT License.
 * See http://en.wikipedia.org/wiki/MIT_License for more.
 *
 * Specifies all the tests that should be run by the standalone mutest executable.
 * Of course, this will only ever be a single test: mutest_tests.  Don't expect
 * this file to change much...
 */

#include "mutest.h"


void all_tests()
{
	mutest_test_assert_flavor();
}
