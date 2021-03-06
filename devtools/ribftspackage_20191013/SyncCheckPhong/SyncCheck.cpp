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

#define Debug1
#define Debug3
#define Debug2

int main(int argc, char **argv) {
	int id=2;
	if (argc==2) id=atoi(argv[1]);

	SpyDataSink MyDataSink;
	if     (id==0)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110205",BUFFERSIZE);
	else if(id==1)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110206",BUFFERSIZE);
	else if(id==2)   MyDataSink= SpyDataSink( "/dev/shm/SHM_110207",BUFFERSIZE);
	while(1){
		uint32_t buffer32 [BUFFERSIZE];
	//	std::vector<uint32_t> buffer32;
	        memset(buffer32,0,BUFFERSIZE);
		int x=0;
		while(x==0){
			x= MyDataSink.Spy( (char*)buffer32);
			if(x>0){
				for(int i=0;i<BUFFERSIZE;i++){
				        std::cout<<i<<"\t"<<buffer32[0]<<std::endl;
				}
				sleep(1);
				usleep(1000);
			}else{
				usleep(100);
			}
		}
	}
	return 0;
}

