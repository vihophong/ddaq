#include "lupoLib.h"
#include "caenvmeA32.h"


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
