#include "typedefs.h"
/* +++++++++++++++++++++++
 * +++ d_error_mac.h +++
 * +++++++++++++++++++++++
 * RSMayer
 * 10-Feb-1994
 * +++++++++++++++++++++++
 * define Error macro (see also error_mac.h)
 * call only if DEBUG == 1
 * 
 * update: 03-Nov-1994 Define DEBUG /RSM
 */
#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef F_UT_ERROR__
#include "f_ut_error.h"
#endif
 
#define D_F_ERROR(ERRNO,C_RETSTR,C_STR,MASK) \
if (DEBUG == 1) \
f_ut_error(c_modnam,ERRNO,C_RETSTR,C_STR,(MASK) | MASK__DEBUG)
