#include <iostream>

#include <fstream>


#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#include "fft.h"
#include "X742CorrectionRoutines.h"
#include "daq_device_caen_v1730_dpppha.h"

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


daq_device_caen_v1730_dpppha::daq_device_caen_v1730_dpppha(const int eventtype
                                             , const int subeventid
                                             , const int isdppmode // DPP mode (1) or waveform mode (0)
                                             , const int boardid
                                             , const int trigger)// do I give the system trigger?
{

    const char* evn_fMAX_NEVT_FIFO_SEND=std::getenv("MAX_NEVT_FIFO_SEND");
    if (evn_fMAX_NEVT_FIFO_SEND)
        fMAX_NEVT_FIFO_SEND = atoi(evn_fMAX_NEVT_FIFO_SEND);
    else fMAX_NEVT_FIFO_SEND = 0;

    if (fMAX_NEVT_FIFO_SEND!=0){
	cout<<"Attached with FIFO comm, MAX_NEVT_FIFO_SEND = "<<fMAX_NEVT_FIFO_SEND<<endl;
        //! fifo for interprocess communication
        sprintf(myfifo,"/tmp/myfifo");
        mkfifo(myfifo,0666);
        fifobuf=0;
        fifocal_prevEventCounter=0;
    }
    m_eventType  = eventtype;
    m_subeventid = subeventid;

    _broken = 0;

    handle = 0;

    int node = 0;

    _warning = 0;

    ret = CAEN_DGTZ_Success;
    Nb=0, Ne=0;
    buffer = NULL;
    EventPtr = NULL;
    isVMEDevice= 0;
    nCycles= 0;


    /* *************************************************************************************** */
    /* Read Parameters                                       */
    /* *************************************************************************************** */

    _boardnumber = boardid;
    sprintf(ConfigFileName,"v1730Config_m%i.txt",_boardnumber);
    printf("Opening Configuration File %s\n", ConfigFileName);

    DigitizerParams_t dgtzparms=getDigitizerParams(ConfigFileName);
    CAEN_DGTZ_DPP_PHA_Params_t dppparms=getDPPPHAParams(ConfigFileName);

    //DigitizerParams_t dgtzparms=getDigitizerParams((char*)"DPPConfig.txt");
    //CAEN_DGTZ_DPP_PHA_Params_t dppparms=getDPPPHAParams((char*)"DPPConfig.txt");
    //cout<<"Trapezoid Rise Time (ns): "<<dppparms.k[0]<<endl;

    /* *************************************************************************************** */
    /* Open the digitizer and read the board information                                       */
    /* *************************************************************************************** */

    ret = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType)dgtzparms.LinkType, dgtzparms.LinkNum, dgtzparms.BoardNum, dgtzparms.VMEBaseAddress, &handle);
    if (ret) {
        cout<<"Can not open digitizer of LinkType="<<dgtzparms.LinkType<<" LinkNum="<<dgtzparms.LinkNum<<" BoardNum="<<dgtzparms.BoardNum<<" VMEBaseAddress="<<dgtzparms.VMEBaseAddress<<endl;
        exit(0);
    }


    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    if (ret) {
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

//    //! write down handle file
//    std::ofstream handlefileout("handle",std::ofstream::out);
//    handlefileout<<handle;
//    handlefileout.close();
//    printf("Write handle file with val = %d\n", handle);

    //Check for possible board internal errors
    ret = CheckBoardFailureStatus2(handle, BoardInfo);
    if (ret) {
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

    //check for possible failures after programming the digitizer
    ret = CheckBoardFailureStatus2(handle, BoardInfo);
    if (ret) {
            cout<<"Board Failure detected!"<<endl;
            exit(0);
    }
    cout<<"Successfully programmed Dititizer!"<<endl;

//    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
//    if (ret) {
//        ErrCode = ERR_MALLOC;
//        exit(0);
//    }

    if (trigger)   _trigger_handler=1;
    _trigger_handler=1;
    if ( _trigger_handler )
      {
        cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
        _th  = new CAENdrsTriggerHandler (handle, m_eventType);
        registerTriggerHandler ( _th);
      }
    else
      {
        _th = 0;
      }

    CAEN_DGTZ_SWStartAcquisition(handle);
}



daq_device_caen_v1730_dpppha::~daq_device_caen_v1730_dpppha()
{
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_FreeEvent(handle, (void**)&Event8);

    if (_th)
    {
        clearTriggerHandler();
        delete _th;
        _th = 0;
    }
}

int  daq_device_caen_v1730_dpppha::init()
{
    //CAEN_DGTZ_SWStartAcquisition(handle);
    receivedTrigger = 0;
    n_datareaderror = 0;
    return 0;
}

// the put_data function

int daq_device_caen_v1730_dpppha::put_data(const int etype, int * adr, const int length )
{
    //cout<<"put data"<<endl;
//    if ( _broken )
//    {
//        //      cout << __LINE__ << "  " << __FILE__ << " broken ";
//        //      identify();
//        cout << __LINE__ << "  " << __FILE__ << " _broken " << _boardnumber <<endl;
//        return 0; //  we had a catastrophic failure
//    }

    if (etype != m_eventType )  // not our id
    {
        cout << __LINE__ << "  " << __FILE__ << " m_eventType " << _boardnumber <<endl;
        return 0;
    }

    if ( length < max_length(etype) )
    {
        cout << __LINE__ << "  " << __FILE__ << " length " << length <<endl;
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

    /* Read data from the board */
    ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, (char *) &sevt->data, &BufferSize);
    if (ret) {
        if (n_datareaderror<10||n_datareaderror%1000==0)
            cout<<"CAEN_DGTZ_ReadData Error! "<<n_datareaderror<<endl;
        n_datareaderror++;
        return 0;
        //exit(0);
    }
    if (BufferSize==0){
        uint32_t lstatus;
        ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
        if (ret) {
            printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
        }
        else {
            if (lstatus & (0x1 << 19)) {
                cout<<"ERR_OVERTEMP code!"<<endl;
            }
        }
        return 0;
    }
    receivedTrigger++;
//    if (fMAX_NEVT_FIFO_SEND!=0){
//        if (receivedTrigger-fifocal_prevEventCounter>fMAX_NEVT_FIFO_SEND){
//            addfifo(receivedTrigger);
//            fifocal_prevEventCounter=receivedTrigger;
//        }
//    }
    //! Calculate data
    //        ret = thouroughput
    Nb += BufferSize;
    CurrentTime = get_time();
    ElapsedTime = CurrentTime - PrevRateTime;
    nCycles++;
    if (ElapsedTime > 1000) {
        ReadoutRate=(float)Nb/((float)ElapsedTime*1048.576f);
        if (Nb == 0)
            printf("No data...\n");
        else
            printf("Reading board %d at %.5f MB/s \n", _boardnumber, ReadoutRate);

        nCycles= 0;
        Nb = 0;
        PrevRateTime = CurrentTime;
    }

    len = BufferSize /4;

    //sevt->sub_padding = len%2;
    //len += sevt->sub_padding;
    len += len%2;
    sevt->sub_length += len;
    //cout << __LINE__ << "  " << __FILE__ << " returning "  << sevt->sub_length << endl;

    sevt->sub_padding = BufferSize/4;//actual number of words;

    //cout<<_boardnumber<<"\t"<<CurrentTime<<endl;
    return  sevt->sub_length;

}


int daq_device_caen_v1730_dpppha::endrun()
{
//    CAEN_DGTZ_SWStopAcquisition(handle);
//    CAEN_DGTZ_ClearData(handle);
//    if ( _broken )
//      {
//        return 0; //  we had a catastrophic failure
//      }
    return _broken;
}

void daq_device_caen_v1730_dpppha::identify(std::ostream& os) const
{
    CAEN_DGTZ_BoardInfo_t       BoardInfo;
}



int daq_device_caen_v1730_dpppha::max_length(const int etype) const
{
    if (etype != m_eventType) return 0;
    return  (14900);
}


// the rearm() function
int  daq_device_caen_v1730_dpppha::rearm(const int etype)
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

void daq_device_caen_v1730_dpppha::addfifo(int fifobufin)
{
    fifobuf=fifobufin;
    int fd = open(myfifo, O_WRONLY|O_NONBLOCK);
    write(fd, &fifobuf, sizeof(fifobuf));
    //printf("Send = %d\n",fifobuf);
}
