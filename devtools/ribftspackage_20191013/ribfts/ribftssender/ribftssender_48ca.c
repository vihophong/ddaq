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

int transid = 1; ///try exp 0;
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
  int recvsz, i, maxsockid, clen, idx, tidx;
  short adc, coin;
  int dataidx = 0;
  fd_set fdset;
  unsigned long long int ts, tts, pts, ttt;
  struct stbsegid bsid;
  struct stsyncdata syncdata;
  const int geo = 9;
  const int ch = 0;

  int dbmode = 0, tsok=0;
  int coinmode = 0, coinok = 0;

  syncdata.id = 1;
  pts = 0x000000000000000LL;

  if(argc == 2) dbmode = 1;
  if(argc == 3) coinmode = 1; dbmode=1;

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


  printf("ribftssender\n");

  /* Signal */
  signal(SIGINT, (void *)quit);

  transferMultiPort(transid, 10306);
  transferMultiBlockSize(transid, buffersize);
  transferMultiInit(transid, "10.32.0.12");


  // wait for data
  maxsockid = sock;

  while(1){
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    if(select(maxsockid+1, &fdset, NULL, NULL, NULL) != 0){
      recvsz = recvfrom(sock, ridf, buffersize, 0,
			(struct sockaddr *)&caddr, (socklen_t*)&clen);
      ///// printf("Select ok\n");
      
      memcpy((char *)&hd, ridf, sizeof(hd));
      rhd = ridf_dechd(hd);

      switch(rhd.classid){
      case RIDF_EF_BLOCK:
      case RIDF_EA_BLOCK:
      case RIDF_EAEF_BLOCK:
	/////	printf("Receive data from RIBFDAQ size=%d\n", rhd.blksize*2);
	/////	printf("Receive data from RIBFDAQ size=%d\n", rhd.blksize*2);
	memset(data, 0, buffersize);
	dataidx = 0;
	idx = sizeof(hd);


	while(idx < rhd.blksize*2){
	//printf("blksize... %d \n", rhd.blksize);
	//while(idx < rhd.blksize){
	  memcpy((char *)&thd, ridf+idx, sizeof(thd));
	  trhd = ridf_dechd(thd);
	  // analyze event block
	  if(trhd.classid == RIDF_EVENT || trhd.classid == RIDF_EVENT_TS){
	    tidx = sizeof(hd) + sizeof(int);
	    // analyze buffer
	    syncdata.adc = 0x8000;
	    syncdata.ts = 0x8000000000000000;
	    tsok = 0;
	    coinok = 0;
	    adc = -1;
	    tts = 0;
	    tsok = 0;
	
	    if(trhd.classid == RIDF_EVENT_TS){
	      tidx += sizeof(tts);
	    }
	    // analyze segment id
	    while(tidx < trhd.blksize*2){ // analyze event
	      memcpy((char *)&sthd, ridf+idx+tidx, sizeof(hd));
	      rsthd = ridf_dechd(sthd);
	      memcpy((char *)&bsid, ridf+idx+tidx+sizeof(hd), sizeof(int));
	      //if(dbmode){
	      //printf("tidx=%d idx=%d\n", tidx, idx);
	      //printf("dev=%d, fp=%d, det=%d, mode=%d\n",
	      //bsid.device, bsid.focal, bsid.detector, bsid.module);
	      //}
	      
	      // check Timestamp
	      if(bsid.device == 1 && bsid.focal == 41
		 && bsid.detector == 60 && bsid.module == 42){
		memcpy((char *)&tts, 
		       ridf+idx+tidx+sizeof(hd)+4, sizeof(tts));
		tts = tts & 0x0000ffffffffffffLL;
		if(tts != 0){
		  tts = (tts>>2) & 0x0000ffffffffffffLL;
		  if(tts != pts){
		    syncdata.ts = tts ;
		    pts = tts;
		    tsok = 1;
		  }
		}

		if(dbmode){
		  if(tsok){
		    printf("TS=%llu 0x%08llX\n", tts, tts);
		  }
		}
		//printf("%d 0x%016llx\n", sizeof(tts), tts);
	      }else if(bsid.device == 1 && bsid.focal == 11
		       && bsid.detector == 4 && bsid.module == 0){
		memcpy((char *)&adc, 
		       ridf+idx+tidx+sizeof(hd)+4, sizeof(adc));
		if(adc >= 0){
		  syncdata.adc = adc;
		}
	      }else if(bsid.device == 0 && bsid.focal == 63
		       && bsid.detector == 10 && bsid.module == 0){
		memcpy((char *)&coin, 
		       ridf+idx+tidx+sizeof(hd)+4, sizeof(coin));
		//printf("coin=0x%04x\n", coin);
		if(coin & 0x0008){
		  coinok = 1;
		  //printf("coinok\n");
		}
	      }

	      tidx += rsthd.blksize * 2;
	    } // end of event analysis

	    //if(adc != -1 && tsok==1){  // previous
	    if((tsok==1 && coinmode==0) ||
	       tsok==1 && coinmode==1 && coinok==1){
	      memcpy(data+32+dataidx, (char *)&syncdata, sizeof(syncdata));
	      dataidx += sizeof(syncdata);
	      //printf("idx=%d\n", dataidx);
	    }
	    // skip
	    //printf("tidxxxx=%d\n", tidx);
	  }
	  
	  idx += trhd.blksize*2;
	}
	//printf("trasfering data ribf... %d \n", rhd.blksize);
	transferMultiTxData(transid, &data[32], 1, buffersize-32);
	memcpy((char *)&ttt, &data[32], sizeof(ttt));
	//printf("data = %llu (buffersize=%d)\n", ttt, buffersize);
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
