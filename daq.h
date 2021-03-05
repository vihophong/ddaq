#ifndef DAQ_H
#define DAQ_H

#define MAX_NBOARD       8
#define MaxNChannels    16
#define WF_BLOCK_LENGTH 500//4085 //maximum 8170
#define DGTZ_BLOCK_LENGTH 15 //maximum 8170
#define MAX_BLOCK       8

#define WF_WRITELENGTH  500

#define BLOCK_EMPTY     0
#define BLOCK_WRITING   1
#define BLOCK_WAITING   2
#define SHMKEY 75


//For share memory
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>raw_projectName
#include <errno.h>
#include <ctype.h>

#include <time.h>




#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32

    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
    #include <process.h>
    #define getch _getch     /* redefine POSIX 'deprecated' */
    #define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
#include "CAENDigitizer.h"
#ifdef __cplusplus
}
#endif

#include <QString>
#include "dpp.h"

typedef struct header_type_t{
   //config info
  int nboard;
  int MAX_BLOCK_t;
  int MaxNChannels_t;
  int WF_BLOCK_LENGTH_t;
  int DGTZ_BLOCK_LENGTH_t;
  uint32_t TrgCnt[MAX_NBOARD][MaxNChannels];
  uint32_t channelMask[MAX_NBOARD];
  uint32_t  preTrg[MAX_NBOARD][MaxNChannels];
  uint32_t  recordLength[MAX_NBOARD];
  int  preGate[MAX_NBOARD][MaxNChannels];
  int shortGate[MAX_NBOARD][MaxNChannels];
  int longGate[MAX_NBOARD][MaxNChannels];
  uint8_t pulsePolarity[MAX_NBOARD][MaxNChannels];

  time_t  start_stop_time;
  uint32_t  runno;
  char comment[500];
  char raw_filename[756];
  int inputFileContent_Length;
  char inputFileContent[500*MAX_NBOARD][500];
} header_type;

typedef struct buffer_type_t{
  unsigned int status,blockno;
  /*
                 uint32_t BinHeader[10];
                 BinHeader[0] = b;//Board number
                 BinHeader[1] = ch;//Channel number
                 BinHeader[2] = shmp->TrgCnt[b][ch];//event Number
                 BinHeader[3] = Events_t[ch][ev].TimeTag;//TTT LSB
                 BinHeader[4] = Events_t[ch][ev].Extras; //16bit TTT MSB + 16bit baseline value multiply by 4
                 BinHeader[5] = (uint32_t)Events_t[ch][ev].Baseline;
                 BinHeader[6] = (uint32_t)Events_t[ch][ev].ChargeLong;
                 BinHeader[7] = (uint32_t)Events_t[ch][ev].ChargeShort;
                 BinHeader[8] = (uint32_t)Events_t[ch][ev].Pur;
                 BinHeader[9] = (uint32_t)Events_t[ch][ev].Format;
  */
  uint16_t wfdata[WF_BLOCK_LENGTH];
  uint32_t dgtzdata[DGTZ_BLOCK_LENGTH];
  double time_data[2];
} buffer_type;

typedef struct SHM_DGTZ_S{
  uint32_t pid;
  uint32_t  runno;
  uint32_t  status;
  time_t  start_time;
  time_t  stop_time;
  uint32_t  block_no;

  uint32_t channelMask[MAX_NBOARD];
  char config_file_name[MAX_NBOARD][500];
  char raw_projectName[756];
  char raw_saveDir[756];
  char raw_filename[756];
  FILE *raw_fd;

  buffer_type_t buffer[MAX_BLOCK];
  uint32_t TrgCnt[MAX_NBOARD][MaxNChannels];
  uint32_t com_flag;  
  time_t com_time;
  char com_filename[500];
  char com_comments[500];
  char com_message[500]; //interface for start/stop daq

  uint32_t  preTrg[MAX_NBOARD][MaxNChannels];
  uint32_t  recordLength[MAX_NBOARD];
  int  preGate[MAX_NBOARD][MaxNChannels];
  int shortGate[MAX_NBOARD][MaxNChannels];
  int longGate[MAX_NBOARD][MaxNChannels];
  uint8_t pulsePolarity[MAX_NBOARD][MaxNChannels];

  int ana_status;
  int ana_flag;
  char ana_message[500];

  //new add
  int nboard;

} SHM_DGTZ;


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
  uint32_t DCOffset[16];
  uint32_t PreTriggerSize[16];
  char saveDir[500];
  char saveName[500];
} DigitizerParams_t;


class daq
{
    private:
    //char header[BLOCK_LENGTH*2];
    DigitizerParams_t* dgtz_parms;
    CAEN_DGTZ_DPP_PSD_Params_t* dpp_parms;
    char subFilename[500];


    int ret;
    int* handle;
    CAEN_DGTZ_BoardInfo_t           BoardInfo;
    uint32_t sAllocatedSize, sBufferSize,Nb;
    char *sbuffer_t;          // readout buffer, not be used in daqloop
    void *sEvents_t[MaxNChannels];  // events buffer
    uint32_t NumEvents_t[MaxNChannels];
    CAEN_DGTZ_DPP_PSD_Event_t ** Events_t;  // events buffer-convert C++

    CAEN_DGTZ_DPP_PSD_Waveforms_t* Waveform_t;
    void   *sWaveform_t;         // waveforms buffer

    void resetScaler();    
    void WriteHeader(int isHeader);

    uint64_t CurrentTime, PrevRateTime, ElapsedTime;

    CAEN_DGTZ_DPP_PSD_Params_t getDPPParams(char* filename);
    DigitizerParams_t getDigitizerParams(char* filename);
    int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask);
    int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams,char* filename);
    void connect();
    void QuitProgram();

    //----------------Define DPP parameter
    int CFD_delay;
    double CFD_fraction;
    double LED_threshold;
    double LED_threshold_LED;
    int gateOffset;
    int shortGate;
    int longGate;
    int nBaseline;
    int minVarBaseline;
    int mode_selection;

    uint32_t TrgCnt_e[MAX_NBOARD][MaxNChannels];
    uint32_t TrgCnt_rate[MAX_NBOARD][MaxNChannels];

    int write_flag;
    int nline;
    public:
    long get_time();
    SHM_DGTZ* shmp;
    header_type* header;
    header_type* footer;

    int shmid;
    daq(){};
    //~daq(){};
    void daqinit(int nboard_t,char config_file_name_t[MAX_NBOARD][500]);
    void daqloop();    
};

#endif // DAQ_H

