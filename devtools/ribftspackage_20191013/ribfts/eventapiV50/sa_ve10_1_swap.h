/* Generated from GOOVME[SA$VE10_1]  */
/* modified 02-02-1993 RSMayer       */ 
/*  ================= GSI VME Event header =======================  */
/* -------> Swapped <-------- */
/* --------=========--------- */
struct s_ve10_1 
    {
    long  l_ve10_1_dlen;   /*  Data length + 4 in words */
    short i_ve10_1_subtype; 
    short i_ve10_1_type; 
    short i_ve10_1_trigger;   /*  Trigger number */
    short i_ve10_1_dummy;   /*  Not used yet */
    long  l_ve10_1_count;   /*  Current event number */
    short i_ve10_1[EB__BUFFER_DATA];
    } ;
/* ------------------------------------------------------------------ */
