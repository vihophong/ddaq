#ifndef LUPOLIB_H
#define LUPOLIB_H
#include <unistd.h>
#include <stdint.h>   /* C99 compliant compilers: uint64_t */
#include <ctype.h>    /* toupper() */
#include <sys/time.h>

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
    void setLupoAddress(unsigned int addr){lupoAddress=addr;}


private:
    unsigned int lupoAddress;
    int Handle;
    int isConnected;
};

#endif // LUPOLIB_H
