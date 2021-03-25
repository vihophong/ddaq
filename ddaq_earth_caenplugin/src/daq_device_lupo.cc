
#include <iostream>

#include <caen_lib.h>

#include <daq_device_lupo.h>

#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#define VME_INTERRUPT_LEVEL      0
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK

#define MAX_MBLT_SIZE            (256*1024)

using namespace std;

daq_device_lupo::daq_device_lupo(const int eventtype
                                       , const int subeventid
                                       , const int linknumber
                                       , const int boardnumber
                                       , const int address
                                       , const int trigger)
{
  m_eventType  = eventtype;
  m_subeventid = subeventid;

  _linknumber = linknumber;
  _boardnumber = boardnumber;

  _warning = 0;
  _broken = 0;

  //! Initiate the VME bridge and perform ADC reset
  cout << "*************** opening VME Bridge" << endl;
  lupo=new lupoLib(address);
  lupo->lupoConnect(linknumber,boardnumber);
  if ( lupo->getConnectStatus() )
    {
      cout << "Connected to VME Bridge!, VME address =  " << std::hex<<"0x"<<address<<std::dec<< endl;
    }else{
      cout<<"Errors!"<<endl;
      exit(0);
  }


  //! no effect for now, reserved for future developements
  //! trigger handler
  _trigger_handler=0;
  if (trigger)   _trigger_handler=1;

  if ( _trigger_handler )
    {
      cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
      _th  = new CAENdrsTriggerHandler(lupo->getHandle(), m_eventType);
      registerTriggerHandler ( _th);
    }
  else
    {
      _th = 0;
    }
}

daq_device_lupo::~daq_device_lupo()
{

  if (_th)
    {
      clearTriggerHandler();
      delete _th;
      _th = 0;
    }

}

int  daq_device_lupo::init()
{

  if ( _broken )
    {
      return 0; //  we had a catastrophic failure
    }
  receivedTrigger = 0;

  triggerCounter = 0;
  deadtimeC = 0;
  timestamp = 0;
  lupo->clearFIFO();
  //lupo->resetTimeStamp();
  lupo->clearTriggerCounter();
  rearm (m_eventType);
  return 0;
}

// the put_data function

int daq_device_lupo::put_data(const int etype, int * adr, const int length )
{

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
    sevt->reserved[0] = -1;//board number (-1 for LUPO)
    sevt->reserved[1] = 0;

    uint32_t buffersize = 0;
    //! read TDC from here!
    int  *d;
    int ipos=0;

    int prev_TC;
    prev_TC=triggerCounter;
    triggerCounter=lupo->readTriggerCounter();

    if (triggerCounter>prev_TC){
        //!read lupo
        //lupo->sendPulse(1);
        //unsigned int prev_TC;
        //prev_TC=triggerCounter;
        triggerCounter=lupo->readTriggerCounter();
        //if ((triggerCounter-prev_TC)>1&&triggerCounter>0){
         //   deadtimeC+=triggerCounter-prev_TC-1;
        //}
        //timestamp=(long long) BinHeader[1]<<32 | BinHeader[2];
        receivedTrigger++;
        d=(int*) &sevt->data;
        *d++ = triggerCounter;
        ipos++;
        *d++ = receivedTrigger;
        ipos++;
        *d++ = (int)lupo->readTimeStampMSB32();
        ipos++;
        *d++ = (int)lupo->readTimeStampLSB32();
        ipos++;
        //printf("%d\t%d\n",triggerCounter, (int)lupo->readTimeStampLSB32());

        lupo->clearFIFO();//1 us
        //lupo->sendPulse(2);
    }else{
        return 0;
    }
    len = buffersize /4;
    sevt->sub_padding = ipos+len;
    len += len%2;
    sevt->sub_length += ipos+len;
    //cout << __LINE__ << "  " << __FILE__ << " returning "  << sevt->sub_length << endl;
    return  sevt->sub_length;
}


int daq_device_lupo::endrun()
{
  if ( _broken )
    {
      return 0; //  we had a catastrophic failure
    }
  //  cout << __LINE__ << "  " << __FILE__ << " ending run " << endl;
}


void daq_device_lupo::identify(std::ostream& os) const
{

}


int daq_device_lupo::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (14900);
}


// the rearm() function
int  daq_device_lupo::rearm(const int etype)
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



