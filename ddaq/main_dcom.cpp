
#include <QCoreApplication>
#include "daq.h"
#include "TSocket.h"
#include "TPServerSocket.h"
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char *argv[])
{

    int ndict=17;
    char dict[ndict][256];
    strcpy(dict[0],"Ready if you are! ");
    strcpy(dict[1],"Beam me up Scottie! ");
    strcpy(dict[2],"What's up? ");
    strcpy(dict[3],"Hello! ");
    strcpy(dict[4],"Xin chao! ");
    strcpy(dict[5],"Sumimasen? ");
    strcpy(dict[6],"Bonjour! ");
    strcpy(dict[7],"At your service? ");
    strcpy(dict[8],"Your wish? ");
    strcpy(dict[9],"Feed me! ");
    strcpy(dict[10],"Go for it? ");
    strcpy(dict[11],"I want a cookie! ");
    strcpy(dict[12],"Yes Master? ");
    strcpy(dict[13],"Next? ");
    strcpy(dict[14],"Hey, man, what's next? ");
    strcpy(dict[15],"You called? ");
    strcpy(dict[16],"Ohayo gozaimasu! ");


    daq daq_t;
    if ((daq_t.shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1){
        daq_t.shmp = (struct SHM_DGTZ_S *)shmat(daq_t.shmid, 0, 0);
    }else{
        printf("Can not attach to share memory!!!!!\n");
    }
    char comline[256];

    strcpy(comline,dict[rand() % ndict + 0]);

    char* line;
    while(1)
    {        
        line = readline(comline);
        if (line && *line) add_history(line);
        if(line==NULL)
        {
            break;
        }else{
            int temp_num=rand() % ndict + 0;
            if (temp_num==16){
                struct tm * start_timeinfo_t;
                time_t time_now=time(0);
                start_timeinfo_t = localtime (&time_now);
                if (start_timeinfo_t->tm_hour>=6&&start_timeinfo_t->tm_hour<12)
                    strcpy(comline,"Ohayo gozaimasu! ");
                else if (start_timeinfo_t->tm_hour>=12&&start_timeinfo_t->tm_hour<18)
                    strcpy(comline,"Konnichiwa! ");
                else if (start_timeinfo_t->tm_hour>=18&&start_timeinfo_t->tm_hour<24)
                    strcpy(comline,"Konbanwa! ");
                else if (start_timeinfo_t->tm_hour>=0&&start_timeinfo_t->tm_hour<6)
                    strcpy(comline,"Oyasuminasai! ");
                //printf("Start time:%s",asctime(start_timeinfo));
            }
            else {
                strcpy(comline,dict[temp_num]);
            }
        }


        //tolowers(line,80);
        if(strncmp(line,"start",3)==0)
        {
            puts("  ");
            if(daq_t.shmp->status!=0)
                puts("acquisition is already running");
            else
            {
                sprintf(daq_t.shmp->com_message,"start");
                time(&daq_t.shmp->com_time);
                daq_t.shmp->com_flag=1;

                while(daq_t.shmp->com_flag!=0)
                    usleep(500);
                struct tm * start_timeinfo2 ;
                start_timeinfo2 = localtime (&daq_t.shmp->start_time);
                printf("Acquisition Started at %s\n",asctime(start_timeinfo2));
            }            
        }
        else if(strncmp(line,"stop",3)==0)
        {
            puts("  ");
            if(daq_t.shmp->status==0)
                puts("no acquisition ran");
            else
            {
                sprintf(daq_t.shmp->com_message,"stop");
                time(&daq_t.shmp->com_time);
                daq_t.shmp->com_flag=1;
                while(daq_t.shmp->com_flag!=0)
                    usleep(200);
                struct tm * stop_timeinfo2;
                stop_timeinfo2 = localtime (&daq_t.shmp->stop_time);
                printf("Acquisition Stopped at %s\n",asctime(stop_timeinfo2));
            }            
        }
        else if(strncmp(line,"open",3)==0)
        {
            puts("  ");
            sprintf(daq_t.shmp->com_message,"open");

            if(daq_t.shmp->status!=0)
                puts("acquisition still running !! Stop first!!");
            else if(daq_t.shmp->raw_fd == NULL)
            {
              ////-----------Change by Size----------------
              char line2[80];
              sprintf(line2, "%s.%03d", daq_t.shmp->com_filename, daq_t.shmp->runno+1);
              if(access(line2,0)==0)
                {
                  printf("file=%s is exist, do you want to overwrite it? (y/n)\n",line2);
                  int ch;
                  ch=getchar();
                  if(ch=='y'||ch=='Y')
                chmod(line2,0640);
                  else
                continue;
                }
                printf("open RUN-%03d\socketn",daq_t.shmp->runno);
                printf("Comment for the begining of the RUN\n");
                fgets(daq_t.shmp->com_comments,256,stdin);
                sprintf(daq_t.shmp->com_message,"open");
                daq_t.shmp->com_flag=1;
                while(daq_t.shmp->com_flag!=0)
                    usleep(200);
            }
            else
                puts("Another file still openning !! Close first!!");
        }
        else if(strncmp(line,"close",3)==0)
        {
            //sprintf(daq_t.shmp->com_message,"close");
            //daq_t.shmp->com_flag=1;
            puts("  ");
            if(daq_t.shmp->status!=0)
                puts("acquisition still running !! Stop first!!");
            else if(daq_t.shmp->raw_fd != NULL)
            {
                printf("Closing RUN-%04d file=%s\n",daq_t.shmp->runno,daq_t.shmp->raw_filename);
                printf("Comment for the end of RUN\n");
                fgets(daq_t.shmp->com_comments,256,stdin);
                time(&daq_t.shmp->com_time);
                daq_t.shmp->com_flag=1;
                sprintf(daq_t.shmp->com_message,"close");
                while(daq_t.shmp->com_flag!=0)
                    usleep(200);
                printf("Closed file!\n");
            }            
        }
        else if(strncmp(line,"set",3)==0)
        {
            puts("  ");
            if(daq_t.shmp->status!=0)
                puts("acquisition still running !! Stop first!!");
            else if(daq_t.shmp->raw_fd != NULL)
                puts("output file is opened !! close first!!");
            else
            {
                sprintf(daq_t.shmp->com_message,"set");
                printf("Project file name: ");
                char line2[256];
                fgets(line2,256,stdin);
                for(int i=0;i<256;i++) if(line2[i]<0x20) line2[i]=0;//get rid of \n
                sprintf(daq_t.shmp->raw_projectName,"%s",line2);

            }
            puts("  ");
        }
        else if(strncmp(line,"run",3)==0)
        {
            puts("  ");
            if(daq_t.shmp->status!=0)
                puts("acquisition still running !! Stop first!!");
            else if(daq_t.shmp->raw_fd != NULL)
                puts("output file is opened !! close first!!");
            else
            {
                sprintf(daq_t.shmp->com_message,"run");
                printf("RUN No. = ");
                fgets(line,80,stdin);
                if(atoi(line)>0)
                  daq_t.shmp->runno=atoi(line);
            }
            puts("  ");
        }
        else if(strncmp(line,"exit",3)==0)
        {
            exit(0);
        }
        else if(strncmp(line,"help",3)==0)
        {
            puts("************ command for DAQ control *************");
            puts(" start : DAQ start ");
            puts(" stop  : DAQ stop ");
            puts(" socket  : send data over internet ");
            puts(" open  : open a new data file ");
            puts(" close : close present data file ");
            puts(" set   : set filename");
            puts(" run   : set run number");
            puts("ENTER: Display status");
            puts(" help: its me ");
            puts(" exit : exit this command interface");
        }
        else{
            puts(" ");
            if(daq_t.shmp->status!=0)
            {
                puts("Acquisition is running");

                if(daq_t.shmp->raw_fd != NULL){
                    puts("");
                    printf("An output file is opened at: %s\n",daq_t.shmp->raw_filename);
                }
                else{
                    puts("No openning output file");
                }
                printf("----------------RUN NO %i--------------------\n",daq_t.shmp->runno);
                struct tm * start_timeinfo;
                start_timeinfo = localtime ( &daq_t.shmp->start_time );
                printf("Start time:%s\n",asctime(start_timeinfo));

                for (int b=0;b<daq_t.shmp->nboard;b++){
                    printf("Board No %i\n",b);
                    for (int ch=0;ch<MaxNChannels;ch++){
                        if (!(daq_t.shmp->channelMask[b] & (1<<ch))) continue;
                        printf("          Active channel %i received %i events\n",ch,daq_t.shmp->TrgCnt[b][ch]);
                    }
                }

            }else {
                puts("No Acquisition running");
            }
            printf("****\n");
            printf("Current run number: %i\n",daq_t.shmp->runno);
            printf("Project name: %s\n",daq_t.shmp->raw_projectName);
            printf("Data directory: %s\n",daq_t.shmp->raw_saveDir);
            puts("  ");
        }
        //send information over internet
        //printf("%d",socket->SendRaw(daq_t.shmp,sizeof(SHM_DGTZ_S)));
    }
    return 0;
}

