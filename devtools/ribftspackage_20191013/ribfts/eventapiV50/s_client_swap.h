#include "typedefs.h"
/* +++ s_client_swap.h +++
 * client structure for SBS monitor 
 * (differs from s_client.h for GPS-Server on (Open)VMS )
 * ---> SWAPPED VERSION <---- for big endian
 * R.S. Mayer
 * 18-Feb-1994
*/
struct s_client 
      {
      struct s_client        *ps_client_hd[2];/* ptr to clnt struc fwrd/bck */
      /* +++++++++++++++++ connect stuff +++++++++++++++++++++++++++++++++ */
      INTS2                  i_client_no;    /* client number              */
/**/  INTS2                  i_unused;       /* currently unused           */
      INTS4                    l_channel;      /* connect channel            */
      /* ----------------------------------------------------------------- */
      /* stuff for buffer flushing */
      time_t                 time_t_lstbuf;  /* time, when last buf ackn   */
      /* ----------------------------------------------------------------- */
      INTU2         if_clntvalid;   /* flag client full valid     */
      INTU2         if_client_con;  /* flag client is connected   */
      INTU2         if_flt_valid;   /* flag filter rec and valid  */
      INTU2         if_clnt_mrk ;   /* flag client marked for del */
      struct s_clntoutbuf    *ps_clntoutbuf; /* ptr to curr output buf     */
      CHARS                   *ph_current;    /* ptr to 1st free byte       */
      /* ----------------------------------------------------------------- */
      INTU4          lf_byteswap;    /* byte swap enabled/disabl */ 
      /* ----------------------------------------------------------------- */
      /* +++++ following section identical to s_clnt_filter ++++++++++++++ */
      /* --- control byte ordering and machine type (2LW) ---- */
      INTU4          l_testbit;      /* bit pattern                */
      INTU4          l_endian;       /* endian of sender           */
      /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      INTS4                   l_numb_of_evt;  /* numb of events to send     */
      INTS4                   l_sample_rate;  /* flt match sample rate      */ 
      INTS4                   l_flush_rate;   /* buffer flushing rate [sec] */
      struct s_filter        filter[GPS__MAXFLT];
                                             /* 32 filter express (à 3LW)  */
      struct s_flt_descr     flt_descr[GPS__MAXFLTDESCR]; 
                                             /* Filter descriptor          */
      INTU2         if_fltsev;      /* filter on subevent         */
      INTU2         if_fltevt;      /* filter on event            */
      INTU2         if_wrtsev;      /* wrt subevts (numb of sev)  */
      INTU2         if_wrtevt;      /* write whole event          */
      /* ----------------------------------------------------------------- */
      INTS4                   lp_subevt_wrt[16]; /* vector with sev ptr     */
      INTS4                   l_subevt_len[16];  /* length of sev in bytes  */
					        /* IndexOrigin = 1 !!      */
      INTS4                   l_clntoutbuf_fcnt; /* filter matched count    */  
      /* ----- following section identical to s_clntoutbuf --------------- */
      /* ----- length is CLNT__INFO_CLIENT ---------- -------------------- */
      INTU4          l_clntoutbuf_sdev; /* sent events             */
      INTU4          l_clntoutbuf_sdby; /* sent bytes              */
      INTU4          l_clntoutbuf_sdbf; /* sent buffer             */
      INTU4          l_clntoutbuf_prev; /* processed evt since con */
      INTU4          l_clntoutbuf_fltm; /* filter matched on evt   */  
      /* ----------------------------------------------------------------- */
      time_t                 time_t_sndbuf;  /* time, when buf sent        */
      /* ----------------------------------------------------------------- */
      };
