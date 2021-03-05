#include "typedefs.h"
/* +++ s_esctr.h +++
 * control structure for Esone Server
 * R.S. Mayer
 * 29-Jun-1994 
 * modified: 
 * 15-Jul-1994 new members /RSM
 */

  struct s_esctr 
  {                                      /* main control structure       */
      INTS4             l_client_mrk[ESS__MAXCLIENTS + 2];
                                         /* vector for marked clients    */
      INTS4             l_client_ack[ESS__MAXCLIENTS + 2];
                                         /* vector for read ackn         */
      /* --------------------------------------------------------------- */
      INTS4              l_port;           /* TCP/IP port number           */
      /* --------------------------------------------------------------- */
      struct   s_esclnt        *ps_esclnt_head[2]; /* header esclnt queue */
      struct   s_esclnt        *ps_esclnt_free[2]; /* esclnt free   queue */
      struct   s_tcpcomm       *ps_tcpcomm;        /* I/O        block TCP*/
      /* --------------------------------------------------------------- */
      INTS2          i_maxesclnts;     /* maximum number of esclnts      */
      INTS2          i_esclnt;         /* running esclnt number          */
      INTS2          i_esclnt_act;     /* currently active esclnts, (having 
                                        * still space in output buffer   */
      INTS2          i_esclnt_con;     /* currently connect esclnts      */
                                       /* count p_esclnt_head            */
                                       /* count p_esclnt_free            */
      INTS2          i_esclnt_total;   /* total existing esclnt struct   */
      INTS2          if_print;         /* flag: printout in f_ut_esone   */
      /* --------------------------------------------------------------- */
      /* ----- following section identical to s_esclntbuf  ------------- */
      /* ----- length is ESS__CTRL_ESCLNT ------------------------------ */
      INTU4          l_esclntbuf_rdby;  /* read bytes            */
      INTU4          l_esclntbuf_sdby;  /* sent bytes            */
      INTU4          l_esclntbuf_rdbf;  /* read buffer           */
      INTU4          l_esclntbuf_sdbf;  /* sent buffer           */
      /* --------------------------------------------------------------- */
      CHARS           *pc_essername;    /* pointer to Esone Server name   */
      CHARS           *pc_hostname;     /* pointer to host name           */
      /* --------------------------------------------------------------- */
      CHARU  h_lam;            /* Look at me! (ther's a change)  */
      CHARU  h_ctrl_c;         /* Ctrl^C                         */
      CHARU  h_ctrl_z;         /* Ctrl^Z                         */
      CHARU  h_ctrl_bckslsh;   /* Ctrl^\                         */
      CHARU  h_portserv;       /* connect port server            */
      CHARU  h_es_serv_start;  /* esone server started           */
      CHARU  h_dummy2;         /* dummy                          */
      CHARU  h_dummy3;         /* dummy                          */
      /* --------------------------------------------------------------- */
      INTS4           *pl_stream_seg;     /* virtual address of the start */
                                         /* of the buffer stream segment */
      s_setup        *ps_setup;          /* */
      s_form         *ps_form;           /* */
      s_daqst        *ps_daqst;          /* */
      INTS4           l_cam_offset;       /* offset CAMAC segment         */
      INTS4           l_vsb_offset;       /* offset VSB c segment         */
      /* --------------------------------------------------------------- */
      INTS4           *pl_ut_error_ctrl;  /* pointer to control struct    */
      };
