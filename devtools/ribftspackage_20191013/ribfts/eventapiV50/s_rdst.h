#include "typedefs.h"

/*
 * N. Kurz 
 * structure used to indicate status of subevent readout
 */

typedef struct
{
	INTU4 bl_wait_trig;  /* 1: wait for trigger                      */
	                             /* 0: not waiting trigger                   */

	INTU4 bl_wait_pipe;  /* 1: wait for free pipe index              */
	                             /* 0: not waiting for free pipe index       */

	INTU4 bl_wait_dat;   /* 1: wait for free subevent data buffer    */
	                             /* 0: not waiting for free se. data buffer  */

	INTU4 bl_dat_full[2];/* 1: subevent data buffer (0,1) full       */
	                             /* 0: subevent data buffer (0,1) empty      */
} s_rdst;
