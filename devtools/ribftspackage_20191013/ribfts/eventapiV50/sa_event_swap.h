/* ================= GSI Event header ======================= */
/* -------> Swapped <-------- */
/* --------=========--------- */
/* modified 03-02-1993 RSMayer        */ 
struct s_event {
       long  l_event_dlen;
       short i_event_subtype; 
       short i_event_type;
       short i_event[EB__BUFFER_DATA]; /* this is modified from 512 */
       } ;
/*------------------------------------------------------------------*/
