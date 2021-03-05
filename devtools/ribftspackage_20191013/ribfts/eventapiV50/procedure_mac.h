#include "typedefs.h"
/* +++++++++++++++++++++++
 * +++ procedure_mac.h +++
 * +++++++++++++++++++++++
 * RSMayer
 * 07-Feb-1994
 * +++++++++++++++++++++++
 * define a C-Function
 * example:
 *   PROCEDURE(int,f_test,(l_a,i_b,c_c),long l_a; short i_b; char *c_c;)
 *   C-code
 *   }
 */

#define PROCEDURE(RETTYP,F_NAME,VARS,DCL) \
RETTYP F_NAME VARS \
DCL \
{ \
static CHARS c_modnam[] = #F_NAME     /* ANSI style, Lynx, OpenVMS */
/* static char c_modnam[] = "F_NAME"    VMS style  */     
