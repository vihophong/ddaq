#include "typedefs.h"
/* +++++++++++++++++++++++
 * +++ error_mac.h +++
 * +++++++++++++++++++++++
 * RSMayer
 * 07-Feb-1994
 * +++++++++++++++++++++++
 * define Error macro 
 */

#ifndef F_UT_ERROR__
#include "f_ut_error.h"
#endif
 
#define F_ERROR(ERRNO,C_RETSTR,C_STR,MASK) \
f_ut_error(c_modnam,ERRNO,C_RETSTR,C_STR,MASK)
