#include <stdio.h>

#include "bb-decode-V7XX.h"

int get_chdata_V7XX(int geo, int ch, char *buf, int size){
  unsigned int *evtdata = (unsigned int*)buf;
  unsigned int evtsize = (unsigned int)size/4;
  unsigned int i;
  int ih, igeo, ich, idata;
  int evtflag = 0;
  
  for(i=0; i<evtsize; i++) {
    ih = (evtdata[i]&0x06000000);
    if (ih == 0x02000000) {
      igeo = (evtdata[i]&0xf8000000)>>27;
      evtflag = 1;
    } else if (ih == 0 && evtflag == 1) {
      ich = (evtdata[i]&0x001f0000) >> 16;  // 2014.05.02 H.B.
      idata = evtdata[i]&0x1fff;
      //printf("geo=%d ch=%d data=%d\n", igeo, ich, idata);
      if(geo == igeo && ch == ich){
	return idata;
      }
    } else if (ih == 0x04000000) {
      evtflag = 0;
    }
  }
  return -1;
}
