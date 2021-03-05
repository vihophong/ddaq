#include "TSocket.h"
#include "TPServerSocket.h"
#include <TApplication.h>
#include"TFile.h"
#include"TTree.h"
#include"TROOT.h"
#include"TString.h"
#include"TRandom.h"
#include"TH1.h"
#include"TH2.h"
#include"TF1.h"
#include"TGraph.h"
#include"TCanvas.h"
#include<TSystem.h>
#include<TStyle.h>

#include "../ddaq/daq.h"
#include "../ddaq/keyb.h"
#include "../ddaq/dpp.h"
#include "../ddaq/mysignalhandler.h"

#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TSystem.h"
#include "THttpServer.h"
#include "randCircle.cpp"

Bool_t bFillHist = kTRUE;

#define online_evt 5000

int main(int argc, char *argv[])
{
    int port=1212;
    int port_http=9090;
    int refesh_rate=online_evt;    
    if (argc>1){
        port=atoi(argv[1]);        
        if (argc==3) {
            refesh_rate=atoi(argv[2]);            
        }
    }else{
        printf("Please enter port number for http service!\n");
        return 0;
    }

    /*

    //create socket and sent it to "client" (actually data server)
    TServerSocket *ss = new TServerSocket (port, kTRUE);
    TSocket *socket;
    socket = ss->Accept();
    socket->Send("go");
    ss->Close();


    //receiving first data from "client" and get record length
    daq daq_t;
    daq_t.shmp=new SHM_DGTZ;//important// dont forget to delete it!
    socket->RecvRaw(daq_t.shmp,sizeof(SHM_DGTZ_S));
    uint32_t record_length=daq_t.shmp->recordLength[0];
    */

    //*****************
    // Setup histogram and graph
    //*****************

    daq daq_t;
    if ((daq_t.shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1){
        daq_t.shmp = (struct SHM_DGTZ_S *)shmat(daq_t.shmid, 0, 0);
    }else{
        printf("Can not attach to share memory!!!!!\n");
    }
    uint32_t record_length=daq_t.shmp->recordLength[0];// record length for board 0
    daq_t.shmp->ana_status=1;


    //BRIKEN histogram
    Int_t ch2pos=100;
    TRandom rr;
    TH2D* briken=new TH2D("briken","briken",500,-450,450,500,-450,450);
    briken->GetXaxis()->SetRangeUser(-450,450);
    briken->GetYaxis()->SetRangeUser(-450,450);
    briken->SetDirectory(0);
    std::ifstream inf("briken_mapping.txt",std::ios::in);
    Double_t xpos[172];
    Double_t ypos[172];
    Double_t diameter[172];
    Int_t ch;
    while (inf.good()){
        inf>>ch;
        inf>>xpos[ch]>>ypos[ch]>>diameter[ch];
        cout<<ch<<"-"<<xpos[ch]<<endl;
    }
    for (Int_t i=0;i<172;i++){
        drawCircleHist(briken,xpos[i],ypos[i],diameter[i]/2,200,100);
    }


    TH1F *ch1_E_QDC = new TH1F("ch1_E_QDC","channel 1 QDC",200,100,50000);
    TH1F *ch1_E_ADC = new TH1F("ch1_E_ADC","channel 1 ADC",200,100,10000);
    TH1I *ch1_wf = new TH1I("ch1_wf","Waveform of channel 1",record_length,0,record_length);
    TH1F *ch2_E_QDC = new TH1F("ch2_E_QDC","channel 2 QDC",200,100,50000);
    TH1F *ch2_E_ADC = new TH1F("ch2_E_ADC","channel 2 ADC",200,100,3000);
    TH1I *ch2_wf = new TH1I("ch2_wf","Waveform of channel 2",record_length,0,record_length);
    ch1_E_QDC->SetDirectory(0);
    ch1_E_ADC->SetDirectory(0);
    ch1_wf->SetDirectory(0);
    ch2_E_QDC->SetDirectory(0);
    ch2_E_ADC->SetDirectory(0);
    ch2_wf->SetDirectory(0);


    /*
    for (uint32_t i=0;i<record_length;i++){
        ch1_wf->SetBinContent(i+1,0);
    }
    */

    //*****************
    //  Setup http server
    //*****************
    // start http server

    THttpServer* serv = new THttpServer(Form("http:%d",port_http));
    serv->SetTopName("DDAQ");
    // One could specify location of newer version of JSROOT
    // serv->SetJSROOT("https://root.cern.ch/js/3.3/");
    // serv->SetJSROOT("http://web-docs.gsi.de/~linev/js/3.3/");

    //fake hist
    TH1I* fk=new TH1I("fk","fkk",1,0,1);
    fk->SetBinContent(1,100);
    fk->SetDirectory(0);
    serv->Register("/", fk);


    // register histograms and associated command
    //serv->Register("/", briken);
    serv->Register("/", ch1_E_QDC);
    serv->Register("/", ch1_E_ADC);
    serv->Register("/", ch1_wf);
    serv->Register("/", ch2_E_QDC);
    serv->Register("/", ch2_E_ADC);
    serv->Register("/", ch2_wf);

    // register commands, invoking object methods for histogram
    // one could set command properties directly
    serv->RegisterCommand("/Resetch1_E_QDC","/ch1_E_QDC/->Reset()");

    serv->SetIcon("/Resetch1_E_QDC", "/rootsys/icons/ed_delete.png");
    serv->SetItemField("/Resetch1_E_QDC","_fastcmd", "true");
    serv->RegisterCommand("/Resetch1_E_ADC","/ch1_E_ADC/->Reset()");
    serv->SetIcon("/Resetch1_E_ADC", "/rootsys/icons/bld_stop.png");
    serv->SetItemField("/Resetch1_E_ADC","_fastcmd", "true");
    serv->RegisterCommand("/Resetch1_wf","/ch1_wf/->Reset()");
    serv->SetIcon("/Resetch1_wf", "/rootsys/icons/bld_delete.png");
    serv->SetItemField("/Resetch1_wf", "_fastcmd", "true");


    // enable monitoring and
    // specify item to draw when page is opened
    serv->SetItemField("/","_monitoring","2000");
    serv->SetItemField("/","_drawitem","ch2_E_QDC");
    serv->SetItemField("/","_drawopt","");

    // register simple start/stop commands

    serv->RegisterCommand("/Start", "/fk/->SetBinContent(1,100)");
    serv->SetIcon("/Start", "/rootsys/icons/ed_execute.png");
    serv->SetItemField("/Start","_fastcmd", "true");
    serv->RegisterCommand("/Stop", "/fk/->Reset()");
    serv->SetIcon("/Stop", "/rootsys/icons/ed_interrupt.png");
    serv->SetItemField("/Stop","_fastcmd", "true");

    // one could hide commands and let them appear only as buttons
    serv->Hide("/Start");
    serv->Hide("/Stop");
    serv->Hide("/fk");


    const Long_t kUPDATE = (Long_t) refesh_rate; //refesh rate

    //*****************
    //  Loop
    //*****************
    Long_t cnt = 0;
    Int_t ch1_nevt=0;
    Int_t ch2_nevt=0;
    Int_t cnt_fail_ch1=0;
    Int_t cnt_fail_ch2=0;
    while(kTRUE)
    {
        //get data sent by "client"        
        //-------------
        //filling histogram and update graph
        if (fk->GetBinContent(1)>0) {
            if (daq_t.shmp->ana_flag==1){
                if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[1]==0){
                    //fill adc
                    //dpp processing here
                    //if (daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0]>0){
                        dpp *oj=new dpp(record_length,daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata);
                        //oj->baselineMean(16,100);
                        oj->bL=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0];
                        ch1_E_ADC->Fill((Double_t) oj->bL-oj->minAdcNeg(record_length));                        
                        delete oj;
                    //}
                    //fill qdc
                    ch1_E_QDC->Fill((Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]);
                    /*
                    //fill briken
                    if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]>1000){
                        Double_t a,b,x,y;
                        a=rr.Rndm();
                        b=rr.Rndm();
                        genRndRec(x,y,a,b);
                        briken->Fill(x,y);
                    }
                    */

                    if (ch1_nevt%refesh_rate==0&&ch1_nevt>0) {
                        if (daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0]>0){
                            //falt bits of wf
                            int f_cnt=0;
                            for (uint32_t i=0;i<record_length;i++){
                                Int_t ypoint=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i];
                                if (ypoint<=1000&&daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i+1]==0&&i<record_length-1) f_cnt++;
                                ch1_wf->SetBinContent(i+1,ypoint);
                            }
                            if (f_cnt>0) cnt_fail_ch1++;
                        }

                        //Int_t lossrate=(Int_t)((int)(daq_t.shmp->TrgCnt[0][0]-offlineCntBegin_ch1)-ch1_nevt);                        
                        printf("CH1- Number of event=%i - online event=%i - fail wf even=%i\n",(uint32_t) daq_t.shmp->TrgCnt[0][0],ch1_nevt,cnt_fail_ch1);
                        printf("CH2- Number of event=%i - online event=%i - fail wf even=%i\n",(uint32_t) daq_t.shmp->TrgCnt[0][1],ch2_nevt,cnt_fail_ch2);
                        printf("\n");
                    }
                    ch1_nevt++;
                }
                else if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[1]==1){
                    //fill adc
                    //dpp processing here                    
                    //if (daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0]>0){
                        dpp *oj=new dpp(record_length,daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata);
                        //oj->baselineMean(16,100);
                        oj->bL=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0];
                        ch2_E_ADC->Fill((Double_t) oj->bL-oj->minAdcNeg(record_length));
                        delete oj;
                    //}
                    //fill qdc
                    ch2_E_QDC->Fill((Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]);
                    /*
                    //fill briken
                    if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]>1000){
                        Double_t a,b,x,y;
                        a=rr.Rndm();
                        b=rr.Rndm();
                        genRndCircle(x,y,a,b,xpos[ch2pos],ypos[ch2pos],diameter[ch2pos]/2);
                        briken->Fill(x,y);
                    }
                    */
                    if (ch2_nevt%refesh_rate==0&&ch2_nevt>0) {
                        if (daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0]>0){
                            //falt bits of wf
                            int f_cnt=0;
                            for (uint32_t i=0;i<record_length;i++){
                                Int_t ypoint=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i];
                                if (ypoint<=1000&&daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i+1]==0&&i<record_length-1) f_cnt++;
                                ch2_wf->SetBinContent(i+1,ypoint);
                            }
                            if (f_cnt>0) cnt_fail_ch2++;
                        }
                    }
                    ch2_nevt++;
                }
                daq_t.shmp->ana_flag=0;
            }
        cnt++;
        }//fill hist start online

        if ((ch1_nevt % kUPDATE==0) || !(fk->GetBinContent(1)>0)) {
           // IMPORTANT: one should regularly call ProcessEvents
           // to let http server process requests
           if (gSystem->ProcessEvents()) break;
        }

    }    


}

