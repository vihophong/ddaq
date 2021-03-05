#include "typedefs.h"
/* +++ s_esclntbuf.h +++
 * input/output buffer structure for Esone Server 
 * R.S. Mayer
 * 06-Jul-1994
 */

struct s_esclntbuf
    {
    /* ----- length is ESS__ESOBUF_HEAD ----------------------------------- */
    struct s_esobuf s_esobuf_1;              /* Esone buffer header         */
    /* ----- length is ESS__BUFSIZE --------------------------------------- */
    CHARS            c_buffer[ESS__BUFSIZE];  /* Esone data                  */
    };
