#include "typedefs.h"
/* N.Kurz
 * the master readout must allocate as many structures of the following type 
 * as crates have to be readout by the master                                
 * each readout task will fill this values during runtime for the crates     
 * that will be handeld by this task.                                    
 */ 

typedef struct
{
	INTS4 *pl_rd_tab_start;           /* virtual pointer to the start of the  */
	                                 /* readout table                        */

	INTS4 **pl_init[SBS__N_CR];       /* virtual pointer to the start of the  */
	                                 /* init table                           */

	INTS4 **pl_read[SBS__N_CR][SBS__N_TRG_TYP]; /* the same but for the readout part    */
	                                 /* but also as a function of the        */
	                                 /* triggertype                          */				

} s_rdtab;
