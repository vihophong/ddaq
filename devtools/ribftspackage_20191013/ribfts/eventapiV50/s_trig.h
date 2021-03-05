#include "typedefs.h"

/* 
 * N. Kurz
 * virtual pointer to the trigger module register 
 */

typedef struct
{
	INTS4 volatile  *pl_stat_rd, *pl_ctrl_rd, *pl_fcatr_rd, *pl_ctimr_rd;
	INTS4 volatile  *pl_stat_wr, *pl_ctrl_wr, *pl_fcatr_wr, *pl_ctimr_wr;
} s_trig; 
