#include <iostream>

#include <fstream>

#include <daq_device_caen_v1740zsp.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#include "fft.h"
#include "X742CorrectionRoutines.h"


#include <cstdlib>

//extern int dc_file[MAX_CH];
//extern int thr_file[MAX_CH];

#define DEFAULT_THR_LOW_1740 200

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


daq_device_caen_v1740zsp::daq_device_caen_v1740zsp(const int eventtype
                                             , const int subeventid
                                             , const int isdppmode // DPP mode (1) or waveform mode (0)
                                             , const int boardid
                                             , const int trigger):WDcfg(),WDrun()// do I give the system trigger?
{

//    const char* evn_fMAX_NEVT_FIFO_SEND=std::getenv("MAX_NEVT_FIFO_SEND");
//    if (evn_fMAX_NEVT_FIFO_SEND)
//        fMAX_NEVT_FIFO_SEND = atoi(evn_fMAX_NEVT_FIFO_SEND);
//    else fMAX_NEVT_FIFO_SEND = 0;

//    if (fMAX_NEVT_FIFO_SEND!=0){
//	cout<<"Attached with FIFO comm, MAX_NEVT_FIFO_SEND = "<<fMAX_NEVT_FIFO_SEND<<endl;
//        //! fifo for interprocess communication
//        sprintf(myfifo,"/tmp/myfifo");
//        mkfifo(myfifo,0666);
//        fifobuf=0;
//        fifocal_prevEventCounter=0;
//    }

    m_eventType  = eventtype;
    m_subeventid = subeventid;


    handle = 0;

    int node = 0;

    _warning = 0;

    ret = CAEN_DGTZ_Success;
    ErrCode= ERR_NONE;
    Nb=0, Ne=0;
    buffer = NULL;
    EventPtr = NULL;
    isVMEDevice= 0;
    nCycles= 0;

    Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */

    Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */
    Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
    FILE *f_ini;

    ReloadCfgStatus = 0x7FFFFFFF; // Init to the bigger positive number

    /* *************************************************************************************** */
    /* Open and parse default configuration file                                                       */
    /* *************************************************************************************** */
    memset(&WDrun, 0, sizeof(WDrun));
    memset(&WDcfg, 0, sizeof(WDcfg));

    _boardnumber = boardid;
    //strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);
    sprintf(ConfigFileName,"m%i.txt",_boardnumber);
    printf("Opening Configuration File %s\n", ConfigFileName);
    f_ini = fopen(ConfigFileName, "r");
    if (f_ini == NULL) {
        ErrCode = ERR_CONF_FILE_NOT_FOUND;
        exit(0);
    }
    ParseConfigFile(f_ini, &WDcfg);
    fclose(f_ini);

    /* *************************************************************************************** */
    /* Open the digitizer and read the board information                                       */
    /* *************************************************************************************** */
    isVMEDevice = WDcfg.BaseAddress ? 1 : 0;

    ret = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, WDcfg.BaseAddress, &handle);
    if (ret) {
        ErrCode = ERR_DGZ_OPEN;
        exit(0);
    }
    //! trigger overlap mode!
    //if (isdppmode ==1) {
    //    WriteRegisterBitmask(handle,0x8000, 0x2, 0x2);
    //    cout<<"Trigger overlap enabled!"<<endl;
    //}


    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    if (ret) {
        ErrCode = ERR_BOARD_INFO_READ;
        exit(0);
    }
    printf("Connected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
    printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

    //! write down handle file
    std::ofstream handlefileout("handle",std::ofstream::out);
    handlefileout<<handle;
    handlefileout.close();
    printf("Write handle file with val = %d\n", handle);


    // Check firmware rivision (DPP firmwares cannot be used with WaveDump */
    sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
    if (MajorNumber >= 128) {
        printf("This digitizer has a DPP firmware\n");
        ErrCode = ERR_INVALID_BOARD_TYPE;
        exit(0);
    }

    // Get Number of Channels, Number of bits, Number of Groups of the board */
    ret = GetMoreBoardInfo(handle, BoardInfo, &WDcfg);
    if (ret) {
        ErrCode = ERR_INVALID_BOARD_TYPE;
        exit(0);
    }

    //Check for possible board internal errors
    ret = CheckBoardFailureStatus(handle, BoardInfo);
    if (ret) {
        ErrCode = ERR_BOARD_FAILURE;
        exit(0);
    }

    //set default DAC calibration coefficients
    for (int i = 0; i < MAX_SET; i++) {
        WDcfg.DAC_Calib.cal[i] = 1;
        WDcfg.DAC_Calib.offset[i] = 0;
    }
    //load DAC calibration data (if present in flash)
    if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)//XX742 not considered
        Load_DAC_Calibration_From_Flash(handle, &WDcfg, BoardInfo);

    // Perform calibration (if needed).
    if (WDcfg.StartupCalibration)
        calibrate(handle, &WDrun, BoardInfo);

    // mask the channels not available for this model
    if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        WDcfg.EnableMask &= (1<<WDcfg.Nch)-1;
    } else {
        WDcfg.EnableMask &= (1<<(WDcfg.Nch/8))-1;
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) && WDcfg.DesMode) {
        WDcfg.EnableMask &= 0xAA;
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) && WDcfg.DesMode) {
        WDcfg.EnableMask &= 0x55;
    }
    // Set plot mask
    if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        WDrun.ChannelPlotMask = WDcfg.EnableMask;
    } else {
        WDrun.ChannelPlotMask = (WDcfg.FastTriggerEnabled == 0) ? 0xFF: 0x1FF;
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE)) {
        WDrun.GroupPlotSwitch = 0;
    }

    // Initialize events and buffers
    Event16 = NULL;
    buffer = NULL;
    evtptr = NULL;

    /* *************************************************************************************** */
    /* program the digitizer                                                                   */
    /* *************************************************************************************** */
    ret = ProgramDigitizer(handle, WDcfg, BoardInfo);
    if (ret) {
        ErrCode = ERR_DGZ_PROGRAM;
        cout<<"Error while programming the Dititizer!"<<endl;
        exit(0);
    }
    usleep(300000);//300 ms

    //check for possible failures after programming the digitizer
    ret = CheckBoardFailureStatus(handle, BoardInfo);
    if (ret) {
            ErrCode = ERR_BOARD_FAILURE;
            cout<<"Board Failure detected!"<<endl;
            exit(0);
    }
    // Select the next enabled group for plotting
    if ((WDcfg.EnableMask) && (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE))
        if( ((WDcfg.EnableMask>>WDrun.GroupPlotIndex)&0x1)==0 )
            GoToNextEnabledGroup(&WDrun, &WDcfg);
    // Read again the board infos, just in case some of them were changed by the programming
    // (like, for example, the TSample and the number of channels if DES mode is changed)
    if(ReloadCfgStatus > 0) {
        ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
        if (ret) {
            ErrCode = ERR_BOARD_INFO_READ;
            exit(0);
        }
        ret = GetMoreBoardInfo(handle,BoardInfo, &WDcfg);
        if (ret) {
            ErrCode = ERR_INVALID_BOARD_TYPE;
            exit(0);
        }

        // Reload Correction Tables if changed
        if(BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE && (ReloadCfgStatus & (0x1 << CFGRELOAD_CORRTABLES_BIT)) ) {
            if(WDcfg.useCorrections != -1) { // Use Manual Corrections
                uint32_t GroupMask = 0;

                // Disable Automatic Corrections
                if ((ret = CAEN_DGTZ_DisableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    exit(0);

                // Load the Correction Tables from the Digitizer flash
                if ((ret = CAEN_DGTZ_GetCorrectionTables(handle, WDcfg.DRS4Frequency, (void*)X742Tables)) != CAEN_DGTZ_Success)
                    exit(0);

                if(WDcfg.UseManualTables != -1) { // The user wants to use some custom tables
                    uint32_t gr;
                                        int32_t clret;

                    GroupMask = WDcfg.UseManualTables;

                    for(gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                        if (((GroupMask>>gr)&0x1) == 0)
                            continue;
                        if ((clret = LoadCorrectionTable(WDcfg.TablesFilenames[gr], &(X742Tables[gr]))) != 0)
                            printf("Error [%d] loading custom table from file '%s' for group [%u].\n", clret, WDcfg.TablesFilenames[gr], gr);
                    }
                }
                // Save to file the Tables read from flash
                GroupMask = (~GroupMask) & ((0x1<<WDcfg.MaxGroupNumber)-1);
                char tmpchar[50];
                sprintf(tmpchar,"X742Table");
                SaveCorrectionTables(tmpchar, GroupMask, X742Tables);
            }
            else { // Use Automatic Corrections
                if ((ret = CAEN_DGTZ_LoadDRS4CorrectionData(handle, WDcfg.DRS4Frequency)) != CAEN_DGTZ_Success)
                    exit(0);
                if ((ret = CAEN_DGTZ_EnableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    exit(0);
            }
        }
    }


    // Allocate memory for the event data and readout buffer
    if(WDcfg.Nbit == 8)
        ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event8);
    else {
        if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
        }
        else {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event742);
        }
    }
    if (ret != CAEN_DGTZ_Success) {
        ErrCode = ERR_MALLOC;
        exit(0);
    }
//    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
//    if (ret) {
//        ErrCode = ERR_MALLOC;
//        exit(0);
//    }

    // No need for the moment
    //if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)//XX742 not considered
    //        Set_relative_Threshold(handle, &WDcfg, BoardInfo);

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


    //! check number of enabled channels
    N_Enable_Channel = 0;
    for(int i=0; i<(WDcfg.Nch/8); i++) {
        if (WDcfg.EnableMask & (1<<i)) {
            N_Enable_Channel++;
        }
    }
    N_Enable_Channel=N_Enable_Channel*8;
    cout<<"Number of enabled channel = "<<N_Enable_Channel<<endl;

    //! check if zero encoding mode is enabled!
    ZS_nsbl = -1;
    const char* env_bl = std::getenv("ZS_BASELINE");
    if(env_bl) {
        cout<<"Software Zero suspression mode enabled!"<<endl;
        ZS_nsbl = atoi(env_bl);
        cout<<"baseline mean= "<<ZS_nsbl<<endl;
    }else{
        cout<<"Zero suspression mode disabled!"<<endl;
    }


    uint32_t recordlength;
    ret |= CAEN_DGTZ_GetRecordLength(handle, &recordlength);
    cout<<"RECORD length = "<< recordlength <<endl;

    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    if (ret) {
            cout<<"Unable to start the boards!"<<endl;
            exit(0);
    }
}



daq_device_caen_v1740zsp::~daq_device_caen_v1740zsp()
{
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_FreeEvent(handle, (void**)&Event8);
    if(WDcfg.Nbit == 8)
        CAEN_DGTZ_FreeEvent(handle, (void**)&Event8);
    else
        if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
            CAEN_DGTZ_FreeEvent(handle, (void**)&Event16);
        }
        else {
            CAEN_DGTZ_FreeEvent(handle, (void**)&Event742);
        }
    if (_th)
    {
        clearTriggerHandler();
        delete _th;
        _th = 0;
    }

}

int  daq_device_caen_v1740zsp::init()
{
    //to arm S-IN start/stop
    //WriteRegisterBitmask(handle,0x8100, 0x4, 0x4);

    // software clear
    //WriteRegisterBitmask(handle,0xEF28,0x1,0x1);

    receivedTrigger=0;

    ret = CAEN_DGTZ_MallocReadoutBuffer(handle,&buffer,&BufferSize);

    return 0;
}

// the put_data function

int daq_device_caen_v1740zsp::put_data(const int etype, int * adr, const int length )
{
    //cout<<"put data"<<endl;
//    if ( _broken )
//    {
//        //      cout << __LINE__ << "  " << __FILE__ << " broken ";
//        //      identify();
//        return 0; //  we had a catastrophic failure
//    }

    if (etype != m_eventType )  // not our id
    {
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
    ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, (char *) buffer, &BufferSize);
    if (ret) {
        ErrCode = ERR_READOUT;
        cout<<"CAEN_DGTZ_ReadData Error!"<<endl;
        //exit(0);
        return 0;
    }
    NumEvents = 0;
    if (BufferSize != 0) {
        ret = CAEN_DGTZ_GetNumEvents(handle, (char *) buffer, BufferSize, &NumEvents);
        if (ret) {
            ErrCode = ERR_READOUT;
            cout<<"CAEN_DGTZ_GetNumEvents Error!"<<endl;
            exit(0);
        }
    }
    else {
        uint32_t lstatus;
        ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
        if (ret) {
            printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
        }
        else {
            if (lstatus & (0x1 << 19)) {
                ErrCode = ERR_OVERTEMP;
                cout<<"ERR_OVERTEMP code!"<<endl;
            }
        }
        return 0;
    }
    receivedTrigger++;

    //! Fill events
    int ipos=0;
    int ipos_prev = 0;
    int  *d=(int*) &sevt->data;
    for (int i=0;i<(int)NumEvents;i++) {
        ret = CAEN_DGTZ_GetEventInfo(handle,buffer,BufferSize,i,&eventInfo,&evtptr);
        ret = CAEN_DGTZ_DecodeEvent(handle,evtptr,(void**)&Event16);
        //*************************************
        // Event Elaboration
        //*************************************
        *d++=eventInfo.BoardId;
        ipos++;
        *d++=eventInfo.ChannelMask;
        ipos++;
        *d++=eventInfo.EventCounter;
        ipos++;
        *d++=eventInfo.EventSize;
        //if (eventInfo.EventSize/4!=2884) cout<<eventInfo.EventSize/4<<endl;
        ipos++;
        *d++=eventInfo.Pattern;
        ipos++;
        *d++=eventInfo.TriggerTimeTag;
        ipos++;

        //! Caution on this!!!!!, onlywork for full data readout of all channels! (loop through data)
        //! ZERO SUPRESSION!
        if (ZS_nsbl>0){ // zero suspression mode for v1740 , module 0 and 1(for WASABI)
            for (int i=0;i<MAX_UINT16_CHANNEL_SIZE;i++){
    //            int chgrp = i/8;
    //            if (((eventInfo.ChannelMask>>chgrp)&0x1)==0) continue;
                if (!checkpositivepulse1740(Event16->DataChannel[i],Event16->ChSize[i],WDcfg.Threshold[i/8],DEFAULT_THR_LOW_1740)){
                    Event16->ChSize[i]=ZS_nsbl;
                }
            }
        }
        //! Caution on this!!!!!, onlywork for full data readout
        memcpy(d++,Event16->ChSize,MAX_UINT16_CHANNEL_SIZE*sizeof(uint32_t));
        ipos += MAX_UINT16_CHANNEL_SIZE;
        ipos_prev = MAX_UINT16_CHANNEL_SIZE -1; // -1 bit is nescessary because of d++
        for (int i=0;i<MAX_UINT16_CHANNEL_SIZE;i++){
            memcpy(d+=ipos_prev,Event16->DataChannel[i],Event16->ChSize[i]*sizeof(uint16_t));
            ipos_prev = Event16->ChSize[i]/2+Event16->ChSize[i]%2;
            ipos+=ipos_prev;
        }
        d+=ipos_prev;
        //ret = CAEN_DGTZ_FreeEvent(handle,(void**)&Event16);
    }



    //! Calculate data rate
    //ret = CAEthouroughput and rate
    Nb += BufferSize;
    Ne += NumEvents;
    CurrentTime = get_time();
    ElapsedTime = CurrentTime - PrevRateTime;
    nCycles++;
    if (ElapsedTime > 1000) {
        ReadoutRate=(float)Nb/((float)ElapsedTime*1048.576f);
        TriggerRate=(float)Ne*1000.0f/(float)ElapsedTime;
        if (Nb == 0)
            printf("No data...\n");
        else
            printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz) - MBLT=%d \n", ReadoutRate, TriggerRate,NumEvents);

        nCycles= 0;
        Nb = 0;
        Ne = 0;
        PrevRateTime = CurrentTime;
    }
    //printf("Nevent = %d\n",NumEvents);
    int padding=ipos%2;
    sevt->sub_padding = NumEvents;
    ipos=ipos+padding;
    sevt->sub_length += ipos;

    return  sevt->sub_length;
}


int daq_device_caen_v1740zsp::endrun()
{
    //CAEN_DGTZ_SWStopAcquisition(handle);
    //CAEN_DGTZ_ClearData(handle);
    if ( _broken )
      {
        return 0; //  we had a catastrophic failure
      }
    return _broken;
}

void daq_device_caen_v1740zsp::identify(std::ostream& os) const
{
    CAEN_DGTZ_BoardInfo_t       BoardInfo;
}



int daq_device_caen_v1740zsp::max_length(const int etype) const
{
    if (etype != m_eventType) return 0;
    return  (14900);
}


// the rearm() function
int  daq_device_caen_v1740zsp::rearm(const int etype)
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

bool daq_device_caen_v1740zsp::checkpositivepulse1740(uint16_t* pulse,int nsample, int threshold, int thresholdlow){
    bool ret=false;
    for (int i=0;i<nsample;i++){
        if(pulse[i]>threshold){
            ret=true;
            break;
        }
        if (pulse[i]<thresholdlow){
            ret=true;
            break;
        }
    }
    return ret;
}

//void daq_device_caen_v1740zsp::addfifo(int fifobufin)
//{
//    fifobuf=fifobufin;
//    int fd = open(myfifo, O_WRONLY|O_NONBLOCK);
//    write(fd, &fifobuf, sizeof(fifobuf));
//    //printf("Send = %d\n",fifobuf);
//}
