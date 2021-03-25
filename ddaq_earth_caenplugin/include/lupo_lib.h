#ifndef __LUPO_LIB__
#define __LUPO_LIB__
#include <stdio.h>

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
    unsigned int  getLupoAddress(){return lupoAddress;}

private:
    unsigned int lupoAddress;
    int Handle;
    int isConnected;
};


#endif
