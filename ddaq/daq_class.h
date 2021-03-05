#ifndef DAQ_CLASS_H
#define DAQ_CLASS_H

#define MAX_NBOARD       8
#define MaxNChannels    16
#define WF_BLOCK_LENGTH 2000//4085 //maximum 8170
#define DGTZ_BLOCK_LENGTH 15 //maximum 8170
#define MAX_BLOCK       8

#define WF_WRITELENGTH  500

#define BLOCK_EMPTY     0
#define BLOCK_WRITING   1
#define BLOCK_WAITING   2
#define SHMKEY 75


//For share memory
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <time.h>




#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32

    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
    #include <process.h>
    #define getch _getch     /* redefine POSIX 'deprecated' */
    #define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
#endif
typedef struct buffer_type_t{
  unsigned int status,blockno;
  /*
                 uint32_t BinHeader[10];
                 BinHeader[0] = b;//Board number
                 BinHeader[1] = ch;//Channel number
                 BinHeader[2] = shmp->TrgCnt[b][ch];//event Number
                 BinHeader[3] = Events_t[ch][ev].TimeTag;//TTT LSB
                 BinHeader[4] = Events_t[ch][ev].Extras; //16bit TTT MSB + 16bit baseline value multiply by 4
                 BinHeader[5] = (uint32_t)Events_t[ch][ev].Baseline;
                 BinHeader[6] = (uint32_t)Events_t[ch][ev].ChargeLong;
                 BinHeader[7] = (uint32_t)Events_t[ch][ev].ChargeShort;
                 BinHeader[8] = (uint32_t)Events_t[ch][ev].Pur;
                 BinHeader[9] = (uint32_t)Events_t[ch][ev].Format;
  */
  uint16_t wfdata[WF_BLOCK_LENGTH];
  uint32_t dgtzdata[DGTZ_BLOCK_LENGTH];
  double time_data[2];
} buffer_type;
typedef struct SHM_DGTZ_S{
  uint32_t pid;
  uint32_t  runno;
  uint32_t  status;
  time_t  start_time;
  time_t  stop_time;
  uint32_t  block_no;

  uint32_t channelMask[MAX_NBOARD];
  char config_file_name[MAX_NBOARD][500];
  char raw_projectName[756];
  char raw_saveDir[756];
  char raw_filename[756];
  FILE *raw_fd;

  buffer_type_t buffer[MAX_BLOCK];
  uint32_t TrgCnt[MAX_NBOARD][MaxNChannels];
  uint32_t com_flag;
  time_t com_time;
  char com_filename[500];
  char com_comments[500];
  char com_message[500]; //interface for start/stop daq

  uint32_t  preTrg[MAX_NBOARD][MaxNChannels];
  uint32_t  recordLength[MAX_NBOARD];
  int  preGate[MAX_NBOARD][MaxNChannels];
  int shortGate[MAX_NBOARD][MaxNChannels];
  int longGate[MAX_NBOARD][MaxNChannels];
  uint8_t pulsePolarity[MAX_NBOARD][MaxNChannels];

  int ana_status;
  int ana_flag;
  char ana_message[500];

  //new add
  int nboard;

} SHM_DGTZ;
class daq
{
public:
    SHM_DGTZ* shmp;
    daq();
};

#endif // DAQ_CLASS_H
