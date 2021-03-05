/* S_USL_SWAP.H  union 2 unsigned words and unsigned long */
/* RSMayer, 28.01.94 introduced today for portability     */
/* see also S_USL.H for little endian                     */

/* get unsigned word and longword from data */

union u_usl
   {
      struct
      {
         short unsigned i_u2;
         short unsigned i_u1;
      }  iu;
      long unsigned lu;
   };
