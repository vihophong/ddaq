#include <QCoreApplication>
#include "daq.h"
int main(int argc, char *argv[])
{
    char configFilename[MAX_NBOARD][500];
    if (argc>1){
        for (int i=0;i<argc-1;i++){
            strcpy(configFilename[i],argv[i+1]);
        }
      }else{
          printf("Please enter input file!\n");
          return 0;
      }
    daq daq_t;
    daq_t.daqinit(argc-1,configFilename);
    daq_t.daqloop();
}

