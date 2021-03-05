/***********************************************************************************
 >  *        File:   SyncCheck.cpp                                                 *
 >  * Description:   This file uses Midas Spy function to take data from a 	   *
 >		     DataSink in MIDAS format and look for timestamps coincidences *
 >		     from the tree sources.					   *
 >		     When find a single coincidence fill a 1D histogram,           *
 >		     when find a double coincidence fill a 2D histogram            *
 >		     For the embeded canvas is "inspired" in an example of         *
 >			embstat.C from  Ilka Antcheva (CERN/EP/SFT)                *
 >  *      Author:   Jorge Agramunt Ros  IFIC/CSIC spain                           *
 >  *Site Contact:   agramunt@ific.uv.es                                           *
 >  * Last Change:   24/02/2015                                                    *
 >  *******************************************************************************/


#include <TApplication.h>
//#include <TQApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TGStatusBar.h>
#include <TCanvas.h>
#include <string>
#include <TF1.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TRandom.h>
#include <RQ_OBJECT.h>
#include <TTimer.h>
#include <TRootContextMenu.h>
#include "SyncCheck.h"
#include "syncSpyClass.hpp"
#include<stdio.h>
#include<thread>
#include<vector>
#include <utility>      // std::pair
#include <map>
#include <algorithm>    // std::for_each
#include <mutex>    // std::for_each
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#define BUFFERSIZE 1024
        std::vector<int> X;
        std::vector<int> Y;
std::mutex mtx;           // mutex for critical section
#define LL1D -6e2
#define UL1D 6e2
#define LL2D -8e2
#define UL2D  5e2

#define Debug1
//#define Debug3
#define Debug2

int Master,NoMaster1,NoMaster2;
//extern "C"{
//#include"dataspy.h"
//}
std::mutex Spymtx[3];           // mutex for critical section

struct DataStruct{
uint32_t SourceId;
uint32_t DataId;
uint32_t E;
uint32_t PCtime;
uint32_t Aux1;
uint32_t Aux2;
uint32_t Aux3;
uint32_t Mark;
uint32_t TS;

};

void Help(char *progname) {
	std::cerr<<"SyncCheck  -M [Id of the master setup, 0,1,2 default 0]"<<std::endl;
    exit(1);
}


#define MAPTIME 800
void Spy(std::vector<uint64_t> & v,std::vector<DataStruct> & e,int & vid,const int id){
  SpyDataSink MyDataSink;
  if     (id==0)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110205",BUFFERSIZE);
  else if(id==1)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110206",BUFFERSIZE);
  else if(id==2)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110207",BUFFERSIZE);
  
  while(1)
  {
            
    int x=0,k=0;
    uint32_t buffer32 [BUFFERSIZE];

    memset(buffer32,0,BUFFERSIZE);
    std::cerr<<"kk en.. Id "<<std::dec<< id<<std::endl;
    int debn=0;
    bool HaveData=false;
    while (x==0)
    {
        if (id==0)x= MyDataSink.Spy( (char*)buffer32);
        else if (id==2)x= MyDataSink.Spy( (char*)buffer32);
        else if (id==1)x= MyDataSink.Spy( (char*)buffer32);
        int i;
        if(x>0)
        { 
            mtx.lock();
            std::cerr<<"kk2 en.. Id "<<std::dec<< id<<std::endl;
            vid=buffer32[6];
            Spymtx[id].lock();
            HaveData=true;
            for(i=0*4;i<BUFFERSIZE;i+=4)
            { 
                //std::cerr<<" 0x"<<std::hex<<buffer32[i];
                //		std::cerr<<"Id "<<std::dec<< id<<" buff0 0x"<<std::setw(8) << std::setfill('0')<<std::hex<<buffer32[i+0]<<" "<<"buff1 0x"<<std::setw(8) <<std::setfill('0')<<std::hex<<buffer32[i+1]<<" "<<"buff2 0x"<<std::setw(8) << std::setfill('0')<<std::hex<<buffer32[i+2]<<" "<<"buff3 0x"<<std::setw(8) << std::setfill('0')<<std::hex<<buffer32[i+3]<<std::dec<<" i "<<i<<std::endl;
                int dispcount=0;
                if(buffer32[i]!=0)
                {

                    struct DataStruct Data;
                    Data.PCtime=(uint32_t)time(NULL);
                    //                                       Data.E=buffer32[i+1];
                    Data.Mark=0;
                    uint64_t TimeStamp;
                    TimeStamp=buffer32[i+1];
                    TimeStamp=TimeStamp<<32;
                    TimeStamp+=buffer32[i];
                    Data.TS=TimeStamp;
                    v.push_back(TimeStamp);
                    e.push_back(Data);
					std::cerr<<"Id "<<std::dec<< id<<" buff0 0x"<<std::hex<<buffer32[i+0]<<" "
						<<"buff1 0x"<<std::hex<<buffer32[i+1]<<" "<<"buff2 0x"<<std::hex
						<<buffer32[i+2]<<" "<<"buff3 0x"<<std::hex<<buffer32[i+3]<<" "<<" 0x"<< TimeStamp <<std::endl;

#ifdef Debug2
//				        std::cerr<<"+----------------------------+Id "<<std::dec<< id<<" TimeStamp 0d"<<std::dec<<TimeStamp<<std::endl;
					//if(id==0)if(debn<100)std::cerr<<"Id "<<std::dec<< id<<" TimeStamp 0x"<<std::hex<<TimeStamp<<std::endl;
					debn++;
#endif


                }//if(buffer32[i]!=0)

            }//for(i=0*4;i<BUFFERSIZE;i+=4)
            std::cerr<<std::endl;
            Spymtx[id].unlock();
            usleep(1000);
        }
        else
        {
            usleep(100);
            k++;
        }

        if(k>5000)
            x=1; // Wait for 5 seconds for data
    }//while (x==0)
    mtx.unlock();
  }//while(1)
  return;
}

void Merge(std::vector<int> & X,std::vector<int> & Y)
{
    short buffer16 [32*1024];
    int buffer32 [16*1024];
    long long buffer64 [8*1024];
    time_t timer;
    int i,j,k;
    int x;
    int ID = 0;
    int d = 32;
    int v = 0;
    int DifCount=0;
    int NoDif=0;

    std::ofstream alvaroOut("alvaroOut.txt");

    std::map <uint64_t,DataStruct> MergeMap[3];
    std::vector<std::thread> SpyTh;
    int MyId;
    std::vector <uint64_t> TSvect[3];
    std::vector <DataStruct> Evect[3];
    int j2;
    int TimeMapLength=MAPTIME;
    for(j2=0;j2<3;j2++)
    {
            SpyTh.push_back(std::thread(Spy,std::ref(TSvect[j2]),std::ref(Evect[j2]),std::ref(MyId),j2));
    }

    while(1)
    {
        sleep(1);

        int NewTime=(int)time(NULL);
        //std::cerr<<"New Time...."<< NewTime <<std::endl;
        int j2;
        int vsize=0;
        for(j2=0;j2<3;j2++)
        {
            if(TSvect[j2].size()>0)
            {
                Spymtx[j2].lock();
                vsize+=TSvect[j2].size();
                for(k=0;k<TSvect[j2].size();k++)
                {
                    if(MergeMap[j2].find(TSvect[j2][k])!=MergeMap[j2].end())
                        std::cerr<< j2 << " Data already exist" <<std::endl;
                    MergeMap[j2].emplace(TSvect[j2][k],Evect[j2][k]);
                }
                Evect[j2].clear();
                TSvect[j2].clear();
                Spymtx[j2].unlock();
            }
        }//for(j2=0;j2<3;j2++)
        
        if(vsize>0)
        {
            int PrevId, NextId, PresentId;
            uint64_t NextTS , PrevTS , PresentTS ,PresentKey;
            for(j2=0;j2<3;j2++)
            {
                for(auto itx:MergeMap[j2])
                {
                    //	std::cout<< j2 <<" TS 0x "<<std::hex<<itx.first <<std::endl;
                    if(itx.second.PCtime<(NewTime-TimeMapLength))
                    {
                        // Erase elemnet too old
                        std::cout<< j2 <<" ------------------> Erase TS 0x"<<std::hex<<itx.first <<std::endl;
                        MergeMap[j2].erase(itx.first);
                    }
                    else if((itx.second.Mark&0xf)==1)
                    {
                        // Erase element already used
                        std::cout<< j2 <<" ------------------> Erase TS 0x"<<std::hex<<itx.first <<std::endl;
                        MergeMap[j2].erase(itx.first);
                    }
                }//for(auto itx:MergeMap[j2])
            }//for(j2=0;j2<3;j2++)

            int compare1,compare2;
            if(Master==0) {compare1=1;compare2=2;}
            else if(Master==1) {compare1=0;compare2=2;}
            else if(Master==2) {compare1=0;compare2=1;}	
            int Ncoinc1=0,Ncoinc2=0;
            
            int64_t lastTS(0);
            alvaroOut << "----------------"  << std::endl;
            for( auto & mit : MergeMap[2] )
            {
                
                alvaroOut << std::dec <<  int64_t(mit.first) - lastTS<<" mit.first " <<int64_t(mit.first) <<" lastTS "<< lastTS << std::endl;
                lastTS = mit.first ;
            }
            
            for(auto itx:MergeMap[Master])
            {
                Ncoinc1=0;Ncoinc2=0;
                auto itlow=MergeMap[compare1].lower_bound (itx.first+LL1D);  // itlow points to b
                auto itup =MergeMap[compare1].upper_bound (itx.first+UL1D);   // itup points to e (not d!)
                for (std::map<uint64_t,DataStruct>::iterator it=itlow; it!=itup; ++it)
                {
                    Ncoinc1++;
                    int Diff=(int) ((int64_t)(it->first)-(int64_t)itx.first);
                    std::cerr<<"-------------------------------Find Coinc 1 "<<std::dec<< Diff<< "Mark"
                    << it->second.Mark << std::hex <<" Main TS 0x"<<std::setw(11) << std::setfill('0')
                    <<itx.first << " 1 TS 0x"<<std::setw(11) << std::setfill('0') << it->first <<std::endl;
                    if((it->second.Mark&0xf)==0)
                    {
                        mtx.lock();
                        X.push_back(Diff);
                        Y.push_back(0xFFFF);
                        mtx.unlock();
                        it->second.Mark=it->second.Mark|0x00000001;        // Mark as 0xXXXX1
                    }//if((it->second.Mark&0xf)==0)
                }//for (std::map<uint64_t,DataStruct>::iterator it=itlow; it!=itup; ++it)
                
                itlow=MergeMap[compare2].lower_bound (itx.first+LL1D);  // itlow points to b
                itup =MergeMap[compare2].upper_bound (itx.first+UL1D);   // itup points to e (not d!)
                
                for (std::map<uint64_t,DataStruct>::iterator it=itlow; it!=itup; ++it)
                {
                    Ncoinc2++;
                    int Diff=(int) ((int64_t)(it->first)-(int64_t)itx.first);
                    std::cerr<<"-------------------------------Find Coinc 2 "<<std::dec<< Diff<< " Mark "<< it->second.Mark 
                    << std::hex <<" Main TS 0x"<<std::setw(11) << std::setfill('0')<<itx.first << " 2 TS 0x"<<std::setw(11) << std::setfill('0') << it->first <<std::endl;
                    if((it->second.Mark&0xf)==0)
                    {
                        mtx.lock();
                        Y.push_back(Diff);
                        X.push_back(0xFFFF);
                        mtx.unlock();
                        it->second.Mark=it->second.Mark|0x00000001;        // Mark as 0xXXXX1
                    }
                }//for (std::map<uint64_t,DataStruct>::iterator it=itlow; it!=itup; ++it)
            }//for(auto itx:MergeMap[Master])
            std::cerr<<"Coinc 1 "<< Ncoinc1 <<std::endl;
            std::cerr<<"Coinc 2 "<< Ncoinc2 <<std::endl;
            
        }//if(vsize>0)

    }//while(1)
    alvaroOut.close();
    return;
}



void MyMainFrame::AutoUpdate()
{
    mtx.lock();
    if(PlotUpdate->IsOn())
    {
        timer->Start(100,  kFALSE);   // 2 seconds single-shot
        fprintf(stderr,"Auto Update Start\n");
    }
    else
    {
        timer->Stop();
        fprintf(stderr,"Auto Update Stop\n");
    }
	X.clear();
	Y.clear();
    mtx.unlock();
    return;
}

void MyMainFrame::DoDraw()
{
    // Draw a function    
    Printf("Slot DoDraw()");
    Double_t x=0,y=0;
    h2->Fill(x,y);
    h2->Draw("COLZ");

    TCanvas *fCanvas = fEcan->GetCanvas();
    fCanvas->cd();
    fCanvas->Update();
}

void MyMainFrame::DoExit()
{
    Printf("Slot DoExit()");
    gApplication->Terminate(0);
}


// Widget methods here...
void MyMainFrame::SetStatusText(const char *txt, Int_t pi)
{
    // Set text in status bar.
    fStatusBar->SetText(txt,pi);
}

void MyMainFrame::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
    //  Writes the event status in the status bar parts
    //Printf("Slot EventInfo()");
    
    TCanvas *c = (TCanvas *) gTQSender;
    
    const char *text0, *text1, *text3;
    char text2[50];
    text0 = selected->GetTitle();
    SetStatusText(text0,0);
    text1 = selected->GetName();
    SetStatusText(text1,1);
    if (event == kKeyPress)
        sprintf(text2, "%c", (char) px);
    else
        sprintf(text2, "%d,%d", px, py);
    SetStatusText(text2,2);
    text3 = selected->GetObjectInfo(px,py);
    SetStatusText(text3,3);
    if (event == kButton1Down){
        printf("event = %d, px=%d, py=%d, obj = %s::%s , kk %s\n", event,px,py,
               selected->GetName(), selected->GetTitle(),text3);
    }
    
}
void MyMainFrame::SelectPixel(Int_t event, Int_t x, Int_t y, TObject *selected){
    // Handle mouse click events in the embedded canvas.
    
    //    const char *text0, *text1, *text3;
    //    if (event == kButton1Down){
    //        text3 = selected->GetObjectInfo(x,y);
    //        printf("event = %d, px=%d, py=%d, obj = %s::%s , kk %s\n", event,x,y,
    //               selected->GetName(), selected->GetTitle(),text3);
    //    }
}

void MyMainFrame::ClearDone(){
	 h2partial->Reset("ICES");
}
void MyMainFrame::Reset(){
 	h2partial->Reset("ICES");
 	h2->Reset("ICES");
 	h1X->Reset("ICES");
 	h1Y->Reset("ICES");
}
void MyMainFrame::TimerDone()
{
    // Draw a function
   int k3;
//    Printf("Slot DoDraw()");

    if(X.size()>0)
    {
        mtx.lock();
        int i;
        for(i=0;i<X.size();i++)
        {
            if(X[i]>LL1D && X[i]<UL1D && X[i]!=0xFFFF )
            {
                if(X[i]!=0)h1X->Fill(X[i]);
                std::cerr <<X[i] << '\n';
            }
        }
        mtx.unlock();
    }
    
    if(Y.size()>0)
    {
        mtx.lock();
        int i;
        for(i=0;i<Y.size();i++)
        {
            if(Y[i]>LL1D && Y[i]<UL1D && Y[i]!=0xFFFF )
            {
                if(Y[i]!=0)
                {
                    h1Y->Fill(Y[i]);
                }
            }
        }
        mtx.unlock();
    }


    if(X.size()>0&&Y.size()>0)
    {
        mtx.lock();
        int i;
        int ls=X.size();
        if(X.size()>Y.size())
            ls=Y.size();
        for(i=0;i<X.size();i++)
        {
            if(X[i]==0xFFFF) X[i]=0;
                    if(Y[i]==0xFFFF) Y[i]=0;
                    if((X[i]!=0)&&(Y[i]!=0) ) h2->Fill(X[i],Y[i]);
                    if((X[i]!=0)&&(Y[i]!=0) ) h2partial->Fill(X[i],Y[i]);
        }
        mtx.unlock();
    }
    
	if(X.size()>0) X.clear(); 
    if(Y.size()>0) Y.clear(); 
 
    TCanvas *fCanvas = fEcan->GetCanvas();
    fCanvas->cd(1)->cd(1);

        h2->Draw("COLZ");
    fCanvas->cd(1)->cd(2);

        h2partial->Draw("COLZ");

	
    fCanvas->cd(2)->cd(1);
	        h1X->Draw();

    fCanvas->cd(2)->cd(2);
                h1Y->Draw();

    fCanvas->Update();
}

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{
    // Create the main frame
    fMain = new TGMainFrame(p,w,h);
    
    // Create the embedded canvas
    fEcan = new TRootEmbeddedCanvas(0,fMain,1200,800);
    Int_t wid = fEcan->GetCanvasWindowId();
    TCanvas *myc = new TCanvas("MyCanvas", 500,400,wid);
	myc->Divide(1,2);
	myc->cd(1)->Divide(2,1);

	myc->cd(2)->Divide(2,1);
    fEcan->AdoptCanvas(myc);
    myc->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","MyMainFrame",this,
                 "EventInfo(Int_t,Int_t,Int_t,TObject*)");
    
    myc->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
                 "MyMainFrame", this,
                 "SelectPixel(Int_t,Int_t,Int_t,TObject*)");
    
    
    timer = new TTimer();
    timer->Connect("Timeout()", "MyMainFrame",
                   this, "TimerDone()");
    timer2 = new TTimer();
    timer2->Connect("Timeout()", "MyMainFrame",
                   this, "ClearDone()");

    double ll1d= LL1D;
    double ul1d= UL1D;
    double ll2d= LL2D;
    double ul2d= UL2D;


   	if(Master==0){ 
    		h2=new TH2F("Check","Synchronization Check;RIBF-DTAS time difference        x40ns;AIDA/WASABI-DTAS time difference                  x40ns",80, ll2d, ul2d, 80, ll2d, ul2d); 
    		h2partial=new TH2F("CheckPartial","Partial;RBIF-DTAS time difference        x40ns;AIDA/WASABI-DTAS time difference                  x40ns",80, ll2d, ul2d, 80, ll2d, ul2d);

    		h1X=new TH1F("Check","RIBF-DTAS time difference;                                x40ns",20000, ll1d, ul1d);
    		h1Y=new TH1F("Check","AIDA/WASABI-DTAS time difference;                                x40ns",20000, ll1d, ul1d);
	}
	        else if(Master==1){
                h2=new TH2F("Check","Synchronization Check;AIDA-RIBF time difference        x40ns;DTAS-RIBF time difference                  x40ns",200, ll2d, ul2d, 200, ll2d, ul2d);
                h2partial=new TH2F("CheckPartial","Partial;AIDA-RIBF time difference        x40ns;DTAS-RIBF time difference                  x40ns",200, ll2d, ul2d, 200, ll2d, ul2d);

                h1X=new TH1F("Check","AIDA-RIBF time difference;                                x40ns",2000, ll1d, ul1d);
                h1Y=new TH1F("Check","DTAS-RIBF time difference;                                x40ns",20000, ll1d, ul1d);
        }
        else if(Master==2){
                h2=new TH2F("Check","Synchronization Check;RIBF-AIDA  time difference        x40ns;DTAS-AIDA  time difference                  x40ns",200, ll2d, ul2d, 200, ll2d, ul2d);
                h2partial=new TH2F("CheckPartial","Partial;RIBF-AIDA  time difference        x40ns;DTAS-AIDA  time difference                  x40ns",200, ll2d, ul2d, 200, ll2d, ul2d);

                h1X=new TH1F("Check","BRIKEN-AIDA time difference;                                x40ns",40000, ll1d, ul1d);
                h1Y=new TH1F("Check","RIBF-AIDA time difference;                                x40ns",40000, ll1d, ul1d);
        }

    gStyle->SetOptStat(1110111);

   fMain->AddFrame(fEcan, new TGLayoutHints(kLHintsTop | kLHintsLeft
                                             | kLHintsExpandX  | kLHintsExpandY,0,0,1,1));
    // status bar
    Int_t parts[] = {45, 15, 10, 30};
    fStatusBar = new TGStatusBar(fMain, 50, 10, kVerticalFrame);
    fStatusBar->SetParts(parts, 4);
    fMain->AddFrame(fStatusBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));
    
    // Create a horizontal frame containing two buttons
    TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, 200, 40);
    
 //   PlotUpdate = new TGCheckButton(hframe, "Auto Update",1);
    //     AutoUpdate->SetState(kButtonUp);
//    PlotUpdate->Connect("Clicked()", "MyMainFrame", this, "AutoUpdate( )");
//    hframe->AddFrame(PlotUpdate, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
    
    
 //   TGTextButton *draw = new TGTextButton(hframe, "&Draw");
 //   draw->Connect("Clicked()", "MyMainFrame", this, "DoDraw()");
 //   hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
 
    TGTextButton *reset = new TGTextButton(hframe, "&Reset");
    reset->Connect("Clicked()", "MyMainFrame", this, "Reset()");
    hframe->AddFrame(reset, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
 
   
    TGTextButton *exit = new TGTextButton(hframe, "&Exit ");
    exit->Connect("Pressed()", "MyMainFrame", this, "DoExit()");
    hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

//	std::thread(GetData,std::ref(X),std::ref(Y));   

    
    fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
    
    // Set a name to the main frame
    fMain->SetWindowName("Embedded Canvas Status Info");
    fMain->MapSubwindows();
          timer->Start(100,  kFALSE);   // 1 seconds single-shot
          timer2->Start(100000,  kFALSE);   // 1000 seconds single-shot
  
    // Initialize the layout algorithm via Resize()
    fMain->Resize(fMain->GetDefaultSize());
    
    // Map main frame
    fMain->MapWindow();
    
}


MyMainFrame::~MyMainFrame()
{
    // Clean up main frame...
    fMain->Cleanup();
    delete fEcan;
    delete fStatusBar;
    delete fMain;
}


void embstat()
{
    // Popup the GUI...
    new MyMainFrame(gClient->GetRoot(), 200, 200);
}

int main(int argc, char **argv) 
{
    Master=0;
    NoMaster1=1;
    NoMaster2=2;
	int i;
    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(argv[i][1]) {
                    case 'M':
                        Master = atoi(argv[++i]);
                        break;
                    default:
                        Help(argv[0]);
                        break;
                }//end switch
            }//end if '-'
        }
    }//end if opt
    
	if(Master==1){
		NoMaster1=2;
		NoMaster2=0;
	}
	else if (Master==2){
                NoMaster1=0;
                NoMaster2=1;
    }
    
	if(Master>2) 
        Help(argv[0]);

    std::cerr<< "Master "<< Master << std::endl;
    std::cerr<< "NoMaster1 "<< NoMaster1 << std::endl;
    std::cerr<< "NoMaster2 "<< NoMaster2 << std::endl;


    std::thread InputData(Merge,std::ref(X),std::ref(Y));   

    TApplication theApp("App",&argc,argv);
    embstat();
    theApp.Run();
    return 0;
}

