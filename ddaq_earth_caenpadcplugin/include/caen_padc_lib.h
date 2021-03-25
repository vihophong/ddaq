#ifndef __CAEN_PADC_LIB__
#define __CAEN_PADC_LIB__

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "CAENPeakSensing.h"
#include "flash.h"

#define MAX_CH_PLOT      8    /* Maximum number of traces in a plot */
#define MB_SIZE          1048576
#define PLOT_DATA_FILE   "PlotData.txt"
#define OUTFILE_NAME     "run0"
#ifdef LINUX
#define GNUPLOT_COMMAND      "gnuplot"
#define OUTFILE_PATH         "/PeakSensing_output/"
#define PLOTTER_PATH         ""
#define DEFAULT_CONFIG_FILE_PADC  "/etc/PeakSensingDAQ/PeakSensing_Config.txt"
#else
#define GNUPLOT_COMMAND		 "pgnuplot"
#define OUTFILE_PATH         "PeakSensing_output\\"
#define PLOTTER_PATH         "\\"
#define DEFAULT_CONFIG_FILE_PADC  "PeakSensing_Config.txt"
#endif

#include "CAENPeakSensingType.h"

typedef struct {
    CAEN_PADC_ConnectionType LinkType;
    int LinkNum;
    int ConetNode;
    uint32_t BaseAddress;
    CAEN_PADC_IOLevel_t FPIOtype;
    //CAEN_PADC_TriggerMode_t SWTriggerMode;
    CAEN_PADC_TriggerMode_t ExtTriggerMode;
    CAEN_PADC_AcqMode_t StartMode;
    //uint32_t GainFactor;
    uint32_t RecordLength; // Record-length
    int SlScaleEnable; // Sliding scale enable (if disabled, no baseline subtraction performed by FPGA)
    int ZSEnable; // Zero-suppression enable
    int Polarity; // Signal polarity selector
    int InputRange; // Input Range selector
    CAEN_PADC_DBTriggerMode_t TrigMode; // Trigger mode (PULSE or GATE)
    float GateValue; // Width of the gate in which the measurement is performed after the trigger (required for the rate estimate)
    int GateParalizable; // Gate is paralizable
    CAEN_PADC_ChMode_t ChMode; // number of bits (14,...,10)
    int ChMax; // maximum number of channels (64 or 32)
    uint8_t EnableMask[8]; // Enable Mask (8 8-channel groups)
    uint16_t ZSThreshold[8]; // Zero-suppression threshold (for groups of 8 channels)
} PeakSensingBoardConfig_t;

typedef struct {
    int Nhandle; // number of selected boards
    int AcqRun; // Acquisition flag
    int Quit; // Quit Demo flag
    int ContTrigger; // continuous SW trigger rate enabled
    // Parameters for the plot and the output file
    int OFRawEnable; // Save raw data
    int OFListEnable; // Save data list
    int OFHistoEnable; // Save histograms
    char OutFilePath[100]; // Output file path
    char OutFileName[100]; // Output file name
    char GnuPlotPath[100]; // gnuplot exec path
    int MaxFileSize; // Max file size (when the output file reaches this size, it is closed and a new one is opened)
    int PlotEnable; // Enable periodic plot
    int PlotRefreshTime; // Plot period
    int SinglePlot; // One-shot plot
    uint64_t StartTime; // start time (reset when r or R is pressed)
    uint32_t PileUpTime;
    uint32_t TotalTime;
    uint16_t BoardPlotted; // board plotted(change with +/-)
    uint16_t GroupPlotted;  // 8-channel group plotted (change with 0/9)
    uint8_t TrackPlotted; // Plotted channels of the selected 8-channel group (activate/deactivate through the [1:8] keys)
    PeakSensingBoardConfig_t *BoardConfigVar[100];
} PeakSensingConfig_t;

typedef struct {
    uint32_t Tot_Ev;
    uint32_t NotRej_Ev;
    uint32_t Pu_Ev;
    uint32_t Byte_Cnt;
    uint64_t MB_Cnt; // MB event counter (by default it also includes triggers received by the MB during the dead time)
    uint64_t MB_TS;
    uint32_t Tot_Cnt[64]; //
    uint32_t Of_Cnt[64]; // Overflow counters: counts the number of events in which at least one sample was in overflow
    uint32_t Pu_Cnt[64]; // Pile-up counters: counts the events during whose collection the pile-up signal (PUR) fired
    double *Amp_Cnt[64]; // Amplitude counters: counts the events for all bins of each channel
} Counter_t;

typedef struct {
    char              Title[100];
    char              TraceName[MAX_CH_PLOT][100];
    char              Xlabel[100];
    char              Ylabel[100];
    int               Xautoscale;
    int               Yautoscale;
    float             Xscale;
    float             Yscale;
    float             Xmax;
    float             Ymax;
    float             Xmin;
    float             Ymin;
    int               Gain[MAX_CH_PLOT];
    int               Offset[MAX_CH_PLOT];
    FILE              *plotpipe;
} PeakSensingPlot_t;

/* Error messages */
typedef enum {
    ERR_NONE = 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_PARSE_CONFIG,
    ERR_PADC_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_PS_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTDIR_OPEN,
    ERR_OUTFILE_OPEN,
    ERR_OUTFILE_WRITE,
    ERR_CALIB_UPLOAD,
} ERROR_CODES;

static char ErrMsg[ERR_CALIB_UPLOAD][100] = {
    "No Error",                                         /* ERR_NONE */
    "Configuration File not found",                     /* ERR_CONF_FILE_NOT_FOUND */
    "Parsing Error",                                    /* ERR_PARSE_CONFIG */
    "Can't open the peak sensing",                      /* ERR_PADC_OPEN */
    "Can't read the board info",                        /* ERR_BOARD_INFO_READ */
    "Can't run the DAQ program on this peak sensing",   /* ERR_INVALID_BOARD_TYPE */
    "Can't program the peak sensing",                   /* ERR_PS_PROGRAM */
    "Can't allocate the memory for the readout buffer", /* ERR_MALLOC */
    "Restarting Error",                                 /* ERR_RESTART */
    "Interrupt Error",                                  /* ERR_INTERRUPT */
    "Readout Error",                                    /* ERR_READOUT */
    "Event Build Error",                                /* ERR_EVENT_BUILD */
    "Can't allocate the memory for the histograms",     /* ERR_HISTO_MALLOC */
    "Unhandled board type",                             /* ERR_UNHANDLED_BOARD */
    "Output directory open error"                       /* ERR_OUTDIR_OPEN*/
    "Output file open error"                            /* ERR_OUTFILE_OPEN*/
    "Output file write error",                          /* ERR_OUTFILE_WRITE */
    "Calibration Upload Error",						    /* ERR_CALIB_UPLOAD */
};

int OpenPeakSensing(int *handle, PeakSensingConfig_t ConfigVar);
int ProgramPeakSensing(int handle, PeakSensingBoardConfig_t *BoardConfigVar);
int MallocCounter(Counter_t *Counter);
int ResetCounter(Counter_t *Counter);
int AddEvents(CAEN_PADC_Event_t *Event, uint32_t NumEvents, Counter_t *Counter, int MaxChannels);
ERROR_CODES OpenConfigFile(FILE **f_ini, char *ConfigFileName);
ERROR_CODES ParseConfigFile(FILE *f_ini, PeakSensingConfig_t *ConfigVar);

#endif
