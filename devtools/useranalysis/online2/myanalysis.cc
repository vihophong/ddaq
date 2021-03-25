/* ************************ DO NOT REMOVE  ****************************** */
#include <iostream>
#include <pmonitor.h>
#include <TSystem.h>
#include <TROOT.h>
#include "pdecoder.h"
#include <dpp.h>
#include <libDataStruct.h>
#include <DataStruct_dict.h>

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
#define V1740_N_CH 64
#define V1740_PACKET 100
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

//counter
//Int_t dgtz_clong[V1730_N_MAX_BOARD*V1730_N_MAX_CH+V1740_N_CH];
int trgcnt_prev;
int its;
long long ts_prev;

int dgtzcnt_prev[V1730_N_MAX_BOARD*V1730_N_MAX_CH+V1740_N_CH];
long long dgtzts_prev[V1730_N_MAX_BOARD*V1730_N_MAX_CH+V1740_N_CH];
int tdccnt_prev[TDC_N_CH];
long long tdcts_prev[TDC_N_CH];

//! histograms and trees
TH2F *h2;
TH1F *h1wf[V1730_N_MAX_BOARD][V1730_N_MAX_CH];
TH2F *h2wf[V1730_N_MAX_BOARD][V1730_N_MAX_CH];
TH1F *htdc;

TH1F *h3wf[V1740_N_MAX_CH*V1740_];
TH2F *h4wf[V1740_N_MAX_CH];
TH2F *h5;


TH1F *hratedgtz;

TH1F *htrgrateintegrate;



void Init(){
    htrgrateintegrate = new TH1F("rate","rate",1000,0,1000);
    h2 = new TH2F("e2d","Energy Spectra 2D",V1740_N_CH,0,V1740_N_CH,500,0,500);
    h5 = new TH2F("e2d740","Energy Spectra 2D",V1740_N_CH,0,V1740_N_CH,2000,0,2000);
    for (Int_t i=0;i<V1730_N_MAX_BOARD;i++){
        for (Int_t j=0;j<V1730_N_MAX_CH;j++){
            h1wf[i][j] = new TH1F (Form("wf1d_%d_%d",i,j),Form("Waveform 1d %d_%d",i,j), N_MAX_WF_LENGTH, 0,N_MAX_WF_LENGTH );
            h2wf[i][j] = new TH2F (Form("wf2d_%d_%d",i,j),Form("Waveform 2d %d_%d",i,j), N_MAX_WF_LENGTH, 0,N_MAX_WF_LENGTH, 1700, 0,17000 );
        }
    }
    for (int i=0;i<V1740_N_MAX_CH;i++){
	h3wf[i] = new TH1F (Form("wf740_1d_%d",i),Form("Waveform 1d v1740 %d",i), N_MAX_WF_LENGTH, 0,N_MAX_WF_LENGTH );
	h4wf[i] = new TH2F (Form("wf740_2d_%d",i),Form("Waveform 2d v1740 %d",i), N_MAX_WF_LENGTH, 0,N_MAX_WF_LENGTH, 2500, 0,5000 );
    }
    htdc = new TH1F("htdc","htdc",TDC_N_CH,0,TDC_N_CH);
    for (int i=0;i<TDC_N_CH;i++){
      tdccnt_prev[i]=0;
      tdcts_prev[i]=0;
    }
    for (int i=0;i<V1730_N_MAX_BOARD*V1730_N_MAX_CH+V1740_N_CH;i++){
	dgtzcnt_prev[i] = 0;
	dgtzts_prev[i] = 0;

    }
    
    trgcnt_prev=0;
    ts_prev = 0;
    its = 0;    
}

void ProcessEvent(NIGIRIHit* data){
    if (data->evt_type == V1740_EVENT_TYPE){
        int b = data->b;
        int ch  = (b-V1740_BOARD_N)*V1730_N_MAX_CH + data->ch;
//cout<<(b-V1740_BOARD_N)*V1730_N_MAX_CH + data->ch<<endl;
        int itcnt=0;
    if (data->clong>0) {h5->Fill(ch,data->clong);h2->Fill(ch,data->finets);}
	for (std::vector<UShort_t>::iterator it = data->pulse.begin() ; it != data->pulse.end(); ++it){
            if (itcnt<N_MAX_WF_LENGTH){
                h4wf[ch]->Fill(itcnt,*it);
                h3wf[ch]->SetBinContent(itcnt+1,*it);
            }
            itcnt++;
        }
    }else if (data->evt_type == TDC_EVENT_TYPE){
      for (std::vector<UShort_t>::iterator it = data->tdc_ch.begin() ; it != data->tdc_ch.end(); ++it){
	htdc->Fill(*it);
	h2->Fill(*it+V1730_N_MAX_BOARD*V1730_N_MAX_CH+V1740_N_CH,25000);
	//! tdc counter, rate metter
      }
      if (((data->evt-trgcnt_prev)>COUNTS_TO_CAL==0)){	
	double tdiff= (double)((long long)data->ts - (long long)ts_prev)/1e10;
	double rate = (double)(data->evt-trgcnt_prev)/tdiff;//cps	
	if (ts_prev>0){           
	   htrgrateintegrate->SetBinContent(its+1,rate);
	   its++;
	   if (its>1000) {
		htrgrateintegrate->Reset();
		its = 0;
	   }          	              			  
	}
        ts_prev = (long long) data->ts;	
	trgcnt_prev = (int)data->evt;
      }      
      
           
    }else{
        cout<<"ERROR! @.@"<<endl;
    }
    //! digitizer counter rate meter
	if (data->evt_type == V1740_EVENT_TYPE){
	    int ich =data->b*V1730_N_MAX_CH+data->ch;
	    if (dgtzcnt_prev[ich]%COUNTS_TO_CAL==0){
		if (dgtzcnt_prev[ich]>0){
		   double rate=((double)(data->evt-dgtzcnt_prev[ich]))/((double)(data->ts-dgtzts_prev[ich]))/DGTZ_CLK_RES*1e9;
		   hratedgtz->SetBinContent(ich+1,rate);
		}
		dgtzts_prev[ich] = data->ts;
		dgtzcnt_prev[ich] = data->evt;
	    }
	}

    
}

void CloseMe(){

}




