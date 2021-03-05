#include "typedefs.h"
/* +++ s_esclnt_swap.h +++
 * client structure for Esone Server 
 * R.S. Mayer
 * 06-Jul-1994
*/
struct s_esclnt
      {
      struct s_esclnt        *ps_esclnt_hd[2];/* ptr to clnt struc fwrd/bck */
      /* +++++++++++++++++ connect stuff +++++++++++++++++++++++++++++++++ */
      INTS4                    l_channel;      /* connect channel            */
      /* ----------------------------------------------------------------- */
      INTS4                   l_esclnt_no;    /* esclnt number              */
      INTU4         lf_clnt_mrk ;   /* flag esclnt marked for del */
      INTS4                    l_sts_esclnt;   /* status of esone client     */
      INTS4                    l_sts_esbuf;    /* status of buffer and transf*/ 
      struct s_esclntbuf     *ps_esclntbuf;  /* ptr to curr output buf     */
      CHARS                   *ph_current;    /* ptr to 1st free byte       */
      /* ----------------------------------------------------------------- */
      INTU4          lf_byteswap;    /* byte swap enabled/disabl   */ 
      /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* ----- length is ESS__INFO_ESCLNT -------------------------------- */
      INTU4          l_esclntbuf_rdby;  /* read bytes              */
      INTU4          l_esclntbuf_sdby;  /* sent bytes              */
      INTU4          l_esclntbuf_rdbf;  /* read buffer             */
      INTU4          l_esclntbuf_sdbf;  /* sent buffer             */
      INTS4                   l_maxbuflen;       /* len of 
                                                 * s_esclntbuf.c_buffer[]  
                                                 * default: ESS__BUFSIZE
                                                 */
      /* ----------------------------------------------------------------- */
      };
