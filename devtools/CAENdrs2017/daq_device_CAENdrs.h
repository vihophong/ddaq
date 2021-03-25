#ifndef __DAQ_DEVICE_CAENDRS__
#define __DAQ_DEVICE_CAENDRS__

//Set here to enable TDC readout
//#define TDCREADOUT
//#define QDCREADOUT


#include <daq_device.h>
#include <stdio.h>
#include <CAENdrsTriggerHandler.h>
#include <CAENDigitizerType.h>

#include <unistd.h>
#include <stdint.h>   /* C99 compliant compilers: uint64_t */
#include <ctype.h>    /* toupper() */
#include <sys/time.h>
#include <iostream>

//#include "caenvmeA32.h"

#define V1190_792_SEPARATION_BIT 0x90000546
/* Register */
#define V1190_OUTBUFF         0x0000
#define V1190_SOFT_CLEAR      0x1016
#define V1190_ALMOST_FULL     0x1022
#define V1190_INTLEVEL        0x100a
#define V1190_CTRL_REG        0x1000
#define V1190_DATA_RDY        0x1002
/* Bit */
#define V1190_SOFT_CLEAR_BIT  1

#define V1190_TYPE_MASK_S          0xf800
#define V1190_GLOBAL_HEADER_BIT_S  0x4000
#define V1190_TDC_HEADER_BIT_S     0x0800
#define V1190_TDC_DATA_BIT_S       0x0000
#define V1190_TDC_TRAILER_BIT_S    0x1800
#define V1190_TDC_ERROR_BIT_S      0x2000
#define V1190_GLOBAL_TRAILER_BIT_S 0x8000

#define V1190_TYPE_MASK          0xf8000000
#define V1190_GLOBAL_HEADER_BIT  0x40000000
#define V1190_TDC_HEADER_BIT     0x08000000
#define V1190_TDC_DATA_BIT       0x00000000
#define V1190_TDC_TRAILER_BIT    0x18000000
#define V1190_TDC_ERROR_BIT      0x20000000
#define V1190_GLOBAL_TRAILER_BIT 0x80000000

/* Register */
#define V792_OUTBUFF        0x0000   /* - 0x07ff (D32) */
#define V792_BIT_SET2       0x1032
#define V792_BIT_CLE2       0x1034

#define V792_CTRL_REG1      0x1010
#define V792_EVT_TRIG_REG   0x1020
#define V792_INT_REG1       0x100a

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "CAENVMEtypes.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "CAENVMElib.h"
#ifdef __cplusplus
}
#endif

int init_nbbqvio(int* ret, short Link, short Device);
unsigned short vme_read16(int BHandle,unsigned int addr);
unsigned int vme_read32(int BHandle,unsigned int addr);
int vme_readBlock32(int BHandle,unsigned int addr,unsigned int* Data,int size);
int vme_write16(int BHandle,unsigned int addr,unsigned short sval);
int vme_write16a16(int BHandle,unsigned int addr,unsigned short sval);
int vme_write32(int BHandle,unsigned int addr,unsigned int lval);
int vme_amsr(unsigned int lval);
int vread32(int BHandle,unsigned int addr, int *val);
short vread16(int BHandle,unsigned int addr, short *val);
void vwrite32(int BHandle,unsigned int addr, int *val);
void vwrite16(int BHandle,unsigned int addr, short *val);
void vwrite16a16(int BHandle,unsigned int addr, short *val);
void release_nbbqvio(int BHandle);
int dma_vread32(int BHandle,unsigned int addr, char *buff, int size);


class lupoLib
{
public:
    lupoLib(unsigned int lpAddr);
    void lupoConnect(short linkNum, short boardNum);
    unsigned int readTimeStampMSB32();
    unsigned int readTimeStampLSB32();
    unsigned int readTriggerCounter();
    void resetTimeStamp();
    void clearFIFO();
    void clearTriggerCounter();
    void clearAll();
    void closeLUPO();
    void sendPulse(int outputNum);
    int getHandle(){return Handle;}
    int getConnectStatus(){return isConnected;}
    void setLupoAddress(unsigned int addr){
        lupoAddress=addr;
    }


private:
    unsigned int lupoAddress;
    int Handle;
    int isConnected;
};

class v1190lib
{
public:
    v1190lib(unsigned int Addr);
    void connect(short linkNum, short boardNum);
    void init();

    int getHandle(){return Handle;}
    void setHandle(int bhandle){Handle = bhandle;}
    int getConnectStatus(){return isConnected;}
    int read_segdata(int *data);
    int blockread_segdata(char* blkdata,int cnt);
    void clear();
    int checkdataready();
    int checkberr();

private:
    unsigned int mAddr;
    int Handle;
    int isConnected;
};

class v792lib
{
public:
    v792lib(unsigned int Addr);
    void connect(short linkNum, short boardNum);
    void init();

    int getHandle(){return Handle;}
    void setHandle(int bhandle){Handle = bhandle;}
    int getConnectStatus(){return isConnected;}

    int read_segdata(int *data);
    int blockread_segdata(char* blkdata,int cnt);
    void clear();

    void noberr();
    void multievtberr();
    void intlevelmulti(short level, short evtn);
    void intlevel(short level);
private:
    unsigned int mAddr;
    int Handle;
    int isConnected;
};


class daq_device_CAENdrs: public  daq_device {


public:

  daq_device_CAENdrs(const int eventtype
             , const int subeventid
             , const int linknumber = 0
             , const int trigger = 1);

  ~daq_device_CAENdrs();


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const;

  // functions to do the work

  int put_data(const int etype, int * adr, const int length);

  int init();
  int rearm( const int etype);
  int endrun();

 protected:

  int ClearConfigRegisterBit( const int bit);
  int SetConfigRegisterBit( const int bit);
  float getGS() const;
  int getDelay() const;

  int _broken;
  int _warning;

  subevtdata_ptr sevt;

  int handle;

  int _trigger;
  int _trigger_handler;
  int _linknumber;
  CAEN_DGTZ_DRS4Frequency_t _speed;

  CAEN_DGTZ_X742_EVENT_t *_Event742;
  uint32_t AllocatedSize, BufferSize, NumEvents;

  lupoLib* lupo;
  v1190lib* tdc;
  v792lib* qdc;

  int triggerCounter;
  int receivedTrigger;
  unsigned int deadtimeC;
  long long timestamp;

  CAENdrsTriggerHandler *_th;

};



#endif
