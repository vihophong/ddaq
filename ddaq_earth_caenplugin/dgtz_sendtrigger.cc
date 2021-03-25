
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define TIMEOUT 0x20000
#define OPADDR   0x102e
#define HSADDR   0x1030
#define OPWOK    0x01
#define OPROK    0x02


#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <pthread.h>

#include "CAENVMEtypes.h"
#include "CAENVMElib.h"
#include <CAENDigitizer.h>
#include <sstream>
#include <strings.h>

#include <math.h>
#include <time.h>


#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MIN 60


#define FIFOPATH "/tmp/fifo"
using namespace  std;

int get_value_hex(const char *arg){
    std::istringstream sarg ( arg);
    int value;
    std::string s = arg;

    int x =  s.find("0x",0);


    if (x == -1 )
      {
        sarg >> value;
      }
    else
      {
        sarg >> std::hex >> value;
      }

    return value;
}


double ran_expo(double lambda)
{
    double u;
    u = rand()/ (RAND_MAX+1.0);
    return -log(1-u)/lambda;
}

int main(int argc, char *argv[])
{
    if (argc!=2&&argc!=6&&argc!=3) {
        std::cout<<"Usage: ./dgtz_sendtrigger server linktype(optical/vme) link_no board_no vmeaddr "<<std::endl;
        std::cout<<"Usage: ./dgtz_sendtrigger client"<<std::endl;
        return 0;
    }else if ((strcmp(argv[1],(char*)"server")!=0)&&(strcmp(argv[1],(char*)"client")!=0)&&(strcmp(argv[1],(char*)"rate")!=0)){
        std::cout<<"Usage: ./dgtz_sendtrigger server linktype(optical/usb) link_no board_no vmeaddr "<<std::endl;
        std::cout<<"Usage: ./dgtz_sendtrigger client"<<std::endl;
        return 0;
    }
    if (strcmp(argv[1],(char*)"server")==0 && argc==6){
        int ret,handle;
        CAEN_DGTZ_ConnectionType LinkType;
        if (strcmp(argv[2],(char*)"optical")==0)
            LinkType = CAEN_DGTZ_OpticalLink;
        else
            LinkType = CAEN_DGTZ_USB;

        int LinkNum = atoi(argv[3]);
        int BoardNum = atoi(argv[4]);
        int VMEBaseAddress = atoi(argv[5]);

        ret = CAEN_DGTZ_OpenDigitizer(LinkType, LinkNum, BoardNum, VMEBaseAddress, &handle);
        if (ret) {
            std::cout<<"Can not open digitizer of LinkType="<<LinkType<<" LinkNum="<<LinkNum<<" BoardNum="<<BoardNum<<" VMEBaseAddress="<<VMEBaseAddress<<std::endl;
            exit(0);
        }
        CAEN_DGTZ_BoardInfo_t BoardInfo;
        ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
        if (ret) {
            cout<<"Board failure!"<<endl;
            exit(0);
        }
        printf("Connected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
        printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

        /* Open FIFO FD */
        int fifopipe;
        if((fifopipe = open(FIFOPATH, O_RDWR)) == -1){
          char myfifo[500];
          sprintf(myfifo,FIFOPATH);
          mkfifo(myfifo,0666);
          if((fifopipe = open(FIFOPATH, O_RDWR)) == -1){
              printf("can't open fifo\n");
          }
        }
        int bufin=0;
        while(1){
            ssize_t nbytesread = read(fifopipe, (char *)&bufin, sizeof(bufin));
            if (nbytesread==0) {
                std::cout<<"fifo read error!"<<std::endl;
                return 0;
            }
            ret = CAEN_DGTZ_SendSWtrigger(handle);
            if (ret) {
                cout<<"Can not send SW trigger!"<<endl;
                break;
            }
            struct timeval tv;
            struct timezone tz;
            gettimeofday(&tv,&tz);
            long hms = tv.tv_sec % SEC_PER_DAY;
            hms += tz.tz_dsttime * SEC_PER_HOUR;
            hms -= tz.tz_minuteswest * SEC_PER_MIN;
            hms = (hms + SEC_PER_DAY) % SEC_PER_DAY;
            int hour  = hms / SEC_PER_HOUR;
            int min = (hms % SEC_PER_HOUR) / SEC_PER_MIN;
            int sec = (hms % SEC_PER_HOUR) % SEC_PER_MIN;
            //printf("Software trigger sent at %d:%02d:%02d\n",hour, min, sec);
        }
    }else if (strcmp(argv[1],(char*)"client")==0 && argc==2){
        /* Open FIFO FD */
        char myfifo[500];
        sprintf(myfifo,FIFOPATH);
        //mkfifo(myfifo,0666);
        int fifobuf=1;
        int fd = open(myfifo, O_WRONLY|O_NONBLOCK);
        write(fd, &fifobuf, sizeof(fifobuf));
    }else if (strcmp(argv[1],(char*)"rate")==0 && argc==3){
        srand((unsigned)time(NULL));
        double rate = atof(argv[2]);
        cout<<ran_expo(rate)*1000000<<endl;
    }
    return 0;
}

