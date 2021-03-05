#ifndef __CAEN_DGTZ_LIB__
#define __CAEN_DGTZ_LIB__

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

#include "flash.h"

#define DEFAULT_CONFIG_FILE  "WaveDumpConfig.txt"  /* local directory */
#define GNUPLOT_DEFAULT_PATH ".\\"

#define OUTFILENAME "wave"  /* The actual file name is wave_n.txt, where n is the channel */
#define MAX_CH  64          /* max. number of channels */
#define MAX_SET 16           /* max. number of independent settings */
#define MAX_GROUPS  8          /* max. number of groups */

#define MAX_GW  1000        /* max. number of generic write commads */

#define PLOT_REFRESH_TIME 1000

#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_TIMEOUT        200  // ms

#define PLOT_WAVEFORMS   0
#define PLOT_FFT         1
#define PLOT_HISTOGRAM   2

#define CFGRELOAD_CORRTABLES_BIT (0)
#define CFGRELOAD_DESMODE_BIT (1)

#define NPOINTS 2
#define NACQS   50


/* ###########################################################################
   Typedefs
   ###########################################################################
*/

typedef enum {
        OFF_BINARY=	0x00000001,			// Bit 0: 1 = BINARY, 0 =ASCII
        OFF_HEADER= 0x00000002,			// Bit 1: 1 = include header, 0 = just samples data
} OUTFILE_FLAGS;

typedef struct{
        float cal[MAX_SET];
        float offset[MAX_SET];
}DAC_Calibration_data;

typedef struct {
    int LinkType;
    int LinkNum;
    int ConetNode;
    uint32_t BaseAddress;
    int Nch;
    int Nbit;
    float Ts;
    int NumEvents;
    uint32_t RecordLength;
    int PostTrigger;
    int InterruptNumEvents;
    int TestPattern;
    CAEN_DGTZ_EnaDis_t DesMode;
    //int TriggerEdge;
    CAEN_DGTZ_IOLevel_t FPIOtype;
    CAEN_DGTZ_TriggerMode_t ExtTriggerMode;
    uint16_t EnableMask;
    char GnuPlotPath[1000];
    CAEN_DGTZ_TriggerMode_t ChannelTriggerMode[MAX_SET];
        CAEN_DGTZ_PulsePolarity_t PulsePolarity[MAX_SET];
    uint32_t DCoffset[MAX_SET];
    int32_t  DCoffsetGrpCh[MAX_SET][MAX_SET];
    uint32_t Threshold[MAX_SET];
        int Version_used[MAX_SET];
        uint8_t GroupTrgEnableMask[MAX_SET];
    uint32_t MaxGroupNumber;

        uint32_t FTDCoffset[MAX_SET];
        uint32_t FTThreshold[MAX_SET];
        CAEN_DGTZ_TriggerMode_t	FastTriggerMode;
        uint32_t	 FastTriggerEnabled;
    int GWn;
    uint32_t GWaddr[MAX_GW];
    uint32_t GWdata[MAX_GW];
        uint32_t GWmask[MAX_GW];
        OUTFILE_FLAGS OutFileFlags;
        uint16_t DecimationFactor;
    int useCorrections;
    int UseManualTables;
    char TablesFilenames[MAX_X742_GROUP_SIZE][1000];
    CAEN_DGTZ_DRS4Frequency_t DRS4Frequency;
    int StartupCalibration;
        DAC_Calibration_data DAC_Calib;
} WaveDumpConfig_t;


typedef struct WaveDumpRun_t {
    int Quit;
    int AcqRun;
    int PlotType;
    int ContinuousTrigger;
    int ContinuousWrite;
    int SingleWrite;
    int ContinuousPlot;
    int SinglePlot;
    int SetPlotOptions;
    int GroupPlotIndex;
        int GroupPlotSwitch;
    int ChannelPlotMask;
    int Restart;
    int RunHisto;
    uint32_t *Histogram[MAX_CH];
    FILE *fout[MAX_CH];
} WaveDumpRun_t;


/* Error messages */
typedef enum  {
    ERR_NONE= 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,
        ERR_OVERTEMP,
        ERR_BOARD_FAILURE,

    ERR_DUMMY_LAST,
} ERROR_CODES;

/* Function prototypes */
int ParseConfigFile(FILE *f_ini, WaveDumpConfig_t *WDcfg);
void Load_DAC_Calibration_From_Flash(int handle, WaveDumpConfig_t *WDcfg,CAEN_DGTZ_BoardInfo_t BoardInfo);
void Save_DAC_Calibration_To_Flash(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

//----------------------
void Set_relative_Threshold(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);
void Calibrate_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);
void Calibrate_XX740_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);
int Set_calibrated_DCO(int handle, int ch, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int GetMoreBoardInfo(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo, WaveDumpConfig_t *WDcfg);
int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask);
int CheckBoardFailureStatus(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo);
void GoToNextEnabledGroup(WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg);
int32_t BoardSupportsCalibration(CAEN_DGTZ_BoardInfo_t BoardInfo);
int32_t BoardSupportsTemperatureRead(CAEN_DGTZ_BoardInfo_t BoardInfo);
void calibrate(int handle, WaveDumpRun_t *WDrun, CAEN_DGTZ_BoardInfo_t BoardInfo);
int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

/*
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

int ProgramDigitizerDPPPHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams);

int ResetDigitizer(int handle);

DigitizerParams_t getDigitizerParams(char* filename);
CAEN_DGTZ_DPP_PHA_Params_t getDPPPHAParams(char* filename);
*/
#endif
