#include <stdio.h>
#include "typedefs.h"
#include "f_histo_io.h"
#include "f_swaplw.h"

//=================================
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
 )
//----------------------------------
{
  head_histobin h_head;
  FILE *fout;

  memset(&h_head,0,sizeof(head_histobin));

  h_head.endian    = 1;
  h_head.headid    = 100;
  h_head.headbytes = sizeof(head_histobin)-8;
  h_head.itembytes = xbins*ybins*4;
  h_head.xbins     = xbins;
  h_head.ybins     = ybins;
  h_head.isfloat   = isfloat;
  h_head.xlow      = xlow;
  h_head.ylow      = ylow;
  h_head.xup       = xup;
  h_head.yup       = yup;
  strncpy(h_head.name, name,63);
  strncpy(h_head.title,title,63);
  strncpy(h_head.xlet, xlet,63);
  strncpy(h_head.ylet, ylet,63);

  fout=fopen(file,"w");
  fwrite(&h_head,1,sizeof(head_histobin),fout);
  fwrite(data,1,h_head.itembytes,fout);
  fclose(fout);
  return 0;
}
//=================================
INTS4 f_histo_head
(
 CHARS *file,
 head_histobin *h_head,
 INTS4 print
 )
//----------------------------------
{
  FILE *fin;

  fin=fopen(file,"r");
  if(fin == 0) {printf("histo_head:   error open: %s\n",file); return -1; }
  fread(h_head,1,sizeof(head_histobin),fin);
  fclose(fin);
  if(h_head->endian != 1)
    {
      f_swaplw((INTS4 *)h_head,7,NULL);
      f_swaplw((INTS4 *)&h_head->xlow,4,NULL);
    }
  if(print)
    {
	printf("file: %s ",file);
	printf("name: %s ",h_head->name);
	printf("title: %s\n",h_head->title);
	printf("xbins:%6d ",h_head->xbins);
	printf("xlow:%8.2f ",h_head->xlow);
	printf("xup:%8.2f ",h_head->xup);
	printf("xlett: %s\n",h_head->xlet);
	if(h_head->ybins > 1)
	  {
	    printf("ybins:%6d ",h_head->ybins);
	    printf("ylow:%8.2f ",h_head->ylow);
	    printf("yup:%8.2f ",h_head->yup);
	    printf("ylett: %s\n",h_head->ylet);
	  }
	if(h_head->isfloat)
	  printf("Float bytes: %d\n",h_head->itembytes);
        else
	  printf("Integer bytes: %d\n",h_head->itembytes);
    }
  return 0;
}
//=================================
INTS4 f_histo_read
(
 CHARS *file,
 void *data
 )
//----------------------------------
{
  head_histobin h_head;
  FILE *fin;
  INTS4 swap=0;

  fin=fopen(file,"r");
  if(fin == 0) {printf("histo_read:   error open: %s\n",file); return -1; }
  fread(&h_head,1,sizeof(head_histobin),fin);
  if(h_head.endian != 1)
    {
      swap=1;
      f_swaplw((INTS4 *)&h_head,7,NULL);
      f_swaplw((INTS4 *)&h_head.xlow,4,NULL);
    }
  fread(data,1,h_head.itembytes,fin);
  if(swap)f_swaplw((INTS4 *)data,h_head.itembytes/4,NULL);
  fclose(fin);
  return 0; 
}
