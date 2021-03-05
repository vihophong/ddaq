#include "typedefs.h"
/* +++ itoshms_mac.h +++
 *
 * copy int seconds to a string HH:MM:SS
 *
 * R.S. Mayer
 * 22-Feb-1994
 */
#define ITOSHMS(STRING,SEC) \
{ \
INTU4 i_h, i_m, i_s; \
i_h = SEC / 3600; \
i_s = SEC - i_h * 3600; \
i_m = i_s / 60; \
i_s -= i_m * 60;\
sprintf(STRING,"%4d:%2d:%2d", i_h, i_m, i_s); \
}
