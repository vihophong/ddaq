#include "typedefs.h"
/* +++++++++++++++++++++++
 * +++ d_test_mac.h +++
 * +++++++++++++++++++++++
 * RSMayer
 * 07-Feb-1994
 * +++++++++++++++++++++++
 * define test macro 
 * modif: 07-Mar-1994: RSM Show the output format
 */

/* #define TEST(A,F) printf("%s l:%d " #A ":" #F "\n", c_modnam,__LINE__,A)
 * ANSI
 */
#define D_TEST(A,F) \
if (DEBUG == 1) printf("%s l:%d " #A "(%" #F "):" #F "\n",c_modnam,__LINE__,A)
