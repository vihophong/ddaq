/* Generated from GOOVME[SA$VES10_1]  */
/* modified 02-02-1993 RSMayer        */ 
/*  ================= GSI VME Subevent header =======================  */
/* -------> Swapped <-------- */
/* --------=========--------- */
struct s_ves10_1 
   {
    long l_ves10_1_dlen;       /*  Data length +2 in words */
    short i_ves10_1_subtype;  
    short i_ves10_1_type; 
    char  h_ves10_1_control;   /*  Processor type code */
    char  h_ves10_1_subcrate;  /*  Subcrate number */
    short i_ves10_1_procid;    /*  Processor ID [as loaded from VAX] */
    short i_ves10_1[EB__BUFFER_DATA]; 
   } ;
  /* ------------------------------------------------------------------ */
