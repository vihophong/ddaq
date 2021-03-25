#include <iostream>

#include <fstream>

#include <daq_device_caen_v1741.h>
#include <string.h>


#include <cstdlib>

//extern int dc_file[MAX_CH];
//extern int thr_file[MAX_CH];

using namespace std;

static long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif


daq_device_caen_v1741::daq_device_caen_v1741(const int eventtype
                                             , const int subeventid
                                             , const int isdppmode // DPP mode (1) or waveform mode (0)
                                             , const int boardid
                                             , const int trigger)// do I give the system trigger?
{
    m_eventType  = eventtype;
    m_subeventid = subeventid;



    int node = 0;

    _warning = 0;

//    ret = CAEN_PADC_Success;
//    ErrCode= ERR_NONE;
    handle = NULL; // handle pointer
    MB_Cnt_Roll = NULL;

    buffer  = NULL;
    ReloadCfgStatus = 0x7FFFFFFF; // Init to the bigger positive number


    int handllee;
    if (CAEN_PADC_OpenPeakSensing(CAEN_PADC_OpticalLink, 3,1,0, &handllee)!=CAEN_PADC_Success) {
        printf("Error\n");
    }

//    /* *************************************************************************************** */
//    /* Open and parse configuration file                                                       */
//    /* *************************************************************************************** */

//    _boardnumber = boardid;
//    FILE *f_ini;
//    char ConfigFileName[100];
//    //strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
//    sprintf(ConfigFileName,"m1741_%i.txt",_boardnumber);
//    ErrCode = OpenConfigFile(&f_ini,ConfigFileName);
//    if (ErrCode == ERR_NONE) ErrCode = ParseConfigFile(f_ini, &ConfigVar);
//    if (f_ini != NULL) fclose(f_ini);
//    // Allocate space for handles and counters according to the number of boards in the acquisition chain
//    if (ErrCode == ERR_NONE) {
//        if (((handle = (int*)calloc(ConfigVar.Nhandle, sizeof(int)))==NULL) ||
//            ((MB_Cnt_Roll = (uint32_t*)calloc(ConfigVar.Nhandle, sizeof(uint32_t))) == NULL) ||
//            ((Counter = (Counter_t*)calloc(ConfigVar.Nhandle,sizeof(Counter_t)))==NULL) ||
//            ((CounterOld = (Counter_t*)calloc(ConfigVar.Nhandle, sizeof(Counter_t))) == NULL)
//            ) ErrCode = ERR_MALLOC;
//    }


//    // Open the peak-sensing devices and read the board information
//    if (ErrCode == ERR_NONE) {
//        printf("Open peak-sensing devices\n");
//        if (OpenPeakSensing(handle,ConfigVar)) {ErrCode = ERR_PADC_OPEN;}
//    }

//    // Print board info
//    if (ErrCode == ERR_NONE) {
//        printf("Get board info and set board-specific parameters\n");
//        for (int board = 0; board < ConfigVar.Nhandle; board++) {
//            if (CAEN_PADC_GetInfo(handle[board], &BoardInfo)) ErrCode = ERR_BOARD_INFO_READ;
//            else {
//                printf("****************************************\n");
//                printf("Connected to CAEN Peak-Sensing Model %s\n", BoardInfo.ModelName);
//                printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
//                printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
//            }
//        }
//        printf("****************************************\n");
//    }
//    // Program the peak-sensing devices
//    if (ErrCode == ERR_NONE) {
//        printf("Program the peak-sensing\n");
//        for (int board = 0; board < ConfigVar.Nhandle; board++) if (ProgramPeakSensing(handle[board], ConfigVar.BoardConfigVar[board])) ErrCode = ERR_PS_PROGRAM;
//    }
//    // Malloc readout buffer
//    if (ErrCode == ERR_NONE) {
//        printf("Readout buffer malloc\n");
//        Event = (CAEN_PADC_Event_t*)malloc(MAX_BLT_EVENTS*sizeof(CAEN_PADC_Event_t));
//        if (CAEN_PADC_MallocReadoutBuffer(handle[0], &buffer, &BufferSize)!=CAEN_PADC_Success) ErrCode = ERR_MALLOC; // WARNING: This malloc must be done after the peak-sensing programming
//    }

//    // malloc and reset counters
//    if(ErrCode== ERR_NONE) {
//        printf("Counter malloc\n");
//        for (int board = 0; board < ConfigVar.Nhandle; board++) {
//            if (MallocCounter(Counter + board) || MallocCounter(CounterOld + board)) {ErrCode = ERR_MALLOC; break;}
//            ResetCounter(Counter+board);
//            ResetCounter(CounterOld+board);
//        }
//    }

//    // Calibration Upload into SRAM
//    if(ErrCode== ERR_NONE) {
//        printf("Calibration upload\n");
//        for (int board = 0; board < ConfigVar.Nhandle; board++) {
//            if (CAEN_PADC_LoadOffset(handle[board]) != CAEN_PADC_Success) { ErrCode = ERR_CALIB_UPLOAD; break; }
//            if (CAEN_PADC_LoadCalibration(handle[board]) != CAEN_PADC_Success) {ErrCode = ERR_CALIB_UPLOAD; break;}
//        }
//    }

//    // No need for the moment
//    //if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)//XX742 not considered
//    //        Set_relative_Threshold(handle, &WDcfg, BoardInfo);

//    if (trigger)   _trigger_handler=1;
//    _trigger_handler=1;
//    if ( _trigger_handler )
//      {
//        cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
//        _th  = new CAENdrsTriggerHandler (handle[0], m_eventType);
//        registerTriggerHandler ( _th);
//      }
//    else
//      {
//        _th = 0;
//      }

//    for (int board = 0; board < ConfigVar.Nhandle; board++)
//        CAEN_PADC_SWStartAcquisition(handle[board]);


}



daq_device_caen_v1741::~daq_device_caen_v1741()
{


    if (_th)
    {
        clearTriggerHandler();
        delete _th;
        _th = 0;
    }

}

int  daq_device_caen_v1741::init()
{
    //to arm S-IN start/stop
    //WriteRegisterBitmask(handle,0x8100, 0x4, 0x4);
    receivedTrigger=0;
    return 0;
}

// the put_data function

int daq_device_caen_v1741::put_data(const int etype, int * adr, const int length )
{
    //cout<<"put data"<<endl;
    if ( _broken )
    {
        //      cout << __LINE__ << "  " << __FILE__ << " broken ";
        //      identify();
        return 0; //  we had a catastrophic failure
    }

    if (etype != m_eventType )  // not our id
    {
        return 0;
    }

    if ( length < max_length(etype) )
    {
        //      cout << __LINE__ << "  " << __FILE__ << " length " << length <<endl;
        return 0;
    }


    int len = 0;

    sevt =  (subevtdata_ptr) adr;
    // set the initial subevent length
    sevt->sub_length =  SEVTHEADERLENGTH;

    // update id's etc
    sevt->sub_id =  m_subeventid;
    sevt->sub_type=4;
    sevt->sub_decoding = 105;
    sevt->reserved[0] = _boardnumber;
    sevt->reserved[1] = 0;

    //sevt->sub_padding = len%2;
    //len += sevt->sub_padding;
    len += len%2;
    sevt->sub_length += len;
    //cout << __LINE__ << "  " << __FILE__ << " returning "  << sevt->sub_length << endl;

    sevt->sub_padding = NumEvents;//actual number of events;

    return  sevt->sub_length;
}


int daq_device_caen_v1741::endrun()
{
    //_broken = CAEN_DGTZ_SWStopAcquisition(handle);
    //_broken = CAEN_DGTZ_ClearData(handle);
//    if ( _broken )
//      {
//        cout<<"can't clear data"<<endl;
//        return 0; //  we had a catastrophic failure
//      }
//    return _broken;
    return _broken;
}

void daq_device_caen_v1741::identify(std::ostream& os) const
{

}


int daq_device_caen_v1741::max_length(const int etype) const
{
    if (etype != m_eventType) return 0;
    return  (14900);
}


// the rearm() function
int  daq_device_caen_v1741::rearm(const int etype)
{
    if ( _broken )
      {
        //      cout << __LINE__ << "  " << __FILE__ << " broken ";
        //      identify();
        return 0; //  we had a catastrophic failure
      }

    if (etype != m_eventType) return 0;

    return 0;
}
