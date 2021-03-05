#ifndef __CAEN_DGTZ_PHA_LIB__
#define __CAEN_DGTZ_PHA_LIB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <fstream.h>
#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>
#include <unistd.h>
#include <stdint.h>   /* C99 compliant compilers: uint64_t */
#include <ctype.h>    /* toupper() */
#include <sys/time.h>

//! DPP functions
#define MaxNChannels    16
typedef struct
{
  CAEN_DGTZ_ConnectionType LinkType;
  uint32_t VMEBaseAddress;
  int LinkNum;
  int BoardNum;
  uint32_t RecordLength;
  uint32_t ChannelMask;
  int EventAggr;
  CAEN_DGTZ_PulsePolarity_t PulsePolarity[MaxNChannels];
  CAEN_DGTZ_DPP_AcqMode_t AcqMode;
  CAEN_DGTZ_IOLevel_t IOlev;
  CAEN_DGTZ_TriggerMode_t ExtTriggerInputMode;
  uint32_t DCOffset[MaxNChannels];
  uint32_t PreTriggerSize[MaxNChannels];
  uint32_t reg_address[100];
  uint32_t reg_data[100];
  uint32_t reg_mask[100];
  uint32_t nreg;
  int analogProbes[2];
  int digitalProbes;
  //CAEN_DGTZ_DPP_VirtualProbe_t virtualProbeMode;
  //CAEN_DGTZ_DPP_PHA_VirtualProbe1_t phaVirtualProbe1;
  //CAEN_DGTZ_DPP_PHA_VirtualProbe2_t phaVirtualProbe2;
  //CAEN_DGTZ_DPP_PHA_DigitalProbe_t phaDigitalProbe;
} DigitizerParams_t;

int WriteRegisterBitmask2(int32_t handle, uint32_t address, uint32_t data, uint32_t mask);
int CheckBoardFailureStatus2(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo);
int ProgramDigitizerDPPPHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams);
int ResetDigitizer(int handle);

DigitizerParams_t getDigitizerParams(char* filename);
CAEN_DGTZ_DPP_PHA_Params_t getDPPPHAParams(char* filename);

#endif
