#include <sys/stat.h>
#include <QCoreApplication>
#include "daq.h"
#include <iostream>
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
#include<map>

using namespace std;
int main(int argc, char *argv[])
{
    int nCh=2;
    char line[100];
    char filename[500];
    char acsii_filename[500];

    if (argc==3){
        strcpy(filename,argv[1]);
        strcpy(acsii_filename,argv[2]);
    }else{
        cout<<"Invalid";
        return 0;
    }
    FILE *binFile;
    binFile=fopen(filename,"rb");

    FILE *acsii[nCh];
    for (int i=0;i<nCh;i++){
        char filename_temp[756];
        sprintf(filename_temp,"%s_ch%i.txt",acsii_filename,i);
        acsii[i]=fopen(filename_temp,"w");
        fprintf(acsii[i],"board_number|channel_number|event_number|coarse_timestamp|timmingLED|timmingCFD|shortQDC|longQDC\n");
    }

    unsigned int eventN;
    unsigned long coarseTS;
    double fineTScfd;
    double fineTSled;
    int lqdc;
    int sqdc;
    int bL;

    //read header if available here
    header_type header;
    header_type footer;
    size_t ss;
    if (binFile!=NULL) ss=fread(&header,sizeof(header_type),1,binFile);
    if (ss!=1) cout<<"Error reading header!"<<endl;
    cout<<"----------HEADER content---------"<<endl;
    cout<<"Run No: "<<header.runno<<endl;
    struct tm * start_timeinfo;
    start_timeinfo = localtime ( &header.start_stop_time );
    printf("Start time:%s",asctime(start_timeinfo));
    cout<<"Comment: "<<header.comment;
    cout<<"----------PROCEED TO FILE CHECKING-------------------"<<endl;
    fgets(line,80,stdin);

    //Calculating size
    //GET SIZE
        size_t file_size;
        struct stat st;
        stat(filename, &st);
        file_size = st.st_size;
        cout<<"FILE SIZE="<<file_size<<endl;
        int nBlock;
    if ((file_size-2*sizeof(header_type))%(sizeof(buffer_type)*header.MAX_BLOCK_t)==0){
        cout<<"FILE is OK!"<<endl;
        nBlock=(file_size-2*sizeof(header_type))/(sizeof(buffer_type)*header.MAX_BLOCK_t);
        cout<<"NO of BLOCK="<<nBlock<<endl;
        cout<<"NO of Event="<<nBlock*header.MAX_BLOCK_t<<endl;
    }else {
        cout<<"Bad file!!"<<endl;
        nBlock=(file_size-2*sizeof(header_type))/(sizeof(buffer_type)*header.MAX_BLOCK_t);
        cout<<"NO of BLOCK="<<nBlock<<endl;
        cout<<"NO of Event="<<nBlock*header.MAX_BLOCK_t<<endl;
    }
    cout<<"---------------------------------------"<<endl;
    fgets(line,80,stdin);
    //--------------------------
    buffer_type buf[header.MAX_BLOCK_t];
    size_t s;
    uint32_t totalTrgCnt[header.nboard][header.MaxNChannels_t];
    //int end_flag=0;
    int temp=0;
    int check_Flag=0;
    /*
                               shmp->buffer[shmp->block_no].blockno=shmp->block_no;
                               shmp->buffer[shmp->block_no].dgtzdata[0] = b;//Board number
                               shmp->buffer[shmp->block_no].dgtzdata[1] = ch;//Channel number
                               shmp->buffer[shmp->block_no].dgtzdata[2] = shmp->TrgCnt[b][ch];//event Number
                               shmp->buffer[shmp->block_no].dgtzdata[3] = Events_t[ch][ev].TimeTag;//TTT LSB
                               shmp->buffer[shmp->block_no].dgtzdata[4] = Events_t[ch][ev].Extras; //16bit TTT MSB + 16bit baseline value multiply by 4
                               if (Events_t[ch][ev].Baseline<0) Events_t[ch][ev].Baseline=0;
                               if (Events_t[ch][ev].ChargeLong<0) Events_t[ch][ev].ChargeLong=0;
                               if (Events_t[ch][ev].ChargeShort<0) Events_t[ch][ev].ChargeShort=0;
                               shmp->buffer[shmp->block_no].dgtzdata[5] = (uint32_t)Events_t[ch][ev].Baseline;
                               shmp->buffer[shmp->block_no].dgtzdata[6] = (uint32_t)Events_t[ch][ev].ChargeLong;
                               shmp->buffer[shmp->block_no].dgtzdata[7] = (uint32_t)Events_t[ch][ev].ChargeShort;
                               shmp->buffer[shmp->block_no].dgtzdata[8] = (uint32_t)Events_t[ch][ev].Pur;
                               shmp->buffer[shmp->block_no].dgtzdata[9] = (uint32_t)Events_t[ch][ev].Format;
                               shmp->buffer[shmp->block_no].dgtzdata[10] = 0;//bit 10 contain sample length, default 0
     */
    int nch=2;
    int channel;
    int board;
    int trg_num[100];

        for (int j=0;j<nBlock;j++){
            s=fread(buf,sizeof(buffer_type),header.MAX_BLOCK_t,binFile);
            for (Int_t i=0;i<header.MAX_BLOCK_t;i++){
                //get trg count
                board=buf[i].dgtzdata[0];
                channel=buf[i].dgtzdata[1];
                totalTrgCnt[board][channel]=buf[i].dgtzdata[2];
                eventN=buf[i].dgtzdata[2];
                coarseTS=buf[i].dgtzdata[3];
                lqdc=(Double_t)buf[i].dgtzdata[6];
                sqdc=(Double_t)buf[i].dgtzdata[7];
                //if (trg_order==trg_num)
                fineTScfd=(Double_t)buf[i].time_data[1];
                fineTSled=(Double_t)buf[i].time_data[0];
                bL=(Double_t)buf[i].dgtzdata[5];

                fprintf(acsii[channel],"%i %i %i %i %f %f %i %i\n",board,channel,eventN,coarseTS,fineTSled,fineTScfd,sqdc,lqdc);

                if (temp%1000==0&&temp>0) cout<<temp+1<<" events"<<endl;
                if (check_Flag==0){
                    cout<<"Channel "<<buf[i].dgtzdata[1]<<" data"<<endl;
                    cout<<"Event No="<<buf[i].dgtzdata[2]<<endl;

                    cout<<"LED time="<<buf[i].time_data[0]<<" | CFD time="<<buf[i].time_data[1]<<endl;
                    cout<<"Charge short="<<buf[i].dgtzdata[7]<<" | Charge long="<<buf[i].dgtzdata[6]<<endl;
                    cout<<"---------------"<<endl;
                }
                if (check_Flag==0){
                    if(fgets(line,80,stdin)==NULL) break;
                    if (strncmp(line," ",1)==0){
                        printf("START filling root file\n");
                        check_Flag=1;
                    }
                }
                temp++;
            }
        }




    cout<<"Total events prcess: "<<temp-1<<endl;

    if (binFile!=NULL) s=fread(&footer,sizeof(header_type),1,binFile);
    if (s<1) {
        cout<<"Can't read footer!"<<endl;
    }else{
        //For footer
        cout<<"---------------------------"<<endl;
        cout<<"End of run No "<<footer.runno<<endl;

        struct tm * stop_timeinfo;
        stop_timeinfo = localtime ( &footer.start_stop_time );
        printf("Aquisition stop at %s",asctime(stop_timeinfo));
        cout<<"SCALER INFO from footer:"<<endl;
        cout<<"End of run Comment: "<<footer.comment;

        for (int b=0;b<footer.nboard;b++){
            printf("Board No %i\n",b);
            for (int ch=0;ch<footer.MaxNChannels_t;ch++){
                if (!(footer.channelMask[b] & (1<<ch))) continue;
                printf("          Channel %i received %i events/ while %i events was opened from file \n",ch,footer.TrgCnt[b][ch],totalTrgCnt[b][ch]);
            }
        }
    }
    cout<<"displaying input file"<<endl;
    fgets(line,80,stdin);
    for (int i=0;i<header.inputFileContent_Length;i++) cout<<header.inputFileContent[i]<<endl;

    fclose(binFile);
}
