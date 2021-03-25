#include "caenvmeA32.h"
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
