/* ************************ DO NOT REMOVE  ****************************** */
#include <iostream>
#include <pmonitor.h>
#include <TSystem.h>
#include <TROOT.h>
#include "pdecoder.h"
#include <dpp.h>
#include <libDataStruct.h>
#include <DataStruct_dict.h>

#include "transfer.h"

#define DGTZ_CLK_RES 8
#define V1730_EVENT_TYPE 1
#define V1740_EVENT_TYPE 2
#define TDC_EVENT_TYPE 3

#define V1730_N_MAX_BOARD 5
#define V1730_N_MAX_CH 16
#define N_MAX_INFO 9

#define TDC_BOARD_N 50
#define TDC_PACKET 10
#define TDC_N_CH 64

#define V1740_BOARD_N 5
#define V1740_HDR 6
#define V1740_N_CH 32
#define V1740_PACKET_1 100
#define V1740_PACKET_2 101
#define V1740_PACKET_3 102
#define V1740_PACKET_4 103
#define V1740_N_MAX_CH 64
#define NSBL 8
// 5000000 - 15 % memory = 1.2 Gb

/* ********************************************************************** */

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <vector>
#include <map>
#include <TCanvas.h>

#define N_REFESH_WF 1000
#define N_MAX_WF_LENGTH 500

#define COUNTS_TO_DISPLAY 10000
#define COUNTS_TO_CAL 5000
#define MAX_N_SAMPLE 500

#define MAX_MAP_LENGTH 2000000

using namespace std;

struct stsyncdata{
  unsigned int id;
  unsigned int adc;
  unsigned long long int ts;
};

int transid = 1; ///try exp 0;
char *dataxfer;
void *quit(void){
  transferMultiClose(transid);
  free(dataxfer);
  exit(0);
}
const int buffersize = 16*1024;
int dataxferidx;
struct stsyncdata syncdata;
int connection_status;


int trgcnt_prev;
int its;
long long ts_prev;


void Init(){    
    trgcnt_prev=0;
    ts_prev = 0;
    its = 0;
    dataxferidx=0;
    connection_status=0;

    syncdata.id = 2;
    syncdata.adc = 0;
    dataxfer = (char*) malloc(buffersize+32);
    if(!dataxfer){
      printf("Cannot' malloc sender data%d\n", buffersize);
      exit(0);
    }
    printf("rcdaqsender\n");
    /* Signal */
    //signal(SIGINT, (void *)quit);
    transferMultiPort(transid, 10307);
    transferMultiBlockSize(transid, buffersize);
    if (transferMultiInit(transid, "10.32.0.12")==0) connection_status=1;

}

void ProcessEvent(NIGIRIHit* data){

  if (dataxferidx>=buffersize){
      if (connection_status) transferMultiTxData(transid, &dataxfer[32], 1, buffersize-32);
      unsigned int ttt;
      memcpy((char *)&ttt, &dataxfer[32], sizeof(ttt));
      printf("data = %llu (buffersize=%d)\n", ttt, buffersize);
      memset(dataxfer, 0, buffersize);
      dataxferidx=0;
  }
  if (data->evt_type == TDC_EVENT_TYPE){
    //cout<<"lupots = "<<data->ts*10<<endl;
    //syncdata.ts=(data->ts>>2) & 0x0000ffffffffffffLL;
    //syncdata.ts=(data->ts>>2)& 0x0000ffffffffffffLL;
    syncdata.ts=data->ts/4;
    cout<<std::hex<<"0x"<<syncdata.ts<<" - ts="<<std::dec<<syncdata.ts<<endl;
    //printf("TS=%llu 0x%08llX\n", syncdata.ts, syncdata.ts);
    memcpy(dataxfer+32+dataxferidx, (char *)&syncdata, sizeof(syncdata));
    dataxferidx += sizeof(syncdata);
  }
  if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0){
    //cout<<"ee"<<endl;
    //cout<<"dgtz "<<data->b<<" ts = "<<data->ts*8<<endl;
    //syncdata.ts=data->ts;
    //memcpy(dataxfer+32+dataxferidx, (char *)&syncdata, sizeof(syncdata));
    //dataxferidx += sizeof(syncdata);
  }

}

void CloseMe(){

}




