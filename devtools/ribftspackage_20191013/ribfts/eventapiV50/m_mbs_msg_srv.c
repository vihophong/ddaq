/*****************+***********+****************************************/
/*                                                                    */
/*   GSI, Gesellschaft fuer Schwerionenforschung mbH                  */
/*   Postfach 11 05 52                                                */
/*   D-64220 Darmstadt                                                */
/*                                                                    */
/*1+ C Procedure *************+****************************************/
/*                                                                    */
/*+ Module      : m_mbs_msg_serv                                      */
/*                                                                    */
/*--------------------------------------------------------------------*/
/*+ CALLING     : m_mbs_msg_serv                                      */
/*--------------------------------------------------------------------*/
/*                                                                    */
/*+ PURPOSE     : STC based message server for MBS.                   */
/*                                                                    */
/*2+Description***+***********+****************************************/
/*                                                                    */
/*                This version works on Unix, VMS                     */
/*                                                                    */
/*2+Implementation************+****************************************/
/*+ File name   : m_mbs_msg_serv.c                                    */
/*+ Home direct.: /mbs//src                                           */
/*+ Version     : 2.01                                                */
/*+ Author      : H.G.Essel                                           */
/*+ Created     : 10-NOV-1998                                         */
/*+ Updates     : Date        Purpose                                 */
/*1- C Procedure *************+****************************************/
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "typedefs.h"
#include "f_stccomm.h"
#include "portnum_def.h"
/**************************************************************************/
main(INTS4 argc,CHARS *argv[])
/**************************************************************************/
{
INTS4 l_gl_tcp_chan;
INTS4 l_gl_port;
INTS4 l_status,l;
/* structure used to talk between client and server */
typedef struct{
      INTS4 l_order;       /* byte order. Set to 1 by sender */
      INTS4 l_type;        /* type of message                */
      INTS4 l_status;      /* completion status              */
      CHARS c_info[512];   /* info buffer                    */
} s_info;
s_info           *ps_info;
struct s_tcpcomm *ps_tcpcomm;
CHARS c_node[64];
INTS4 l_retry=0;
/*---------------------------------------------------------------------*/
ps_tcpcomm = (struct s_tcpcomm *) malloc (sizeof( struct s_tcpcomm));
ps_info   = (s_info *) malloc (sizeof( s_info));

strcpy(c_node,argv[1]);
l_gl_port=PORT__MSG_REM;

while(1)
{
    l_status = f_stc_connectserver (c_node,l_gl_port,&l_gl_tcp_chan,ps_tcpcomm);
    while (l_status != STC__SUCCESS)
    {
  sleep(3);
  l_retry++;
  if(l_retry > 1000) goto G_exit;
  l_status = f_stc_connectserver (c_node,l_gl_port,&l_gl_tcp_chan,ps_tcpcomm);
    }
/*---------------------------------------------------------------------*/
    printf("Connected to %s\n",c_node);
    while(1)
    {
  /* Read command line from client */
  l_status = f_stc_read (ps_info, sizeof (s_info), l_gl_tcp_chan,-1);
  switch (l_status)
  {
      case STC__SUCCESS:
      break;
      case STC__TIMEOUT: printf ("Timeout, disconnect!\n");
      break;
      case STC__BROPIPE: printf ("Broken pipe, disconnect!\n");
      break;
      case STC__NODATA:  printf ("No data,disconnect!\n");
      break;
      default:           printf ("Error %d,disconnect!\n",l_status);
      break;
  }
  if(l_status != STC__SUCCESS) break;
  printf("%s\n",ps_info->c_info);
    } /* end read loop */
/*---------------------------------------------------------------------*/
    f_stc_discclient (l_gl_tcp_chan);
    f_stc_close (ps_tcpcomm);

} /* end connect loop */

G_exit:
printf("Message server exit!\n");
exit(0);
}
