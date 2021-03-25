
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "CAENPeakSensing.h"

#define TIMEOUT 0x20000
#define OPADDR   0x102e
#define HSADDR   0x1030
#define OPWOK    0x01
#define OPROK    0x02


#include <stdint.h>
#include <iostream>
#include <sstream>

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
int main(int argc, char *argv[])
{
    if (argc!=4) {
        std::cout<<"Usage(to send software stop aquisition to a board): ./caendppconfig linkno boardno VMEBaseAddress"<<std::endl;
        return 0;
    }

    if (argc == 4){
        int ret,handle;
        //ret = CAEN_PADC_OpenPeakSensing(CAEN_PADC_OpticalLink, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), &handle);
        ret = CAEN_PADC_OpenPeakSensing(CAEN_PADC_OpticalLink, 3,1,0, &handle);

        if (ret) {
            std::cout<<"Can not open digitizer LinkNum="<<atoi(argv[1])<<" BoardNum="<<atoi(argv[2])<<" VMEBaseAddress="<<argv[3]<<std::endl;
            exit(0);
        }
        CAEN_PADC_BoardInfo_t BoardInfo;

        if (CAEN_PADC_GetInfo(handle, &BoardInfo)) printf("Error Read Info\n");
        else {
            printf("****************************************\n");
            printf("Connected to CAEN Peak-Sensing Model %s\n", BoardInfo.ModelName);
            printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
            printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
        }

    }

    return 0;
}

