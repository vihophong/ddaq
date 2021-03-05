#include "typedefs.h"

/*
 * N. Kurz
 * subevent control structure declaration. for each pipe element there will be 
 * such a structure in the VSB address space.
 */

typedef struct 
{
	INTU4 bl_se_ctr_word; /* bit 0: 0 = empty,  1 = full             */
	                              /* bit 1: 0 = synch., 1 = asynch.          */
	                              /* bit 2: subevent data is in buffer 0 or 1*/
	                              /* bit  3 - 7 : free                       */
	                              /* bit  8 - 15: pipe id (master = 0)       */
	                              /* bit 16 - 31: trigger status word        */
	INTU4 lp_next_wr_ctr; /* virtual address to next control struc-  */
	                              /* ture of this type, only to be used for  */
	                              /* writing of this struc. by readout task  */ 
	INTU4 lp_next_rd_ctr; /* virtual address to next control struc-  */
	                              /* ture of this type, only to be used for  */
	                              /* reading of this struc. by collect task  */ 
	                                
	INTU4 bl_dat_len;     /* length of data in subevent data buffer  */
	INTU4 lp_dat_off;     /* address of subevent data                */
} s_sectr; 
