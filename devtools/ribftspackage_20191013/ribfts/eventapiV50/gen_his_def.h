#include "typedefs.h"
/* gen_his_def.h
 * =============
 *
 * purpose       : General definitions for m_histogram
 * created       : 29-Nov-1994
 * author        : R.S. Mayer
 *
 * updates       :
 *
 */

#ifdef GSI__LYNX
#define HIS__VERSION "Histogram manager v2.05 5-Jul-2000"
#define __SYSERR_MAC(ERRNUM) sys_errlist[ERRNUM]
#define HIS__MAXSEGSIZ 0x400000
#else
#define HIS__VERSION "LEA v3.06 5-May-2003"
#define HIS__MAXSEGSIZ 0x4000000   /* max segment size for histogram          */
#endif

#define HIS__MAXHIS    1024       /* max number of histograms                */
#define HIS__IDBASE    1          /* Identifier for a histogram base         */
#define HIS__BASPERM   0774       /* RW Permission for User and Group        */
