//#include <QCoreApplication>
#include "daq.h"
#include "keyb.h"

#define num_event_draw 10000;

#include <TApplication.h>
#include"TFile.h"
#include"TTree.h"
#include"TROOT.h"
#include"TString.h"
#include"TRandom.h"
#include"TH1.h"
#include"TH2.h"
#include"TGraph.h"
#include"TCanvas.h"
#include<TSystem.h>
#include<TStyle.h>

#include "mysignalhandler.h"

#define online_evt_wf 100
#define online_evt 200

int main(int argc, char *argv[])
{
    TApplication theApp("app",&argc,argv);
    gSystem->AddSignalHandler( new mysignalhandler( kSigInterrupt ) );
    gSystem->AddSignalHandler( new mysignalhandler( kSigTermination ) );

    daq daq_t;
    if ((daq_t.shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1){
        daq_t.shmp = (struct SHM_DGTZ_S *)shmat(daq_t.shmid, 0, 0);
    }else{
        printf("Can not attach to share memory!!!!!\n");
    }
    uint32_t record_length=daq_t.shmp->recordLength[0];// record length for board 0
    daq_t.shmp->ana_status=1;


    TCanvas *c1 = new TCanvas("c1","ADC data",0,10,1000,400);
    TCanvas *c2 = new TCanvas("c2","Waveform_data",0,10,1000,400);

    TH1F *h5 = new TH1F("h5","h5",400,100,50000);
    //shortgate
    TF1 *fa1 = new TF1("fa1",Form("(x>%d)*(x<%d)*2000",daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][0],daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][0]+daq_t.shmp->shortGate[0][0]),0,record_length);
    fa1->GetYaxis()->SetRangeUser(-1000,pow(2,14));
    fa1->SetLineWidth(0.5);
    //longgate
    TF1 *fa2 = new TF1("fa2",Form("(x>%d)*(x<%d)*5000",daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][0],daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][0]+daq_t.shmp->longGate[0][0]),0,record_length);
    fa2->GetYaxis()->SetRangeUser(-1000,pow(2,16));
    fa2->SetLineWidth(0.5);
    fa2->SetLineColor(3);
    //waveform
    Double_t x[record_length];
    Double_t y[record_length];
    for (uint32_t i=0;i<record_length;i++){
        x[i]=(Double_t)i;
        y[i]=0;
    }
    TGraph* grwf=new TGraph(record_length,x,y);

    TH1F *h5_ch2 = new TH1F("h5_ch2","h5_ch2",400,100,50000);
    //shortgate
    TF1 *fa1_ch2 = new TF1("fa1_ch2",Form("(x>%d)*(x<%d)*2000",daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][1],daq_t.shmp->preTrg[0][1]-daq_t.shmp->preGate[0][1]+daq_t.shmp->shortGate[0][1]),0,record_length);
    fa1_ch2->GetYaxis()->SetRangeUser(-1000,pow(2,14));
    fa1_ch2->SetLineWidth(0.5);
    //longgate
    TF1 *fa2_ch2 = new TF1("fa2_ch2",Form("(x>%d)*(x<%d)*5000",daq_t.shmp->preTrg[0][0]-daq_t.shmp->preGate[0][1],daq_t.shmp->preTrg[0][1]-daq_t.shmp->preGate[0][1]+daq_t.shmp->longGate[0][1]),0,record_length);
    fa2_ch2->GetYaxis()->SetRangeUser(-1000,pow(2,16));
    fa2_ch2->SetLineWidth(0.5);
    fa2_ch2->SetLineColor(3);
    //waveform
    Double_t x2[record_length];
    Double_t y2[record_length];
    for (uint32_t i=0;i<record_length;i++){
        x2[i]=(Double_t)i;
        y2[i]=0;

    }
    TGraph* grwf_ch2=new TGraph(record_length,x2,y2);

    //draw adc window
    c1->Divide(2,1);
    c1->Draw();
    c1->cd(1);
    h5->Draw();
    c1->cd(2);
    h5_ch2->Draw();
    c1->Update();
    //draw wf window
    c2->Divide(2,1);
    c2->Draw();
    c2->cd(1);
    fa1->Draw();
    fa2->Draw("same");
    c2->cd(2);
    fa1_ch2->Draw();
    fa2_ch2->Draw("same");
    c2->Update();

    Double_t adcLong;
    int evtn=0;
    int evtn_ch2=0;
    int fail_bit_flag=0;
    while(1){//1st while
        //Check keyboard
        if(kbhit()==1) {
               int if_continue;
               std::cout<<"online monitor is paused"<<std::endl;
               std::cout<<"Press 0 <Enter> to stop, 1 <Enter> to restart"<<std::endl;
               std::cout<<"Press 2 <Enter> to restart after clear"<<std::endl;
               std::cin >> if_continue;
               TString choose;
               if(if_continue==0){
                  choose="stop";
               }else if(if_continue==1){
                  choose="restart";
               }else if(if_continue==2){
                  choose="restart after reset";
               }
               std::cout<<"You choose "<<choose<<", please press ctrl+c to continue;"<<std::endl;
              theApp.Run();
              if(if_continue == 0){
                 //keeploop = false;
                 break;
              }else if(if_continue==1){
                 printf("monitor restarted, press <Space> to pause;\n");
              }else if(if_continue==2){
                 printf("monitor restarted, press <Space> to pause;\n");
                 //tree->Reset();
                 h5->Reset();
                 h5_ch2->Reset();
                 //h5->Reset();
                 //h6->Reset();
                 evtn=0;
                 evtn_ch2=0;
              }

        }

        if (daq_t.shmp->ana_flag==1){
            if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[0]==0&&daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[1]==0){ //select channel,board
                adcLong=(Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6];
                h5->Fill(adcLong);

                if (evtn%online_evt_wf==0&&evtn>0) {
                    for(uint32_t i=0;i<record_length;i++){
                        Double_t ypoint=(Double_t)daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i];
                        if (ypoint>pow(2,14)) fail_bit_flag=1;//check fail bit
                        grwf->SetPoint(i,(Double_t)i,ypoint);
                    }
                    if (fail_bit_flag==0){
                        c2->cd(1);
                        grwf->Draw("same");
                        c2->Update();
                    }
                    fail_bit_flag=0;
                }
                if (evtn%online_evt==0&&evtn>0) {
                    c1->cd(1);
                    h5->Draw("hist");
                    c1->Update();
                    printf("CH1- Number of event=%i - online event=%i\n",(uint32_t) daq_t.shmp->TrgCnt[0][0],evtn);
                    //printf("%i\n",daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[10]);
                }
                evtn++;
                daq_t.shmp->ana_flag=0;
            }
            else if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[0]==0&&daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[1]==1){ //select channel,board
                adcLong=(Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6];
                h5_ch2->Fill(adcLong);

                if (evtn_ch2%online_evt_wf==0&&evtn_ch2>0) {
                    for(uint32_t i=0;i<record_length;i++){
                        Double_t ypoint=(Double_t)daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[i];
                        if (ypoint>pow(2,14)) fail_bit_flag=1;//check fail bit
                        grwf_ch2->SetPoint(i,(Double_t)i,ypoint);
                    }
                    if (fail_bit_flag==0){
                        c2->cd(2);
                        grwf_ch2->Draw("same");
                        c2->Update();
                    }
                    fail_bit_flag=0;
                }
                if (evtn_ch2%online_evt==0&&evtn_ch2>0) {
                    c1->cd(2);
                    h5_ch2->Draw("hist");
                    c1->Update();
                    printf("CH2- Number of event=%i - online event=%i\n",(uint32_t) daq_t.shmp->TrgCnt[0][1],evtn_ch2);
                    //printf("%i\n",daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[10]);
                }
                evtn_ch2++;
                daq_t.shmp->ana_flag=0;
            }

        }
    };

    theApp.Run();
    return 0;
}

