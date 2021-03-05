#include "typedefs.h"
#include "f_his_hist.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "f_radware.h"
/**************************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
s_his_head *ps_his_head;
INTS4 i_j,l_status,l_port;
INTS4 l_size,l_histos=0;
INTS4 *pl_all,*pl_l;
REAL4 *pr;
CHARS c_server[32];
CHARS c_histo[32];
CHARS c_base[32];
CHARS c_access[32];
CHARS c_file[128];

if(argc < 7)
{
printf("node port|6009 password base histogram [-dir|-his] [-data|-rad]\n");
exit(0);
}
l_port=atol(argv[2]);
strcpy(c_server,argv[1]);
strcpy(c_access,argv[3]);
strcpy(c_base,argv[4]);
strcpy(c_histo,argv[5]);

pl_all=NULL;
/* get directory ******************************************************/
if(strstr(argv[6],"-d") != NULL)
{
    printf("Get directory %s from server %s [%d] pass=%s base=%s\n",c_histo,c_server,l_port,c_access,c_base);
    ps_his_head=NULL;
    pl_all=NULL;
    l_status = f_his_getdir(c_server,l_port,c_base,c_access,c_histo,(INTS4 **)&pl_all,(INTS4 **)&l_histos);
    if(l_status == COMM__SUCCESS)
    {
    printf("Histograms: %d, slot size: %d b, Total: %d b\n",l_histos,sizeof(s_his_head),l_histos*sizeof(s_his_head));
    ps_his_head = (s_his_head *)pl_all;
    for(i_j=0;i_j<l_histos;i_j++)
    {
	printf("%-31s %s %8d %8d\n",ps_his_head->c_name,ps_his_head->c_dtype,ps_his_head->l_bins_1,ps_his_head->l_bins_2);
	ps_his_head++;
    }
    } else printf("Error %d\n",l_status);
}

/* get histogram ******************************************************/
if(strstr(argv[6],"-h") != NULL)
{
    printf("Get histogram %s from server %s [%d] pass=%s base=%s \n",c_histo,c_server,l_port,c_access,c_base);
    ps_his_head=NULL;
    pl_all=NULL;
    l_status = f_his_gethis(c_server,l_port,c_base,c_access,c_histo,(s_his_head **)&ps_his_head,(INTS4 **)&pl_all,(INTS4 **)&l_size);
    if(l_status == COMM__SUCCESS)
    {
    printf("Channels: %d, Total: %d b\n",ps_his_head->l_bins_1*ps_his_head->l_bins_2,l_size);
    printf("%-31s %s %8d %8d\n",ps_his_head->c_name,ps_his_head->c_dtype,ps_his_head->l_bins_1,ps_his_head->l_bins_2);
    pl_l = pl_all;
    pr=(REAL4 *)pl_all;
    if(argc >= 8)
    {
      if(strstr(argv[7],"-d") != NULL)
      {
	/* print data channels */
	for (i_j = 0; i_j < (ps_his_head->l_bins_1 - ps_his_head->l_bins_1 % 6); i_j += 6)
	{
	   printf("%7d:%11.4g %11.4g %11.4g %11.4g %11.4g %11.4g\n",
		  i_j + 1,(REAL4) *pl_l,(REAL4) *(pl_l+1),(REAL4) *(pl_l+2),(REAL4) *(pl_l+3),(REAL4) *(pl_l+4),(REAL4) *(pl_l+5));
	   pl_l+=6;
	}
	if ((ps_his_head->l_bins_1 % 6))
	{
	printf("%7d:",i_j + 1);
	for (i_j = ps_his_head->l_bins_1 - ps_his_head->l_bins_1 % 6; i_j < ps_his_head->l_bins_1; i_j++)
	{
	      printf("%11.4g ", (REAL4) *pl_l);
	      pl_l++;
	}
	printf("\n");
	}
      }
      if(strstr(argv[7],"-r") != NULL)
      {
	if(argc == 9)strcpy(c_file,argv[8]);
	else strcpy(c_file,"radware.spe");
	l_status=f_radware_out1d(c_file,c_histo,pr,ps_his_head->l_bins_1,0);
	if(l_status == 0)printf("Radware histogram %s written to file %s.\n",c_histo,c_file);
	else             printf("Radware histogram %s NOT written to file %s, ERROR!\n",c_histo,c_file);
      }
    }}    else printf("Error %d\n",l_status);
    
}
free(pl_all);
}

