/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : m_lea_user                                          */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : m_lea_user xxx                                      */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : User main analysis program template.                */
/*                                                                    */
/*+ ARGUMENTS   :                                                     */
/*                                                                    */
/*+ Return type : int                                                 */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*+ CALLING     : m_lea_user                                          */
/*                                                                    */
/*3+Function******+***********+****************************************/
/*                This working template accepts a few steering        */
/*                parameters, opens an input source and optional      */
/*                output file and reads events. It uses the           */
/*                profiles and analysis function of ADCF.             */
/*2+Implementation************+****************************************/
/*+ Utility     : m_lea                                               */
/*+ File name   : m_lea_user.c                                        */
/*+ Home direct.:  VMS: lea$src                                       */
/*                 AIX: /usr/local/GSI/lea                            */
/*+ Version     : 1.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 01-mar-2000                                         */
/*+ Object libr.:  VMS: lealib                                        */
/*                 AIX: /usr/local/GSI/AIX/lib/liblea.a               */
/*+ Updates     : Date        Purpose                                 */
/*1- C Procedure *************+****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"

#ifdef GSI__LINUX   /* Linux */
#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"
#include "s_ve10_1_swap.h"
#include "s_evhe_swap.h"
#endif

#ifdef GSI__WINNT           /* Windows NT */
#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"
#include "s_ve10_1_swap.h"
#include "s_evhe_swap.h"
#include <time.h>
#include <winsock.h>
#endif

#ifdef _AIX
#include "s_filhe.h"
#include "s_bufhe.h"
#include "s_ve10_1.h"
#include "s_evhe.h"
#define AIX_DEC
#endif

#include "f_evt.h"

/********************************************************************/
main(int argc, char **argv){
  s_evt_channel *s_gl_chan_i;
  s_evt_channel *s_gl_chan_o;
  s_ve10_1 *ps_ve10_1=NULL;
  s_evhe   *ps_evhe=NULL;
  s_filhe  *ps_filhe=NULL;
  s_bufhe  *ps_bufhe=NULL;
  INTS4 ll,l_sts,l_evts=0,l_size=16384,l_streambuf=4;
  /* arguments with defaults: */
  INTS4 l_server=GETEVT__FILE,l_out=0,l_print=0,l_events=10,l_data=0;
  CHARS c_source[64],c_output[64],c_args[64],c_type[64];
  char sbuf[8192];
  if(argc == 1)
    {
      printf("input events [output] [-trans -stream -event -file -out -print -data]\n");
      exit(0);
    }
  /*-------------------------------------------------------------------------------*/
  s_gl_chan_i=f_evt_control();
  s_gl_chan_o=f_evt_control();
  strcpy(c_source,"short.lmd");
  strcpy(c_output,"");
  if(argc > 3)
    {
      if(strstr(argv[3],"-") == NULL)
	strcpy(c_output,argv[3]);
    }
  if(argc > 2)
    {
      if(strstr(argv[2],"-") == NULL)
	l_events=atol(argv[2]);
    }
  if(argc > 1)strcpy(c_source,argv[1]);
  if(l_events <= 0)l_events=100000000;
  strcpy(c_args,"");
  strcpy(c_type,"file");
  for(ll=1;ll<argc;ll++) strcat(c_args,argv[ll]);
  if(strstr(c_args,"-t") != NULL) {l_server=GETEVT__TRANS;  strcpy(c_type,"MBS transport");}
  if(strstr(c_args,"-s") != NULL) {l_server=GETEVT__STREAM; strcpy(c_type,"MBS stream server");}
  if(strstr(c_args,"-e") != NULL) {l_server=GETEVT__EVENT;  strcpy(c_type,"MBS event server");}
  if(strstr(c_args,"-f") != NULL) {l_server=GETEVT__FILE;   strcpy(c_type,"file");}
  if(strstr(c_args,"-o") != NULL) l_out=1;
  if(strstr(c_args,"-p") != NULL) l_print=1;
  if(strstr(c_args,"-d") != NULL) l_data=1;
  if(strstr(c_args,"-n") != NULL) l_streambuf=0;
/*-------------------------------------------------------------------------------*/
/* Open input and output channels                                                */
/*-------------------------------------------------------------------------------*/
  if(f_evt_get_open(l_server, c_source, s_gl_chan_i, (CHARS **) &ps_filhe,0,0)!=GETEVT__SUCCESS)
    {
      printf("Error opening channel %s   ",c_source);
      exit(0);
    }
  printf("Opened input from %s %s, process %d events\n",c_type,c_source,l_events);
  if(l_out == 1)
    {
      /* 4 spanning buffers, size 16384, type 10, subtype 1 */
      if(f_evt_put_open(c_output,l_size,l_streambuf, 10, 1, s_gl_chan_o, NULL)!=GETEVT__SUCCESS)
	{
	  printf("Error opening output channel %s\n",c_output);
	  f_evt_get_close(s_gl_chan_i);
	  exit(0);
	}
      printf("Opened output file %s\n",c_output);
    }
/*-------------------------------------------------------------------------------*/
/* Event loop                                                                    */
/*-------------------------------------------------------------------------------*/
for(ll=0;ll<l_events;ll++)
{
    if(f_evt_get_event(s_gl_chan_i, (INTS4 **) &ps_ve10_1, (INTS4 **) &ps_bufhe) != GETEVT__SUCCESS) break;
     /* note that ps_bufhe and ps_filhe might be NULL */

    if((l_print == 1) ) f_evt_type(NULL,(s_evhe *) ps_ve10_1,-1,0,l_data,0);
    l_evts++;

    printf("l_server_type  = %d\n", s_gl_chan_i->l_server_type);
    printf("l_buf_type     = %d\n", s_gl_chan_i->l_buf_type);
    printf("l_buf_size     = %d\n", s_gl_chan_i->l_buf_size);
    printf("l_io_buf_size  = %d\n", s_gl_chan_i->l_io_buf_size);
    printf("l_evt_size     = %d\n", s_gl_chan_i->l_evt_size);
    printf("lpc_io_buf     = %p\n", s_gl_chan_i->pc_io_buf);
    printf("lpc_evt_buf    = %p\n", s_gl_chan_i->pc_evt_buf);
    memcpy((char *)sbuf, s_gl_chan_i->pc_evt_buf, 40);
    int i;
    for(i=0;i<20;i++){
      printf("%d 0x%04x\n", i, sbuf[i]);
    }

    if(l_out == 1)
    f_evt_put_event(s_gl_chan_o, (INTS4 *) ps_ve10_1);
}
/*-------------------------------------------------------------------------------*/
f_evt_get_close(s_gl_chan_i);
if(l_out == 1)
{
       f_evt_put_close(s_gl_chan_o);
}

printf("Events: %d\n",l_evts);
}
