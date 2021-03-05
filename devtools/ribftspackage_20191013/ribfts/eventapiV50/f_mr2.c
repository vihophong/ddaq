#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camdef.h"
#include "f_mr2.h"
#define RETRY 1000
extern int _cam_errno;
int node_address, camac_register, Q, X, control_block[4], *pd, notOK; 

int f_mr2_cnaf (int   ,int,int ,int,int *,int *,int *);
//************************************************************************
int f_mr2_read(char *node,int station,int start,int channels, int *pdata, char *error)
{
  notOK=ccopen(node, &node_address);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}

  // set start read addr.
  notOK=f_mr2_cnaf(node_address,station,0,17,&start,&Q,&X);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  if(X == 0){ sprintf(error,"No X response station %d 0 17",station); return(1);}
  // read 24-bit words from MR2000
  control_block[0] = channels;
  control_block[1] = control_block[2] = 0;
  cdreg(&camac_register, node_address, 0, station, 1);
  if (_cam_errno != 0) { strcpy(error, cerror(-1, 1)); return(1);}
  cfubr(0, camac_register, pdata, control_block);
  if (_cam_errno != 0) { strcpy(error, cerror(-1, 1)); return(1);}
  if((control_block[0] != channels) |
     (control_block[1] != channels))
    {
      sprintf(error,"channels: %d, returned: %d, tally: %d\n"
	      ,channels,control_block[0], control_block[1]);
      return(1);
    }
  return(0);
}
//************************************************************************
int f_mr2_clear(char *node,int station,int start,int channels, char *error)
{
  int function,i;

  notOK=ccopen(node, &node_address);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // get state
  notOK=f_mr2_cnaf(node_address,station,3,27,&start,&Q,&X);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  if(X == 0){ sprintf(error,"No X response station %d 3 27",station); return(1);}
  if(Q==0)function=24; // q=0, was disabled
  else    function=26; // q=1, was enabled
  // while q = 0: set start
  for(i=0;i<RETRY;i++)
    {
      notOK=f_mr2_cnaf(node_address,station,2,17,&start,&Q,&X);
      if(X == 0){ sprintf(error,"No X response station %d 2 17",station); return(1);}
      if(Q == 1) break;
      if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
    }
  // while q = 0: set channels
  for(i=0;i<RETRY;i++)
    {
      notOK=f_mr2_cnaf(node_address,station,1,17,&channels,&Q,&X);
      if(X == 0){ sprintf(error,"No X response station %d 1 17",station); return(1);}
      if(Q == 1) break;
      if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
    }
  // clear mr2000 region
  notOK=f_mr2_cnaf(node_address,station,1,9,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 1 9",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // while q = 1: test clear finished
  for(i=0;i<RETRY;i++)
    {
      notOK=f_mr2_cnaf(node_address,station,2,27,&start,&Q,&X);
      if(X == 0){ sprintf(error,"No X response station %d 2 27",station); return(1);}
      if(Q == 0) break;
      if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
    }
  // clear address overflow
  notOK=f_mr2_cnaf(node_address,station,0,10,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 0 10",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}

  // clear data overflow
  notOK=f_mr2_cnaf(node_address,station,1,10,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 1 10",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}

  // set state
  notOK=f_mr2_cnaf(node_address,station,3,function,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 3 %d",station,function); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  return(0);
}
//************************************************************************
int f_mr2_init(char *node,int station, char *error)
{
  int start,i;

  notOK=ccopen(node, &node_address);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // stop
  notOK=f_mr2_cnaf(node_address,station,3,24,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 3 24",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // clear whole MR2000
  notOK=f_mr2_cnaf(node_address,station,0,9,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 0 9",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // while q = 1: test clear
  for(i=0;i<RETRY;i++)
    {
      notOK=f_mr2_cnaf(node_address,station,2,27,&start,&Q,&X);
      if(X == 0){ sprintf(error,"No X response station %d 2 27",station); return(1);}
      if(Q == 0) break;
      if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
    }
  // clear address overflow
  notOK=f_mr2_cnaf(node_address,station,0,10,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 0 10",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // clear data overflow
  notOK=f_mr2_cnaf(node_address,station,1,10,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 1 10",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  // set increment mode
  notOK=f_mr2_cnaf(node_address,station,2,24,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 2 24",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  return(0);
}
//************************************************************************
int f_mr2_start(char *node,int station, char *error)
{
  int start;
  notOK=ccopen(node, &node_address);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  notOK=f_mr2_cnaf(node_address,station,3,26,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 3 26",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  return(0);
}
//************************************************************************
int f_mr2_stop(char *node,int station, char *error)
{
  int start;

  notOK=ccopen(node, &node_address);
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  notOK=f_mr2_cnaf(node_address,station,3,24,&start,&Q,&X);
  if(X == 0){ sprintf(error,"No X response station %d 3 24",station); return(1);}
  if(notOK) { strcpy(error,cerror(-1,1)); return(1);}
  return(0);
}
//************************************************************************
int f_mr2_write(char *node,int station,int start,int channels, int *pdata, char *error)
{
  strcpy(error,"Write not implemented");
  return(1);
}
//************************************************************************
int f_mr2_cnaf(int node, int N, int A, int F, int *D, int *Q, int *X)
{
  int camac,qx;
  // get register
  cdreg(&camac, node, 0, N, A);
  if (_cam_errno != 0) return(1);
  // execute cnaf
  cfsa(F, camac, D, &qx);
  ctstat(&qx);
  *Q=(qx&1)^1;
  *X=((qx>>1)&1)^1;
  //  printf("Q=%d X=%d\n",*Q,*X);
  //  printf("N %2d A %2d F %2d D %08x qx %x\n",N,A,F,*D,*QX);
  if (_cam_errno != 0) return(1);
  if(*X == 0) return(1);
  return(0);
}
