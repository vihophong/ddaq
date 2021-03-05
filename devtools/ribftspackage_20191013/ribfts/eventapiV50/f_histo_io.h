#include "head_histobin.h"
INTS4 f_histo_dump
(
 CHARS *file,
 CHARS *name,
 CHARS *title,
 INTS4 xbins,
 REAL4 xlow,
 REAL4 xup,
 CHARS *xlet,
 INTS4 ybins,
 REAL4 ylow,
 REAL4 yup,
 CHARS *ylet,
 INTS4 isfloat,
 void *data
 );
//=================================
INTS4 f_histo_head
(
 CHARS *file,
 head_histobin *h_head,
 INTS4 print
 );
INTS4 f_histo_read
(
 CHARS *file,
 void *data
 );

