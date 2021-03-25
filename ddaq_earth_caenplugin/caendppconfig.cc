
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
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"
#include "caen_dgtz_pha_lib.h"
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
    if (argc!=2&&argc!=4) {
        std::cout<<"Usage: ./caendppconfig conf_file"<<std::endl;
        std::cout<<"Usage(to send software stop aquisition to a board): ./caendppconfig linkno boardno VMEBaseAddress"<<std::endl;
        return 0;
    }
    if (argc == 2){
        DigitizerParams_t dgtzparms=getDigitizerParams(argv[1]);
        CAEN_DGTZ_DPP_PHA_Params_t dppparms=getDPPPHAParams(argv[1]);
        /* *************************************************************************************** */
        /* Open the digitizer and read the board information                                       */
        /* *************************************************************************************** */
        int ret,handle;
        ret = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType)dgtzparms.LinkType, dgtzparms.LinkNum, dgtzparms.BoardNum, dgtzparms.VMEBaseAddress, &handle);
        if (ret) {
            std::cout<<"Can not open digitizer of LinkType="<<dgtzparms.LinkType<<" LinkNum="<<dgtzparms.LinkNum<<" BoardNum="<<dgtzparms.BoardNum<<" VMEBaseAddress="<<dgtzparms.VMEBaseAddress<<std::endl;
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
        // Check firmware revision (only DPP firmware can be used with this Demo) */
       int MajorNumber;
        sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
        if (MajorNumber != 139 && MajorNumber != 132&& MajorNumber != 136  && MajorNumber != 139  ) {
          printf("This digitizer has not a DPP-PSD/PHA firmware\n");
        }
        //Check for possible board internal errors
        ret = CheckBoardFailureStatus2(handle, BoardInfo);
        if (ret) {
            cout<<"Board failure!"<<endl;
            exit(0);
        }

        /* *************************************************************************************** */
        /* program the digitizer                                                                   */
        /* *************************************************************************************** */

        ret = ResetDigitizer(handle);
        if (ret) {
            cout<<"Error while Reseting the Dititizer!"<<endl;
            exit(0);
        }

        ret = ProgramDigitizerDPPPHA(handle,dgtzparms,dppparms);

        if (ret) {
            cout<<"Error while programming the Dititizer!"<<endl;
            exit(0);
        }
        usleep(300000);//300 ms
        cout<<"Programmed digitizer!"<<endl;
    }
    if (argc == 4){
        int ret,handle;
        ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), &handle);
        if (ret) {
            std::cout<<"Can not open digitizer LinkNum="<<atoi(argv[1])<<" BoardNum="<<atoi(argv[2])<<" VMEBaseAddress="<<argv[3]<<std::endl;
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

        CAEN_DGTZ_SWStopAcquisition(handle);
        CAEN_DGTZ_ClearData(handle);
        //CAEN_DGTZ_Reset(handle);
        std::cout<<"Stop and clear data of digitizer LinkNum="<<atoi(argv[1])<<" BoardNum="<<atoi(argv[2])<<" VMEBaseAddress="<<argv[3]<<std::endl;
    }

    return 0;
}

