#include "typedefs.h"
/* +++ s_evctr.h +++
 * control structure for SBS monitor
 * (differs from s_control.h for GPS-Server on (Open)VMS )
 * R.S. Mayer
 * 18-Feb-1994 
 * modified: 
 *  29-Mar-1994 RSM members added
 *  31-mar-1994 RSM pointer and offsets to shared segments added
 */

  struct s_evctr {                       /* main control structure	     */
      struct s_evbuf   *ps_evbuf;        /* pointer to eventbuffer       */
      INTS4             l_client_mrk[GPS__MAXCLIENTS + 2];
                                         /* vector for marked clients    */
      INTS4             l_client_ack[GPS__MAXCLIENTS + 2];
                                         /* vector for read ackn         */
	  /* --------------------------------------------------------------- */
      INTS4              l_port;           /* TCP/IP port number           */
	  /* --------------------------------------------------------------- */
	  /* --------------------------------------------------------------- */
      /* + + + following part is also in s_clntoutbuf  + + + + + + + + + */
	  INTU4  l_inbuf_read_cnt; /* count read buffer              */
	  INTU4  l_inbuf_rdok_cnt; /* count read buffer that are o.k */
	  INTU4  l_inbuf_skip_cnt; /* count skipped buffer           */
	  INTU4  l_inbuf_byrd_cnt; /* count read bytes               */
	  INTU4  l_inbuf_proc_cnt; /* count processed buffer         */
	  INTU4  l_inbuf_prev_cnt; /* count processed events         */
      /* + + + length is CLNT_INFO_CONTROL + + + + + + + + + + + + + + + */
      INTU4  l_inbuf_bywr_cnt; /* count written bytes            */
      /* --------------------------------------------------------------- */
      struct   s_client        *ps_client_head[2]; /* header client queue */
      struct   s_client        *ps_client_free[2]; /* client free   queue */
      struct   s_tcpcomm       *ps_tcpcomm;        /* I/O        block TCP*/
      /* --------------------------------------------------------------- */
      INTS2          i_maxclients;     /* maximum number of clients      */
      INTS2          i_client;         /* running client number          */
      INTS2          i_client_act;     /* currently active clients, (having 
                                        * still space in output buffer   */
      INTS2          if_stop_input;    /* flag to stop mbx input stream  */
      INTS2          i_client_con;     /* currently connect clients      */
                                       /* count p_client_head            */
                                       /* count p_client_free            */
      INTS2          i_client_total;   /* total existing client struct   */
      /* --------------------------------------------------------------- */
      CHARS           *pc_evsername;    /* pointer to EVent Server name   */
      CHARS           *pc_hostname;     /* pointer to host name           */
      INTS4           l_skbufstr;       /* rd every n buffer stream       */
      INTS4           l_bufstr_cnt;     /* count buffer streams           */
      INTS4           l_max_evt;        /* take max n events per stream   */
      INTS4           l_max_evt_cnt;    /* count events per buffer stream */
      /* --------------------------------------------------------------- */
      CHARU  h_lam;            /* Look at me! (ther's a change)  */
      CHARU  h_ctrl_c;         /* Ctrl^C                         */
      CHARU  h_ctrl_z;         /* Ctrl^Z                         */
      CHARU  h_ctrl_bckslsh;   /* Ctrl^\                         */
      CHARU  h_portserv;       /* connect port server            */
      CHARU  h_ev_serv_start;  /* event server started           */
      CHARU  h_dummy2;         /* dummy                          */
      CHARU  h_dummy3;         /* dummy                          */
      /* --------------------------------------------------------------- */
      INTS4           *pl_stream_seg;     /* virtual address of the start */
                                         /* of the buffer stream segment */
      s_setup        *ps_setup;          /* */
      s_form         *ps_form;           /* */
      s_daqst        *ps_daqst;          /* */
      /* --------------------------------------------------------------- */
      CHARS           c_modnam[64];       /* test */ 
      INTS4           *pl_ut_error_ctrl;  /* pointer to control struct    */
      };
