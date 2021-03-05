#include "typedefs.h"
/* +++++++++++++++++++++++
 * +++ queu_mac.h      +++
 * +++++++++++++++++++++++
 * RSMayer
 * 17-Feb-1994
 * +++++++++++++++++++++++
 * create queues and chained structures
 * example:
 *                                                                        
 * 
 * CAUTION: inside a loop with QUEU_BEGIN() and QUEU_END() 
 *          do NOT use QUEU_REE()! Be very carefull with
 *          QUEU_INT() and QUEU_REH() because you may trouble
 *          the POINTER !!!          (RSM 23-Feb-1994)
 */

/*  +++ QUEU_INI(P_HEAD) +++
 *  initialize a header directory
 *  P_HEAD : input pointer to header directory
 *
 */
#define QUEU_INI(P_HEAD) \
{ \
INTS4 *p_head; \
p_head        = (INTS4 *) P_HEAD; \
*p_head       = (INTS4) p_head;  \
*(p_head + 1) = (INTS4) p_head; \
}



/*  +++ QUEU_BEGIN(STRUCTURE,P_HEAD,POINTER) +++
 *  loop over all members of a chained structure
 *  P_HEAD    : input pointer to header directory
 *  STRUCTURE : input structure name or typedef
 *  POINTER   : output pointer to structure
 */
#define QUEU_BEGIN(STRUCTURE,P_HEAD,POINTER) \
POINTER = (STRUCTURE *) *P_HEAD;  \
while ((INTS4) P_HEAD != (INTS4) POINTER) \
{



/*  +++ QUEU_END(STRUCTURE,HEADERNAME,POINTER) +++
 *  loop over all members of a chained structure
 *  POINTER   : input pointer to current structure
 *  STRUCTURE : input structure name or typedef
 *  HEADERNAME: input name of the header in the structure
 */
#define QUEU_END(STRUCTURE,HEADERNAME,POINTER) \
POINTER = (STRUCTURE *) *POINTER->HEADERNAME;   \
}



/*  +++ QUEU_INT(P_HEAD,P_NEW) +++
 *  insert a new member at the end of a queue of chained structures
 *  P_HEAD    : input pointer to header directory  
 *  P_NEW     : input pointer to new structure to be inserted
 */
#define QUEU_INT(P_HEAD,P_NEW) \
{ \
INTS4 *p_hd, *p_last; \
p_hd = p_last = (INTS4 *) P_HEAD; \
while (*p_hd != (INTS4) P_HEAD) \
{ \
p_last = p_hd; \
p_hd   = (INTS4 *) *p_hd; \
} \
*p_hd       = (INTS4) P_NEW; \
*(p_hd + 1) = (INTS4) p_last; \
p_last      = p_hd; \
p_hd        = (INTS4 *) P_NEW; \
*p_hd       = (INTS4) P_HEAD; \
*(p_hd + 1) = (INTS4) p_last; \
}



/*  +++ QUEU_REH(P_HEAD,P_ELEM) +++
 *  remove a member from the head of  chained structures
 *  P_HEAD    : input pointer to header directory  
 *  P_ELEM    : input pointer to removed struct or NULL
 */
#define QUEU_REH(P_HEAD,P_ELEM) \
if ((INTS4 *) P_HEAD == (INTS4 *) *P_HEAD) \
{ \
P_ELEM = NULL; \
} \
else \
{ \
INTS4 *p_head, *p_elem, *p_next; \
p_head  = (INTS4 *) P_HEAD; \
P_ELEM  = *P_HEAD; \
p_elem  = (INTS4 *) *P_HEAD; \
p_next  = (INTS4 *) *p_elem; \
*p_head = (INTS4)  p_next; \
*(p_next + 1) = (INTS4) p_head; \
*p_elem       = (INTS4) p_elem; \
*(p_elem + 1) = (INTS4) p_elem; \
}



/*  +++ QUEU_REE(P_ELEM) +++
 *  remove an element from the queue of chained structures
 *  P_ELEM    : input pointer to removed struct or NULL
 */
#define QUEU_REE(P_ELEM) \
{ \
INTS4 *p_forw, *p_elem, *p_next; \
p_elem        = (INTS4 *) P_ELEM; \
if ( p_elem == (INTS4 *) *p_elem ) \
{ \
printf("E-QUEU-REE: element %d not member of a queue\n", p_elem); \
} \
else \
{ \
p_next        = (INTS4 *) *p_elem; \
p_forw        = (INTS4 *) *(p_elem + 1); \
*p_forw       = (INTS4) p_next; \
*(p_next + 1) = (INTS4) p_forw; \
*p_elem       = (INTS4) p_elem; \
*(p_elem + 1) = (INTS4) p_elem; \
} \
}



/*  +++ QUEU_SHO(P_HEAD) +++
 *  loop over all members of a chained structure and display pointers
 *  P_HEAD    : input pointer to header directory
 */
#define QUEU_SHO(P_HEAD) \
{ \
INTS4 *p_head, *p_ptr, i = 0; \
p_head      = (INTS4 *) P_HEAD; \
printf("QUEU_SHO: header:    p:%d \t fwd:%d \t bck:%d\n", \
        p_head, p_ptr = (INTS4 *) *p_head, *(p_head+1) ); \
while (p_ptr != p_head) \
{ \
printf("QUEU_SHO: member:%2d: p:%d \t fwd:%d \t bck:%d\n", \
        ++i, p_ptr , *p_ptr, *(p_ptr+1) ); \
p_ptr = (INTS4 *) *p_ptr; \
} \
}
