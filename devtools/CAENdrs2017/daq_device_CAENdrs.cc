
#include <unistd.h>
#include "stdlib.h"
#include <iostream>

#include <daq_device_CAENdrs.h>
#include <string.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>


#define VME_INTERRUPT_LEVEL      1
#define VME_INTERRUPT_STATUS_ID  0xAAAA
#define INTERRUPT_MODE           CAEN_DGTZ_IRQ_MODE_ROAK

const unsigned int lupoaddr = 0x111000;
const unsigned int tdcaddr = 0x3333;
const unsigned int qdcaddr = 0x2200;

using namespace std;





int init_nbbqvio(int* ret, short Link, short Device){

  CVBoardTypes VMEBoard = cvV2718;
  int BHandle;
  if( CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess )
  {
      //printf("\n\n Error opening the device\n");
      *ret=0;
  }else{
      *ret=1;
  }
  return BHandle;
}

unsigned short vme_read16(int BHandle,unsigned int addr){
  short Data;
  CAENVME_ReadCycle(BHandle,addr,&Data,cvA32_U_DATA,cvD16);
  return Data;
}

unsigned int vme_read32(int BHandle,unsigned int addr){
  int Data;
  CAENVME_ReadCycle(BHandle,addr,&Data,cvA32_U_DATA,cvD32);
  return Data;
}

int vme_readBlock32(int BHandle,unsigned int addr,unsigned int* Data,int size){
  int count;
  CAENVME_BLTReadCycle(BHandle,addr,Data,size,cvA32_U_DATA,cvD32,&count);
  return count;
}

int vme_write16(int BHandle,unsigned int addr,unsigned short sval){
  CAENVME_WriteCycle(BHandle,addr,&sval,cvA32_U_DATA,cvD16);
  return 0;
}

int vme_write16a16(int BHandle,unsigned int addr,unsigned short sval){
  CAENVME_WriteCycle(BHandle,addr,&sval,cvA16_U,cvD16);
  return 0;
}

int vme_write32(int BHandle,unsigned int addr,unsigned int lval){
  CAENVME_WriteCycle(BHandle,addr,&lval,cvA32_U_DATA,cvD32);
  return 0;
}

int vme_amsr(unsigned int lval){
  //am = val;
  return 0;
}

void release_nbbqvio(int BHandle){
  CAENVME_End(BHandle);
}

int vread32(int BHandle,unsigned int addr, int *val){
  *val = vme_read32(BHandle,addr);
  return *val;
}

short vread16(int BHandle,unsigned int addr, short *val){
  *val = vme_read16(BHandle,addr);
  return *val;
}

void vwrite32(int BHandle,unsigned int addr, int *val){
  vme_write32(BHandle,addr, *val);
}

void vwrite16(int BHandle,unsigned int addr, short *val){
  vme_write16(BHandle,addr, *val);
}

void vwrite16a16(int BHandle,unsigned int addr, short *val){
  vme_write16a16(BHandle,addr, *val);
}

// size in character
int dma_vread32(int BHandle,unsigned int addr, char *buff, int size){
  int rsz;

  CAENVME_BLTReadCycle(BHandle, addr, buff, size, cvA32_U_BLT, cvD32, &rsz);

  return rsz;
}

lupoLib::lupoLib(unsigned int lpAddr):lupoAddress(lpAddr),Handle(0),isConnected(0)
{
}

void lupoLib::lupoConnect(short linkNum, short boardN)
{
    int ret=0;
    Handle=init_nbbqvio(&ret,linkNum,boardN);
    isConnected=ret;
}
unsigned int lupoLib::readTimeStampMSB32(){
    return vme_read32(Handle,lupoAddress<<8|0x0c);
}
unsigned int lupoLib::readTimeStampLSB32(){
    return vme_read32(Handle,lupoAddress<<8|0x08);
}

unsigned int lupoLib::readTriggerCounter(){
    return vme_read32(Handle,lupoAddress<<8|0x10);
}

void lupoLib::resetTimeStamp(){
    vme_read32(Handle,lupoAddress<<8|0x92);
}


void lupoLib::sendPulse(int outputNum){
    if (outputNum==1) vme_write16(Handle,lupoAddress<<8|0x10,0x1); else if(outputNum==2) vme_write16(Handle,lupoAddress<<8|0x10,0x2);
}

void lupoLib::clearTriggerCounter(){
    vme_read32(Handle,lupoAddress<<8|0x94);
}

void lupoLib::clearFIFO(){
    vme_read32(Handle,lupoAddress<<8|0x90);
}
void lupoLib::clearAll(){
    vme_read32(Handle,lupoAddress<<8|0x96);
}
void lupoLib::closeLUPO(){
    release_nbbqvio(Handle);
}


//!--------------------------v1190lib-----------------------------
v1190lib::v1190lib(unsigned int Addr):Handle(0),isConnected(0)
{
    mAddr = Addr<<16;
}
void v1190lib::connect(short linkNum, short boardNum){
    int ret=0;
    Handle=init_nbbqvio(&ret,linkNum,boardNum);
    isConnected=ret;
}
void v1190lib::init()
{
  //system("./v1190.sh");//external command here
}

int v1190lib::blockread_segdata(char* blkdata,int cnt)
{
    int sz, rsz;
    sz = cnt *4;
    rsz = dma_vread32(Handle,mAddr,blkdata, sz);
    return rsz;//return size
}

int v1190lib::read_segdata(int *data)
{
    vread32(Handle,mAddr + V1190_OUTBUFF, data);
    return 1;
}

void v1190lib::clear(){
  short sval;
  sval = V1190_SOFT_CLEAR_BIT;
  vwrite16(Handle,mAddr + V1190_SOFT_CLEAR, &sval);
}

int v1190lib::checkdataready()
{
    short sval;
    vread16(Handle,mAddr + V1190_DATA_RDY,&sval);
    return (int)(sval&0x1);
}

int v1190lib::checkberr()
{
    short sval;
    vread16(Handle,mAddr + V1190_DATA_RDY,&sval);
    return (int)((sval>>10)&0x1);
}
//!-------------------------------------------------------------


//!--------------------------v792lib----------------------------
v792lib::v792lib(unsigned int Addr):Handle(0),isConnected(0)
{
    mAddr = Addr<<16;
}
void v792lib::connect(short linkNum, short boardNum){
    int ret=0;
    Handle=init_nbbqvio(&ret,linkNum,boardNum);
    isConnected=ret;
}
void v792lib::init()
{
  //system("./v792.sh");//external command here
}

int v792lib::blockread_segdata(char* blkdata,int cnt)
{
    int sz, rsz;
    sz = cnt *4;
    rsz = dma_vread32(Handle,mAddr,blkdata, sz);
    return rsz;//return size
}
int v792lib::read_segdata(int *data)
{
    vread32(Handle,mAddr + V792_OUTBUFF, data);
    return 1;
}
void v792lib::clear(){
  short sval;
  sval = 0x04;

  vwrite16(Handle,mAddr + V792_BIT_SET2, &sval);
  vwrite16(Handle,mAddr + V792_BIT_CLE2, &sval);
}
void v792lib::noberr(){
  short sval = 0x00;

  vwrite16(Handle,mAddr + V792_CTRL_REG1, &sval);
}

void v792lib::multievtberr(){
  short sval = 0x20;

  vwrite16(Handle,mAddr + V792_CTRL_REG1, &sval);
}
void v792lib::intlevelmulti(short level, short evtn){
    vwrite16(Handle,mAddr + V792_EVT_TRIG_REG, &evtn);
    vwrite16(Handle,mAddr + V792_INT_REG1, &level);
}
void v792lib::intlevel(short level){
    this->intlevelmulti(level, 1);
}

//!-------------------------------------------------------------

daq_device_CAENdrs::daq_device_CAENdrs(const int eventtype
                       , const int subeventid
                       , const int linknumber
                       , const int trigger)
{

  m_eventType  = eventtype;
  m_subeventid = subeventid;

  _linknumber = linknumber;

  handle = 0;
  _Event742 = 0;

  int node = 0;

  _warning = 0;

  lupo=new lupoLib(lupoaddr);
  lupo->lupoConnect(3,0);

#ifdef TDCREADOUT
  tdc = new v1190lib(tdcaddr);
  tdc->setHandle(lupo->getHandle());
  tdc->init();
#endif
#ifdef QDCREADOUT
  qdc = new v792lib(qdcaddr);
  qdc->setHandle(lupo->getHandle());
  qdc->init();
#endif
  if (lupo->getConnectStatus()==1) {
      printf("Connected!\n");
  }else{
      printf("Error!\n");
      exit(0);
  }
  /*

  cout << "*************** opening Digitizer" << endl;
  _broken = CAEN_DGTZ_OpenDigitizer( CAEN_DGTZ_PCI_OpticalLink, _linknumber , node, 0 ,&handle);
  cout << "*************** " << _broken  << endl;



  _broken =  CAEN_DGTZ_Reset(handle);

  if ( _broken )
    {
      cout << " Error in CAEN_DGTZ_OpenDigitizer " << _broken << endl;
      return;
    }
    */
  _trigger_handler=0;
  if (trigger)   _trigger_handler=1;

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

}

daq_device_CAENdrs::~daq_device_CAENdrs()
{

  if (_th)
    {
      clearTriggerHandler();
      delete _th;
      _th = 0;
    }
/*
  if ( _Event742)
    {
      CAEN_DGTZ_FreeEvent(handle, (void**)&_Event742);
    }
  CAEN_DGTZ_CloseDigitizer(handle);
*/
}


int  daq_device_CAENdrs::init()
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
#ifdef TDCREADOUT
  tdc->clear();
#endif
#ifdef QDCREADOUT
  qdc->clear();
  qdc->noberr();
  qdc->multievtberr();
  qdc->intlevel(VME_INTERRUPT_LEVEL);
#endif
  //lupo->sendPulse(1);
  // and we trigger rearm with our event type so it takes effect
  rearm (m_eventType);
  return 0;

}

// the put_data function

int daq_device_CAENdrs::put_data(const int etype, int * adr, const int length )
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
  sevt->sub_decoding = 85;
  sevt->reserved[0] = 0;
  sevt->reserved[1] = 0;

  uint32_t buffersize = 0;
  //! read TDC from here!
  int  *d;
  int ipos=0;

  unsigned int prev_TC;
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

      lupo->clearFIFO();//1 us      
#ifdef TDCREADOUT
      //! read tdc
      buffersize=tdc->blockread_segdata((char*)d++,256);
      tdc->clear();
      d+=buffersize/4-1;
#endif
#ifdef QDCREADOUT
      //! identifier bit for QDC
      *d++ = V1190_792_SEPARATION_BIT;
      ipos++;
      //! read qdc
      uint32_t qdcbuffersize=qdc->blockread_segdata((char*)(d++),34);
      qdc->clear();
      buffersize+=qdcbuffersize;
      d+=qdcbuffersize/4-1;
      //*d = V1190_792_SEPARATION_BIT;
      //ipos++;
#endif

      lupo->sendPulse(1);
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


int daq_device_CAENdrs::endrun()
{
  if ( _broken )
    {
      return 0; //  we had a catastrophic failure
    }
  //! send stop pulse for digitizer
  //lupo->sendPulse(2);


  return _broken;
}


void daq_device_CAENdrs::identify(std::ostream& os) const
{

  CAEN_DGTZ_BoardInfo_t       BoardInfo;

/*
  if ( _broken)
    {
      os << "CAEN Digitizer Event Type: " << m_eventType
     << " Subevent id: " << m_subeventid
     << " ** not functional ** " << endl;
    }
  else
    {
      int ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
      if ( ret )
    {
      cout << " Error in CAEN_DGTZ_GetInfo" << endl;
      return;
    }

      os << "CAEN Digitizer Model " << BoardInfo.ModelName
     << " Event Type: " << m_eventType
     << " Subevent id: " << m_subeventid
     << " Firmware "     << BoardInfo.ROC_FirmwareRel << " / " << BoardInfo.AMC_FirmwareRel
     << " speed "  << getGS() <<  "GS"
     << " delay "  << getDelay() <<  "% ";
      if (_trigger_handler) os << " *Trigger" ;
      if (_warning) os << " **** warning - check setup parameters ****";
      os << endl;

    }
*/
}


int daq_device_CAENdrs::getDelay() const
{
  unsigned int i;
  int status =  CAEN_DGTZ_GetPostTriggerSize(handle, &i);
  return i;
}

int daq_device_CAENdrs::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (14900);
}


// the rearm() function
int  daq_device_CAENdrs::rearm(const int etype)
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

int daq_device_CAENdrs::SetConfigRegisterBit( const int bit)
{
  const unsigned int forbidden_mask = 0x0FFFE7B7;  // none of these bits must be touched
  unsigned int pattern = 1<<bit;
  if ( pattern & forbidden_mask)
    {
      cout << " attemt to set reserved bit: " << bit << endl;
      return 1;
    }

  return CAEN_DGTZ_WriteRegister(handle,CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, pattern);
}

int daq_device_CAENdrs::ClearConfigRegisterBit( const int bit)
{
  const unsigned int forbidden_mask = 0x0FFFE7B7;  // none of these bits must be touched
  unsigned int pattern = 1<<bit;
  if ( pattern & forbidden_mask)
    {
      cout << " attemt to set reserved bit: " << bit << endl;
      return 1;
    }

  return CAEN_DGTZ_WriteRegister(handle,CAEN_DGTZ_BROAD_CH_CLEAR_CTRL_ADD, pattern);
}


float daq_device_CAENdrs::getGS() const
{
  if ( _broken )
    {
      //      cout << __LINE__ << "  " << __FILE__ << " broken ";
      //      identify();
      return 0; //  we had a catastrophic failure
    }


  CAEN_DGTZ_DRS4Frequency_t  mode;
  int status =  CAEN_DGTZ_GetDRS4SamplingFrequency(handle, &mode);
  switch (mode)
    {
    case CAEN_DGTZ_DRS4_5GHz:
      return 5;
      break;

    case CAEN_DGTZ_DRS4_2_5GHz:
      return 2.5;
      break;

    case CAEN_DGTZ_DRS4_1GHz:
      return 1;
      break;

    default:
      return -1;
    }
  return -1;
}



