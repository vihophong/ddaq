/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : m_mbs_status                                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : m_mbs_status                                        */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : Outputs status blocks of (remote) MBS nodes.        */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*                This version works on Linux                         */
/*                                                                    */
/*1- C Procedure *************+****************************************/

#include "typedefs.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "sbs_def.h"
#include "sys_def.h"
#include "ml_def.h"
#include "s_daqst.h"
#include "s_setup.h"
#include "s_set_ml.h"
#include "s_set_mo.h"
#include "f_mbs_status.h"
INTS4 f_ut_seg_show(s_daqst *, s_setup *, s_set_ml *, s_set_mo *);

int l_status,l;

/**************************************************************************/
main(argc,argv)
     int argc;
     char *argv[];
{
  CHARS c_node[64];
  INTS4 l_swap=0;
  INTS4 l_total;
  INTS4 l_cmd,l_rate=0,l_setup=0,l_daqst=0,l_set_ml=0,l_set_mo=0;
  INTS4 i,k,l_items,l_size,l_crate;
  INTS4 *pl_b,*pl_o;
  s_daqst *ps_daqst;
  s_setup *ps_setup;
  s_set_ml *ps_set_ml;
  s_set_mo *ps_set_mo;

  printf("Sizes B of daqst:%d, setup:%d, set_ml:%d, set_mo:%d\n",
       sizeof(s_daqst),sizeof(s_setup),sizeof(s_set_ml),sizeof(s_set_mo));

  strcpy(c_node,"");

  for(i=1;i<argc;i++)
    {
      if(strstr(argv[i],"-r")       != NULL) l_rate=1;
      else if(strstr(argv[i],"-st") != NULL) l_daqst=1;
      else if(strstr(argv[i],"-se") != NULL) l_setup=1;
      else if(strstr(argv[i],"-ml") != NULL) l_set_ml=1;
      else if(strstr(argv[i],"-mo") != NULL) l_set_mo=1;
      else                                   strcpy(c_node,argv[i]);
    }

  if(l_rate == 1)
    {
      ps_daqst   = (s_daqst *) malloc (sizeof( s_daqst));
      while(1)
	{
	  l_status = f_mbs_status (c_node,ps_daqst);
	  printf("%7d Bufs, %10d Evts, %4d Bufs/s, %4d KB/s, %5d Evts/s \n"
		 ,ps_daqst->bl_n_buffers
		 ,ps_daqst->bl_n_events
		 ,ps_daqst->bl_r_buffers
		 ,ps_daqst->bl_r_kbyte
		 ,ps_daqst->bl_r_events);
	  sleep(1);
	}
      free(ps_daqst);
      return;
    }

  if((l_daqst + l_setup + l_set_ml + l_set_mo) == 0) l_daqst=1;

  /**************************************************************************/
  if(l_daqst == 1)
    {
      printf("MBS status of node %s\n",c_node);
      ps_daqst   = (s_daqst *) malloc (sizeof( s_daqst));
      l_status = f_mbs_status (c_node,ps_daqst);
           if(l_status == -2) printf("Node %s not responding!\n",c_node);
      else if(l_status == -1) printf("Version mismatch daqst!\n");
      else l_status = f_ut_seg_show(ps_daqst,NULL,NULL,NULL);
      free(ps_daqst);
    }
  /**************************************************************************/
  if(l_setup == 1)
    {
      printf("MBS setup of node %s\n",c_node);
      ps_setup   = (s_setup *) malloc (sizeof( s_setup));
      l_status = f_mbs_setup (c_node,ps_setup);
           if(l_status == -2) printf("Node %s not responding!\n",c_node);
      else if(l_status == -1) printf("Version mismatch setup!\n");
      else if(l_status == -3) printf("setup not loaded!\n");
      else l_status = f_ut_seg_show(NULL,ps_setup,NULL,NULL);
      free(ps_setup);
    }
  /**************************************************************************/
  if(l_set_ml == 1)
    {
      printf("MBS multi setup of node %s\n",c_node);
      ps_set_ml   = (s_set_ml *) malloc (sizeof( s_set_ml));
      l_status = f_mbs_ml_setup (c_node,ps_set_ml);
           if(l_status == -2) printf("Node %s not responding!\n",c_node);
      else if(l_status == -1) printf("Version mismatch set_ml!\n");
      else if(l_status == -3) printf("set_ml not loaded!\n");
      else l_status = f_ut_seg_show(NULL,NULL,ps_set_ml,NULL);
      free(ps_set_ml);
    }
  /**************************************************************************/
  if(l_set_mo == 1)
    {
      printf("MBS multi output setup of node %s\n",c_node);
      ps_set_mo   = (s_set_mo *) malloc (sizeof( s_set_mo));
      l_status = f_mbs_mo_setup (c_node,ps_set_mo);
           if(l_status == -2) printf("Node %s not responding!\n",c_node);
      else if(l_status == -1) printf("Version mismatch set_mo!\n");
      else if(l_status == -3) printf("set_mo not loaded!\n");
      else l_status = f_ut_seg_show(NULL,NULL,NULL,ps_set_mo);
      free(ps_set_mo);
    }
}
