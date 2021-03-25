#ifndef __DAQ_DEVICE_LUPO__
#define __DAQ_DEVICE_LUPO__


#include <daq_device.h>
#include <stdio.h>
#include <CAENdrsTriggerHandler.h>
#include <CAENDigitizerType.h>
#include <lupo_lib.h>

#define V775_SEPARATION_BIT 0x98000545

class daq_device_lupo: public  daq_device {


public:

    daq_device_lupo(const int eventtype
                       , const int subeventid
                       , const int linknumber = 0
                       , const int boardnumber = 0
                       , const int address = 0
                       , const int trigger = 1);
    
    ~daq_device_lupo();


    void identify(std::ostream& os = std::cout) const;

    int max_length(const int etype) const;

    // functions to do the work

    int put_data(const int etype, int * adr, const int length);

    int init();
    int rearm( const int etype);
    int endrun();

   protected:

    lupoLib * lupo;


    int _broken;
    int _warning;

    subevtdata_ptr sevt;

    int _linknumber;
    int _boardnumber;

    int receivedTrigger;
    int triggerCounter;
    unsigned int deadtimeC;
    long long timestamp;

    int _trigger;
    int _trigger_handler;

    uint32_t AllocatedSize, BufferSize, NumEvents;


    CAENdrsTriggerHandler *_th;

};


#endif
