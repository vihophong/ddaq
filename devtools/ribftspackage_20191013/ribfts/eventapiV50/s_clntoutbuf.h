#include "typedefs.h"
/* +++ s_clntoutbuf.h +++
 * output buffer structure for SBS monitor 
 * (differs from s_clntoutbuf.h for GPS-Server on (Open)VMS )
 * R.S. Mayer
 * 18-Feb-1994
*/

struct s_clntoutbuf 
    {
    /* ----- some control stuff ---------------------------- */
    struct s_client     *ps_client;         /* ptr to "father" client     */
    INTS4                l_ack_buf;          /* read client buff received  */
    INTS4                l_ack_bytes;        /* read client bytes received */
    INTU4       l_clnt_sts;         /* client sts 32:buf sended   */
                                            /*             0:reset        */ 
    /* ----------------------------------------------------- */
    /* ----- length is CLNT__OUTBUFHEAD -------------------- */
    /* --- control byte ordering and machine type (2LW) ---- */
    INTU4   l_testbit;              /* bit pattern                */
    INTU4   l_endian;               /* endian of sender           */
    /* ----------------------------------------------------- */
    /* ----- length is CLNT__BUFHEAD ----------------------- */
    INTS4            l_dlen;                 /* data length [bytes]        */
    INTS4            l_free;                 /* free  length [bytes]       */
    INTS4            l_events;               /* events in buffer           */
    INTS4            l_maxbufsiz;            /* maximum buffer size        */
    INTS4            l_bytestosnd;           /* sendet bytes               */
    INTS4            l_numbuftosnd;          /* number of buffers to send  */
    INTS4            l_client_con;           /* currently connect clients  */
    INTU4  l_buffertype;           /* type: data:1 msg:2 flush:4 */
                                            /*       last:8 1stBUF:16     */
                                            /* (inclusive) (mask)         */
    INTU4  l_msgtyp;               /* I:1 W:2 E:4 F:8 (exclusive)*/
    CHARS            c_message[CLNT__MSGLEN];/* error msg and other        */
    /* ----- part of s_control structure ------------------- */
    /* ----- length is CLNT__INFO_CONTROL ------------------ */
    INTU4  l_inbuf_read_cnt; /* count read buffer              */
    INTU4  l_inbuf_rdok_cnt; /* count read buffer that are o.k */
    INTU4  l_inbuf_skip_cnt; /* count skipped buffer           */
    INTU4  l_inbuf_byrd_cnt; /* count read bytes               */
    INTU4  l_inbuf_proc_cnt; /* count processed buffer         */
    INTU4  l_inbuf_prev_cnt; /* count processed events         */
    /* ----- part of s_client structure -------------------- */
    /* ----- length is CLNT__INFO_CLIENT ------------------- */
    INTU4          l_clntoutbuf_sdev; /* sent events             */
    INTU4          l_clntoutbuf_sdby; /* sent bytes              */
    INTU4          l_clntoutbuf_sdbf; /* sent buffer             */
    INTU4          l_clntoutbuf_prev; /* processed evt since con */
    INTU4          l_clntoutbuf_fltm; /* filter matched on evt   */
    /* ----------------------------------------------------------------- */
    CHARS            c_buffer[GPS__OUTBUFSIZ];
    };
