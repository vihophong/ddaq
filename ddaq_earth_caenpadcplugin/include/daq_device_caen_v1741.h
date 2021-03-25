#ifndef __DAQ_DEVICE_CAEN_V1741__
#define __DAQ_DEVICE_CAEN_V1741__


#include <daq_device.h>
#include <stdio.h>


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <CAENdrsTriggerHandler.h>

#include <CAENPeakSensing.h>
#include <CAENPeakSensingType.h>
//#include <caen_padc_lib.h>


//unset if you dont want FIFO
//#define MAX_NEVT_FIFO_SEND 20

class daq_device_caen_v1741: public  daq_device {

public:
    daq_device_caen_v1741(const int eventtype
               , const int subeventid
               , const int isdppmode = 0
               , const int boardid = 0
               , const int trigger = 1);

    ~daq_device_caen_v1741();



    void identify(std::ostream& os = std::cout) const;

    int max_length(const int etype) const;

    // functions to do the work

    int put_data(const int etype, int * adr, const int length);

    int init();
    int rearm( const int etype);
    int endrun();

   protected:
    int _broken;
    int _warning;

    subevtdata_ptr sevt;

    int *handle; // handle pointer

    int _trigger;
    int _trigger_handler;
    int _boardnumber;

    int _linknumber;
    int _delay;

    uint32_t AllocatedSize, BufferSize, NumEvents;


    CAENdrsTriggerHandler *_th;


    int receivedTrigger;

    bool is_malloc;


    int ReloadCfgStatus; // Init to the bigger positive number

//    ERROR_CODES ErrCode;
//    CAEN_PADC_ErrorCode ret;

//    CAEN_PADC_Event_t *Event;
//    Counter_t *Counter,*CounterOld;
//    CAEN_PADC_BoardInfo_t BoardInfo;
//    PeakSensingConfig_t ConfigVar;

    uint32_t *MB_Cnt_Roll;
    // readout buffer and related variables
    char *buffer;
};


#endif
