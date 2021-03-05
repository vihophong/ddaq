#include "typedefs.h"


/*
 * N. Kurz:  not valid anymore
 * setup slave CVC parameter structure declaration, all the values of the 
 * structure elements are derived from the s_setup structure elements
 */

typedef struct
{
	INTU4 l_cr_nr;            /* crate nr of slave CVC               */
	INTU4 lp_VSB_start;       /* start of VSB address space (local)  */
	INTU4 bl_VSB_len;         /* length of VSB address space (local) */
	INTU4 bl_pipe_len;        /* length of subevent pipe             */
	INTU4 lp_CAMAC_CVC_local; /* start of CAMAC address space        */
	INTU4 lp_CLB_CVC;         /* CVC CAMAC LAM Buffer address        */
 	INTU4 lp_CSR_CVC;         /* CVC Control a. Status Register addr.*/
	INTU4 lp_IRQ_SOURCE_CVC;  /* Irq Source register of CVC          */
	INTU4 lp_IRQ_MASK_CVC;    /* Irq Mask register of CVC            */
	INTU4 l_cvt;         /* triggermodule conversion time            */
	INTU4 l_fca;         /* triggermodule fast clear acceptance time */
	                             /* both not converted                       */
	INTU4 bl_max_se_len[SBS__N_CR]; /* maximal length of subevent for     */
	                                   /* trigger type                       */
} setup_CVC_slave;	 
