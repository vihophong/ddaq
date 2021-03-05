/* ================= Fastbus data word structure ===================*/
/* Structure of data words  */
/* maps to LA$VESFB_data(i) */
/* --------=========--------- */
struct s_fbdata 
   {
   unsigned b5_addr:5;
   unsigned b3_ev  :3;
   unsigned b1_r   :1;
   unsigned b7_ch  :7;
   unsigned b4_x   :4;
   unsigned b12_d  :12;
   };
