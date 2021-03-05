#include "typedefs.h"
/* ================= GSI Event buffer ======================= */
/* modified 03.02.1993  RSMayer */
/* =======> swapped <=============== */
struct s_evbuf {
       INTS4         l_evbuf_len;      /* data len + 8 words */
       INTS4         l_evbuf_prebuf;   /* previous buffer no */
       INTS4         l_evbuf_dlen;
       INTS2        i_evbuf_subtype;
       INTS2        i_evbuf_type;
       INTS2        i_evbuf[2097152]; /* data(l_evbuf_len - 8) */
      } ;
/* ---------------------------------------------------------- */        
