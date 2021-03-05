/*
 * last modified : 15/02/16 14:30:29 
 * 
 * Hidetada Baba (RIKEN)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define DB(x)

#include "transfer.h"
#include "ridf.h"
#include "bb-decode-V7XX.h"

#define ANRCVPORT   17502
#define SHMID       5

int mkudpsock(int port, struct sockaddr_in *saddr);

void signal_block(){
}
int msgDefineMessage(unsigned int id, unsigned int xclass, unsigned int level, char *source, char *body) {
  return 0;
}

struct stsyncdata{
  unsigned int id;
  unsigned int adc;
  unsigned long long int ts;
};

int transid = 0;
char *data, *ridf;
void *quit(void){
  transferMultiClose(transid);

  free(ridf);
  free(data);
  
  exit(0);
}

int main(int argc, char argv[]){
  const int buffersize = 64*1024;
  int sock = 0;
  struct sockaddr_in saddr, caddr;
  RIDFHD hd, thd, sthd;
  RIDFRHD rhd, trhd, rsthd;
  int recvsz, i, maxsockid, clen, idx, tidx, adc;
  int dataidx = 0;
  fd_set fdset;
  unsigned long long int ts, tts;
  struct stbsegid bsid;
  struct stsyncdata syncdata;
  const int geo = 9;
  const int ch = 0;


  syncdata.id = 1;

  data = malloc(buffersize+32);
  if(!data){
    printf("Cannot' malloc sender data%d\n", buffersize);
    return 0;
  }

  ridf = malloc(buffersize+32);
  if(!ridf){
    printf("Cannot' malloc ridf data%d\n", buffersize);
    return 0;
  }

  if(!(sock = mkudpsock(ANRCVPORT+SHMID, &saddr))){
    printf("Cannot open udp socket\n");
    return 0;
  }


  printf("testsender\n");

  /* Signal */
  signal(SIGINT, (void *)quit);


  transferMultiPort(transid, 10306);
  transferMultiBlockSize(transid, buffersize);
  //transferMultiInit(transid, "10.32.0.10");
  transferMultiInit(transid, "10.32.6.196");


  // wait for data
  maxsockid = sock;

  while(1){
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    if(select(maxsockid+1, &fdset, NULL, NULL, NULL) != 0){
      recvsz = recvfrom(sock, ridf, buffersize, 0,
			(struct sockaddr *)&caddr, (socklen_t*)&clen);
      printf("Select ok\n");
      
      memcpy((char *)&hd, ridf, sizeof(hd));
      rhd = ridf_dechd(hd);

      switch(rhd.classid){
      case RIDF_EF_BLOCK:
      case RIDF_EA_BLOCK:
      case RIDF_EAEF_BLOCK:
	printf("Receive data from RIBFDAQ size=%d\n", rhd.blksize*2);
	memset(data, 0, buffersize);
	dataidx = 0;
	idx = sizeof(hd);

	while(idx < rhd.blksize*2){
	  memcpy((char *)&thd, ridf+idx, sizeof(thd));
	  trhd = ridf_dechd(thd);
	  // analyze event block
	  if(trhd.classid == RIDF_EVENT || trhd.classid == RIDF_EVENT_TS){
	    tidx = sizeof(hd) + sizeof(int);
	    // analyze buffer
	    syncdata.adc = 0x8000;
	    syncdata.ts = 0x8000000000000000;
	
	    if(trhd.classid == RIDF_EVENT_TS){
	      tidx += sizeof(tts);
	    }
	    // analyze segment id
	    while(tidx < trhd.blksize*2){
	      memcpy((char *)&sthd, ridf+idx+tidx, sizeof(hd));
	      rsthd = ridf_dechd(sthd);
	      memcpy((char *)&bsid, ridf+idx+tidx+sizeof(hd), sizeof(int));
	      //printf("tidx=%d idx=%d\n", tidx, idx);
	      //printf("dev=%d, fp=%d, det=%d, mode=%d\n",
	      //bsid.device, bsid.focal, bsid.detector, bsid.module);
	      
	      adc = -1;
	      tts = 0x800000000000000;
	      // check Timestamp
	      if(bsid.device == 1 && bsid.focal == 11
		 && bsid.detector == 60 && bsid.module == 42){
		memcpy((char *)&tts, 
		       ridf+idx+tidx+sizeof(hd)+4, sizeof(tts));
		tts = (tts>>2) & 0x0000ffffffffffff;
		syncdata.ts = tts;
		//printf("%d 0x%016llx\n", sizeof(tts), tts);
	      }else if(bsid.device == 1 && bsid.focal == 11
		       && bsid.detector == 23 && bsid.module == 21){
		adc = get_chdata_V7XX(geo, ch, ridf+idx+tidx+sizeof(hd)+4, 
				  rsthd.blksize*2-sizeof(hd)-4);
		if(adc >= 0){
		  syncdata.adc = adc;
		}
	      }

	      if(adc != -1 && tts != 0x8000000000000000){
		memcpy(data+32+dataidx, (char *)&syncdata, sizeof(syncdata));
		dataidx += sizeof(syncdata);
	      }
	      // skip
	      tidx += rsthd.blksize * 2;
	    }
	  }
	  
	  idx += trhd.blksize*2;
	}
	transferMultiTxData(transid, &data[32], 1, buffersize-32);
	break;
      default:
	printf("\n ****** Error header ******\n");
	printf("layer = %d, classid = %d\n", rhd.layer, rhd.classid);
	printf("blksize = %d, efn = %d\n", rhd.blksize, rhd.efn);
	printf("\n");
	break;
      }
    }
  }

  quit();

  return 0;
}
