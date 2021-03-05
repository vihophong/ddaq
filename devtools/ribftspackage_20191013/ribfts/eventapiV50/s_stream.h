#include "typedefs.h"

/*
 * N. Kurz 
 * stream control structure
 * all members of the following structure must be long.
 * the size of this structure is set to 4 * longword bound.
 * this is done to force the event buffer streams immediately following
 * these structures (one for each stream) are also 4 * longword bounded.
 * if this it NOT the case the dma transfer on the E7 isn't working correctly,
 * which results in a max. transfer speed of ~200 kb/sec via scsi to exabyte. 
 */ 

typedef struct
{
	INTU4 bl_stream_full;
	INTU4 bl_next_ctr_off; /* offset with respect to shared segment  */
	                               /* begin to the start of the next buffer  */
	                               /* stream control structure (in bytes)    */
 
	INTU4 bl_stream_start_off; /* offset with respect to shared seg- */
	                                   /* ment begin to the start of the     */
	                                   /* buffer stream  (in bytes)          */
	
	INTU4 bl_stream_act_off; /* offset with respect to shared seg-   */
	                                 /* ment begin to the pointer of the buf-*/
	                                 /* fer stream which indicates the actual*/
	                                 /* filling (in bytes)                   */

	INTU4 bl_n_buf;          /* nr. of buffers in a single stream    */

	INTU4 bl_buf_len;        /* length of single buffer  (in bytes)  */

	INTU4 bl_netto_buf_len;  /* netto length of one buffer (in bytes)*/

	INTU4 bl_stream_len;     /* length of single stream (in bytes)   */

	INTU4 bl_stream_filled;  /* */
	INTU4 bl_dummy[3];       /* see explanation above */
} s_stream;
