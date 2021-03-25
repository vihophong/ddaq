/* ************************ DO NOT REMOVE  ****************************** */
#include <iostream>
#include <fstream>
#include <pmonitor.h>
#include <TSystem.h>
#include <TROOT.h>
#include "pdecoder.h"
#include <dpp.h>
#include <libDataStruct.h>
#include <DataStruct_dict.h>


#define DGTZ_CLK_RES 8
#define LUPO_CLK_RES 10



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

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <vector>
#include <map>
#include <TCanvas.h>

//#define WRITE_TO_TEXT
#define N_REFESH_WF 1000
#define N_MAX_WF_LENGTH 500

//#define COUNTS_TO_DISPLAY 10000
//#define COUNTS_TO_CAL 5000
#define MAX_N_SAMPLE 500

#define MAX_MAP_LENGTH 200

using namespace std;

std::multimap <ULong64_t,UChar_t> datamap_lupo; //! sort by timestamp
std::multimap <ULong64_t,UChar_t> datamap_dgtz0; //! sort by timestamp
std::multimap <ULong64_t,UChar_t> datamap_dgtz1; //! sort by timestamp
std::multimap <ULong64_t,UChar_t> datamap_dgtz2; //! sort by timestamp
std::multimap <ULong64_t,UChar_t> datamap_dgtz3; //! sort by timestamp
std::multimap<ULong64_t,UChar_t>::iterator it_datamap_lupo;
std::multimap<ULong64_t,UChar_t>::iterator it_datamap_dgtz0;
std::multimap<ULong64_t,UChar_t>::iterator it_datamap_dgtz1;
std::multimap<ULong64_t,UChar_t>::iterator it_datamap_dgtz2;
std::multimap<ULong64_t,UChar_t>::iterator it_datamap_dgtz3;

Int_t nlupo;
Int_t ncorr0;
Int_t ncorr1;
Int_t ncorr2;
Int_t ncorr3;
TCanvas* c1;
TH1F* hlupodgtz0;
TH1F* hlupodgtz1;
TH1F* hlupodgtz2;
TH1F* hlupodgtz3;

std::ofstream ofs("wasabi_timestamp.txt", std::ofstream::out);

int DoSort(bool flagend,NIGIRIHit* data = 0){
  if (datamap_lupo.size()>MAX_MAP_LENGTH||flagend){
         //! clear at the end
         //if (!flagend) datamap_lupo.erase(datamap2.begin(),it);
        for (it_datamap_lupo=datamap_lupo.begin();it_datamap_lupo!=datamap_lupo.end();it_datamap_lupo++){
	  //if (k%1000==0) cout<<ncorr<<" / "<<k<<endl;
            Long64_t ts=(Long64_t)it_datamap_lupo->first;	   
            Long64_t ts1 = ts - 12000;
            Long64_t ts2 = ts + 12000;
            Long64_t corrts = 0;
            it_datamap_dgtz0 = datamap_dgtz0.lower_bound(ts1);
            while(it_datamap_dgtz0!=datamap_dgtz0.end()&&it_datamap_dgtz0->first<ts2){
                corrts = (Long64_t) it_datamap_dgtz0->first;
                hlupodgtz0->Fill(corrts-ts);
                //if (!flagend) datamap_dgtz0.erase(it_datamap_dgtz0);
                ncorr0++;
                break;
            }
            it_datamap_dgtz1 = datamap_dgtz1.lower_bound(ts1);
            while(it_datamap_dgtz1!=datamap_dgtz1.end()&&it_datamap_dgtz1->first<ts2){
                corrts = (Long64_t) it_datamap_dgtz1->first;
                hlupodgtz1->Fill(corrts-ts);
                //if (!flagend) datamap_dgtz0.erase(it_datamap_dgtz0);
                ncorr1++;
                break;
            }
	    it_datamap_dgtz2 = datamap_dgtz2.lower_bound(ts1);
            while(it_datamap_dgtz2!=datamap_dgtz2.end()&&it_datamap_dgtz2->first<ts2){
                corrts = (Long64_t) it_datamap_dgtz2->first;
                hlupodgtz2->Fill(corrts-ts);
                //if (!flagend) datamap_dgtz0.erase(it_datamap_dgtz0);
                ncorr2++;
                break;
            }
        ts1 = ts + 66048;
        ts2 = ts + 86048;
	    it_datamap_dgtz3 = datamap_dgtz3.lower_bound(ts1);
            while(it_datamap_dgtz3!=datamap_dgtz3.end()&&it_datamap_dgtz3->first<ts2){
                corrts = (Long64_t) it_datamap_dgtz3->first;
                hlupodgtz3->Fill(corrts-ts);
                //if (!flagend) datamap_dgtz0.erase(it_datamap_dgtz0);
                ncorr3++;
                break;
            }

        }
	//if (!flagend) hlupodgtz1->Reset();

	datamap_lupo.clear();
	datamap_dgtz0.clear();
	datamap_dgtz1.clear();
	datamap_dgtz2.clear();
	datamap_dgtz3.clear();
	c1->cd(1)->Modified();
	c1->cd(1)->Update();
	c1->cd(2)->Modified();
	c1->cd(2)->Update();
	c1->cd(3)->Modified();
	c1->cd(3)->Update();
	c1->cd(4)->Modified();
	c1->cd(4)->Update();

	//cout<<ncorr0<<"/"<<ncorr1<<"/"<<ncorr2<<"/"<<ncorr3<<"/"<<nlupo<<endl;
    }
    if (!flagend) {
        //! insert data in the map
        if (data->evt_type==TDC_EVENT_TYPE) {
            datamap_lupo.insert(make_pair(data->ts*LUPO_CLK_RES,0));
	    cout<<"lupo\t"<<data->ts*LUPO_CLK_RES<<endl;
	    nlupo++;
        }
        else if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0&&data->b==0) {
            datamap_dgtz0.insert(make_pair(data->ts*DGTZ_CLK_RES,0));
        cout<<"d0\t"<<data->ts*DGTZ_CLK_RES<<endl;
        }else if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0&&data->b==1) {
	  datamap_dgtz1.insert(make_pair(data->ts*DGTZ_CLK_RES,0));
        cout<<"d1\t"<<data->ts*DGTZ_CLK_RES<<endl;
        }else if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0&&data->b==2) {
            datamap_dgtz2.insert(make_pair(data->ts*DGTZ_CLK_RES,0));
        cout<<"d2\t"<<data->ts*DGTZ_CLK_RES<<endl;

#ifdef WRITE_TO_TEXT
	ofs<<data->ts*DGTZ_CLK_RES<<endl;
#endif

        }else if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0&&data->b==3){
            datamap_dgtz3.insert(make_pair(data->ts*DGTZ_CLK_RES,0));
        cout<<"d3\t"<<data->ts*DGTZ_CLK_RES<<endl;
        }
    }
    /*
    if ((data->evt_type==TDC_EVENT_TYPE&&flagfirst>0)||flagend) {
       for (it_datamap_lupo=datamap_lupo.begin();it_datamap_lupo!=datamap_lupo.end();it_datamap_lupo++)
       {
            Long64_t ts=(Long64_t)it_datamap_lupo->first;
            Long64_t ts1 = ts - 1000;
            Long64_t ts2 = ts + 1000;
            Long64_t corrts = 0;
            it_datamap_dgtz0 = datamap_dgtz0.lower_bound(ts1);
            while(it_datamap_dgtz0!=datamap_dgtz0.end()&&it_datamap_dgtz0->first<ts2){
                corrts = (Long64_t) it_datamap_dgtz0->first;
                hlupodgtz1->Fill(corrts-ts);
                if (!flagend) datamap_dgtz0.erase(datamap_dgtz0.begin(),it_datamap_dgtz0);
                break;
            }
            flagfirst=0;
            break;
        }
        if (!flagend) datamap_lupo.erase(datamap_lupo.begin(),it_datamap_lupo);
    }
    */

    return 0;
}


void Init(){
    nlupo=0;
    ncorr0=0;
    ncorr1=0;
    ncorr2=0;
    ncorr3=0;

    c1=new TCanvas("c1","c1",900,700);
    hlupodgtz0=new TH1F("hlupodgtz0","hlupodgtz0",2000,-12000,12000);
    hlupodgtz1=new TH1F("hlupodgtz1","hlupodgtz1",2000,-12000,12000);
    hlupodgtz2=new TH1F("hlupodgtz2","hlupodgtz2",2000,-12000,12000);
    hlupodgtz3=new TH1F("hlupodgtz3","hlupodgtz3",2000,66048,86048);
    c1->Divide(2,2);
    c1->cd(1);
    hlupodgtz0->Draw();
    c1->cd(2);
    hlupodgtz1->Draw();
    c1->cd(3);
    hlupodgtz2->Draw();
    c1->cd(4);
    hlupodgtz3->Draw();
}

void ProcessEvent(NIGIRIHit* data){
  DoSort(false,data);
//  if (data->evt_type == TDC_EVENT_TYPE){
//    cout<<"lupots = "<<data->ts*LUPO_CLK_RES<<endl;
//  }
//  if (data->evt_type == V1740_EVENT_TYPE&&data->ch==0){
//    cout<<"dgtz "<<data->b<<" ts = "<<data->ts*DGTZ_CLK_RES<<endl;
//  }


}

void CloseMe(){
  //DoSort(true);
  hlupodgtz0->Reset();
  hlupodgtz1->Reset();
  hlupodgtz2->Reset();
  hlupodgtz3->Reset();
  cout<<"histograms reset!"<<endl;
}




