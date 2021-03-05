#include "typedefs.h"
/* [mayer.lynx.eson]S_ESOBUF_swap.H  */
/* This is the TCP buffer as sent and returned */
/* The sent buffer is always big enough to keep the data */

/* word order for CVC, E7, E6, AIX, HP endian */

struct s_esobuf
        {
        /* +++ length ESS__ESOBUF_HEAD  +++ */
/*  20 bytes first header. used is l_endian and l_data_bytes. */
        /* +++ length ESS__ESOBUF_H1 +++ */
	INTS2   i_subtype;    /* currently 1 */
	INTS2   i_type;       /* currently 1 */
	INTS4    l_tstendian;  /* !!! must be 1 , otherwise swap */
	INTS4    l_mode;       /* not used */
	INTS4    l_status;     /* !!! return status */
	INTS4    l_data_bytes; /* # of bytes following */

/* This part is described in I$CAMACT.PPL and GOOINC(SI$CAMCB) */
/* It is the same as used in VME$SRC:fep_esone.cp, except that the word */
/* ordering is like VAX, not VME (VME$INC:sv_fep_esone. uses VME order)*/
/* 12 bytes list header */
        /* +++ length ESS__ESOBUF_H2 +++ */
	INTS2	i_head_lgtrd;
	INTS2	i_head_lgtwt;
	INTS4  	l_head_channel;
	INTS2	i_head_length; /* # of bytes following */
	INTS2	i_head_branch;

/* 8 bytes esone header */
        /* +++ length ESS__ESOBUF_H3 +++ */
	INTS2  i_esone_length;        /* Descriptor length        */
	INTS2  i_esone_type;          /* Type of ESONE call       */
        INTS2  i_esone_tally;         /* Number of returned data  */
        INTS2  i_esone_count;         /* Number of NAF to execute */
        } ;

/* For i_esone_type = 1 :                                                  */
/* there are i_esone_count NAF longword pairs starting after l_esone_tally */
/* a pair is the NAF and data (to be written or read)                      */
/*                                                                         */
/* For i_esone_type = 2 or 3:                                              */
/* there is one NAF longword after l_esone_tally. The NAF must be excuted  */
/* i_esone_count times and fill i_esone_count following longwords.         */
/*                                                                         */
/* For i_esone_type = 4 or 5:                                              */
/* there is one NAF longword after l_esone_tally. The NAF must be excuted  */
/* i_esone_count times and fill i_esone_count following words.             */

