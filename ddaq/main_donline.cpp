//#include <QCoreApplication>
#include "daq.h"
#include "keyb.h"

#define num_event_draw 5;

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
#include "TSocket.h"
#include "TPServerSocket.h"

#include "randcircle.h"
#include "TRandom.h"
#include "TMath.h"
#include "TCanvas.h"
#include "Riostream.h"
#include "TH2.h"
#include "TH2F.h"
#include "TEllipse.h"
#include "TColor.h"
#include "TROOT.h"
//#include "randcircle.cpp"

#define online_evt_wf 1
#define online_evt 5



void genRndCircle(Double_t &x,Double_t &y,Double_t a,Double_t b,Double_t xpos,Double_t ypos,Double_t R){
    if (b<a){
        Double_t temp=a;
        a=b;
        b=temp;
    }
    x=xpos+b*R*TMath::Cos(2*TMath::Pi()*a/b);
    y=ypos+b*R*TMath::Sin(2*TMath::Pi()*a/b);
}
void genRndRec(Double_t &x,Double_t &y,Double_t a,Double_t b){
    x=-25+a*50;
    y=-25+b*50;
}
void drawCircleHist(TH2D* his,Double_t xpos,Double_t ypos,Double_t R,Int_t npoint,Double_t weight){
    Double_t dphi=2*TMath::Pi()/(Double_t)npoint;
    for (Int_t i=0;i<npoint;i++){
        Double_t x,y;
        x=xpos+R*TMath::Cos(dphi*i);
        y=ypos+R*TMath::Sin(dphi*i);
        his->Fill(x,y,weight);
    }
}

void randCircle()
{
    TCanvas* c1=new TCanvas("c1","c1",900,900);
    c1->cd();

    TH2D* h2=new TH2D("h2","h2",1000,-450,450,1000,-450,450);
    h2->GetXaxis()->SetRangeUser(-450,450);
    h2->GetYaxis()->SetRangeUser(-450,450);
    h2->Draw();
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

    TEllipse* holes[172];
    for (Int_t i=0;i<172;i++){
        holes[i]=new TEllipse(xpos[i],ypos[i],diameter[i]/2);
        holes[i]->SetLineColor(2);
        holes[i]->SetLineWidth(2);
        //holes[i]->Draw("same");
        drawCircleHist(h2,xpos[i],ypos[i],diameter[i]/2,200,1000);
    }

    TRandom rr;
    for (Int_t i=0;i<100;i++){
        Double_t a,b;
        a=rr.Rndm();
        b=rr.Rndm();
        Double_t x,y;
        genRndCircle(x,y,a,b,xpos[100],ypos[100],diameter[100]/2);
        h2->Fill(x,y);
        a=rr.Rndm();
        b=rr.Rndm();
        genRndRec(x,y,a,b);
        h2->Fill(x,y);
    }
    h2->Draw("colz same");
}

int main(int argc, char *argv[])
{
    int refesh_rate=online_evt;
    int refesh_rate_ch2=online_evt;
    if (argc==3){
        refesh_rate=atoi(argv[1]);
        refesh_rate_ch2=atoi(argv[2]);
    }else{
        printf("Please enter refesh rates\n");
        return 0;
    }
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


    TCanvas *c1 = new TCanvas("c1","ADC-QDC data",1000,1000);
    TCanvas *c2 = new TCanvas("c2","Waveform_data",1000,400);
    TCanvas *c3 = new TCanvas("c3","BRIKEN",900,900);

    TH1F *ch1_E_QDC = new TH1F("ch1_E_QDC","channel 1 QDC",200,100,50000);
    TH1F *ch1_E_ADC = new TH1F("ch1_E_ADC","channel 1 ADC",200,100,10000);
    TH1I *ch1_wf = new TH1I("ch1_wf","Waveform of channel 1",record_length,0,record_length);
    TH1F *ch2_E_QDC = new TH1F("ch2_E_QDC","channel 2 QDC",200,100,50000);
    TH1F *ch2_E_ADC = new TH1F("ch2_E_ADC","channel 2 ADC",200,100,10000);
    TH1I *ch2_wf = new TH1I("ch2_wf","Waveform of channel 2",record_length,0,record_length);

    //BRIKEN histogram
    Int_t ch2pos=166;
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

    //draw adc window
    c1->Divide(2,2);
    c1->Draw();
    c1->cd(1);
    ch1_E_ADC->Draw("hist");
    c1->cd(2);
    ch1_E_QDC->Draw("hist");
    c1->Update();
    c1->cd(3);
    ch2_E_ADC->Draw("hist");
    c1->cd(4);
    ch2_E_QDC->Draw("hist");
    c1->Update();
    //draw wf window
    c2->Divide(2,1);
    c2->Draw();
    c2->cd(1);
    ch1_wf->Draw("hist");
    c2->cd(2);    
    ch2_wf->Draw("hist");
    c2->Update();    
    c3->Draw();
    c3->cd();
    briken->Draw("colz");
    c3->Update();

    Int_t ch1_nevt=0;
    Int_t ch2_nevt=0;
    Int_t cnt_fail_ch1=0;
    Int_t cnt_fail_ch2=0;

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
                 ch1_E_ADC->Reset();
                 ch1_E_QDC->Reset();
                 ch2_E_ADC->Reset();
                 ch2_E_QDC->Reset();
                 ch1_nevt=0;
                 ch2_nevt=0;
                 cnt_fail_ch1=0;
                 cnt_fail_ch2=0;
		 briken->Reset();
		 for (Int_t i=0;i<172;i++){
		   drawCircleHist(briken,xpos[i],ypos[i],diameter[i]/2,200,100);
		 }
   
              }

        }
        if (daq_t.shmp->ana_flag==1){
            if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[1]==0){
                //fill adc
                //dpp processing here
                //if (daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0]>0){
                    dpp *oj=new dpp(record_length,daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata);
                    oj->baselineMean(16,100);
                    //oj->bL=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0];
                    ch1_E_ADC->Fill((Double_t) oj->bL-oj->minAdcNeg(record_length));
                    delete oj;
                //}
                //fill qdc
                ch1_E_QDC->Fill((Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]);

                //fill briken
                if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]>1000){
                    Double_t a,b,x,y;
                    a=rr.Rndm();
                    b=rr.Rndm();
                    genRndRec(x,y,a,b);
                    briken->Fill(x,y);
                }


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
                    c2->cd(1);
                    ch1_wf->Draw("hist");
                    c2->Update();

                    c1->cd(1);
                    ch1_E_ADC->Draw("hist");
                    c1->Update();
                    c1->cd(2);
                    ch1_E_QDC->Draw("hist");
                    c1->Update();
                    c3->cd();
                    briken->Draw("colz");
                    c3->Update();

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
                    oj->baselineMean(16,100);
                    //oj->bL=daq_t.shmp->buffer[daq_t.shmp->block_no].wfdata[0];
                    ch2_E_ADC->Fill((Double_t) oj->bL-oj->minAdcNeg(record_length));
                    delete oj;
                //}
                //fill qdc
                ch2_E_QDC->Fill((Double_t) daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]);

                //fill briken
                if (daq_t.shmp->buffer[daq_t.shmp->block_no].dgtzdata[6]>1000){
                    Double_t a,b,x,y;
                    a=rr.Rndm();
                    b=rr.Rndm();
                    genRndCircle(x,y,a,b,xpos[ch2pos],ypos[ch2pos],diameter[ch2pos]/2);
                    briken->Fill(x,y);
                }

                if (ch2_nevt%refesh_rate_ch2==0&&ch2_nevt>0) {
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
                    c2->cd(2);
                    ch2_wf->Draw("hist");
                    c2->Update();
                    c1->cd(3);
                    ch2_E_ADC->Draw("hist");
                    c1->cd(4);
                    ch2_E_QDC->Draw("hist");
                    c1->Update();

                }
                ch2_nevt++;
            }
            daq_t.shmp->ana_flag=0;
        }

    };

    theApp.Run();
    return 0;
}

