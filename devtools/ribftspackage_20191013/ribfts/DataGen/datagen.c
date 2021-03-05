

#ifdef WIN32
#include <winsock2.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <time.h>

#include "transfer.h"

/*
    The procedures  signal_block   &   msgDefineMessage  are  called from the libxfer library  
      For this example code they can be noopos
*/

#ifndef WIN32
void signal_block ()  {}
#endif

int msgDefineMessage (u_int id, u_int xclass, u_int level, char *source, char *body) {

//  printf("id=%d, class=%d, level=%d, source=%s: %s\n",id, xclass, level, source, body);
  return 0;
}



void Usage(char *progname) {
    fprintf(stderr,"Usage\n%s -n [server] -p [port] -f [file_name] -d [format] -m [mode] -t [ticks] -s [buffersize] -b [blocks] - i [IDs] -I [ID] -w [w] -v [v]\n",progname);
    fprintf(stderr,"\tserver is the IP address or name of server (default localhost)\n");
    fprintf(stderr,"\tport is the base TCP port to use (default 10305)\n");
    fprintf(stderr,"\tfile_name defines a file to be read for data rather than using internally generated data (default no file - use internal data)\n");
    fprintf(stderr,"\tformat defines the data to be generated (default 0)\n");
    fprintf(stderr,"\tmode is the protocol transfer option (default 3)\n");
    fprintf(stderr,"\tticks is the delay between sending data blocks in millisecs (default 10; 0 => no delay)\n");
    fprintf(stderr,"\tbuffersize is data block size in Kbytes (default 64)\n");
    fprintf(stderr,"\tblocks is the number of blocks sent at each tick (default 1) \n");
    fprintf(stderr,"\tIDs is the number of data streams to generate (default 1)\n");
    fprintf(stderr,"\tID is the ID of the first data stream generated (default 0)\n");
    fprintf(stderr,"\tw is the starting size of the TCP send & receive windows in Kbytes (default 512)\n");
    fprintf(stderr,"\tv is the setting for the verbose flag (default 0)\n");
    fprintf(stderr,"mode = 5 is for GANIL\n");
    exit(1);
}
        
int main(int argc, char **argv) {

   int ID = 0;
   int IDs = 1;
   int id;

   int  Sync;
   int Ctime;

    char Buffer[(64*1024)+32];
    int * BufferL;
    short * BufferS;
    char *TS_Server= (char*)"localhost";
    int Port=10305;
    int InFlag =0;     /* no file input by default  */
    int Format = 0;
    FILE *InStream;
    char *FileName= (char*)"/tmp/in.dat";
    char fname[32];
    unsigned int stream = 0;
    int Mode=3;
    int DelayTicks=10;     /* 10 millisecs  */
    int NumBlocks=1;
    int verbose=0;
    int s;
    int i, j;
    int BufferSize=64*1024;
    int SndBufSize=512;
    int RcvBufSize=512;
    int num;
    int b;
    int rc;
    int Fblocks;

    struct timeval Time;
    int InitialTime;
    int diff;

#ifndef WIN32
    struct timespec time_request;
#endif

    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(argv[i][1]) {
                    case 'f':
                        FileName = argv[++i];
                        InFlag = 1;
                        break;
                    case 'd':
                        Format = atoi(argv[++i]);
                        break;
                    case 'n':
                        TS_Server = argv[++i];
                        break;
                    case 'p':
                        Port = atoi(argv[++i]);
                        break;
                    case 'm':
                        Mode = atoi(argv[++i]);
                        break;
                    case 't':
                        DelayTicks = atoi(argv[++i]);
                        break;
                    case 'b':
                        NumBlocks = atoi(argv[++i]);
                        break;
                    case 's':
                        BufferSize = atoi(argv[++i])*1024;
                        break;
                    case 'i':
                        IDs = atoi(argv[++i]);
                        break;
                    case 'I':
                        ID = atoi(argv[++i]);
                        break;
                    case 'w':
                        SndBufSize = atoi(argv[++i]);
                        RcvBufSize = atoi(argv[++i]);
                        break;
                    case 'v':
                        verbose = 1;
                        break;
                    default:
                        Usage(argv[0]);
                        break;
                }
            }
            else
                Usage(argv[0]);
        }
    }

     for (id=ID; id<IDs; id++) {

    (void) transferSetVerbose(verbose);
    (void) transferUseOverlap(0);
    (void) transferNice(1);
    (void) transferMultiBlockSize(id, BufferSize);
    (void) transferMultiSndBufSize(id, SndBufSize);
    (void) transferMultiRcvBufSize(id, RcvBufSize);
    (void) transferMultiMode(id, Mode);
    (void) transferMultiPort(id, Port+id);
    (void) transferMultiInit(id, TS_Server);
    printf ("Resetting communication with server %s\n",TS_Server);

    }
    
#ifdef WIN32
    time(&Time.tv_sec);
#else
    gettimeofday(&Time, NULL);
#endif

    BufferL = (int *) &Buffer[0];
    BufferS = (short *) &Buffer[0];

    InitialTime = Time.tv_sec;
    printf("Start at %d\n", InitialTime);

    num = 0;
    Sync = 0;
    Fblocks = 0;

   if (InFlag != 0) {
         printf("reading from %s\n",FileName);
         sprintf(fname,"%s",FileName);
         InStream = fopen(fname,"r");
   }
    
    for (;;)
    {

       if (DelayTicks) {
#ifdef WIN32
           Sleep((DWORD)DelayTicks);
#endif
#ifdef SOLARIS
           (void) usleep(DelayTicks * 1000);
#endif
#ifdef LINUX
           time_request.tv_sec = DelayTicks/1000;
           time_request.tv_nsec = (DelayTicks - (time_request.tv_sec * 1000)) * 1000000;
           i = nanosleep(&time_request, NULL);
#endif
#ifdef LYNXOS
           time_request.tv_sec = DelayTicks/1000;
           time_request.tv_nsec = (DelayTicks - (time_request.tv_sec * 1000)) * 1000000;
           i = nanosleep(&time_request, NULL);
#endif
        }
         
     for (b=0; b<NumBlocks; b++)
     {
       if (verbose) printf("Transfer %d bytes\n",BufferSize-32);

       Ctime = Sync;

       for (id=ID; id<IDs; id++) {

         Sync = Ctime;

        if (InFlag == 0) {

          switch (Format) {
            default:
            case 0:

              for (i=8; i<BufferSize/4; ) {
                 BufferL[i] = 0x80000000 + (id << 24) + (4 << 20) + ((Sync >> 10) & 0x000fffff) ; i++;
                 BufferL[i] = (Sync << 18) & 0x0fffffff; i++;

                 Sync++;
              }
             break;
           }

         }  /* end of InFlag == 1  */

        if (InFlag == 1) {

retry:

          switch (Format) {
            default:
            case 0:

              rc = fread(&Buffer[8],1,BufferSize,InStream);  /*  note 32-24 = 8 */
              Fblocks++;
              if (rc == 0) {
                  printf("end of file after %d blocks\n",Fblocks);
                  fseek(InStream,0,0);
                  Fblocks=0;
                  goto retry;
              }
              if (rc != BufferSize) {printf("fread returns rc=%d\n",rc);}

             break;
            case 1:

              rc = fread(&Buffer[32],1,BufferSize,InStream);
              Fblocks++;
              if (rc == 0) {
                  printf("end of file after %d blocks\n",Fblocks);
                  fseek(InStream,0,0);
                  Fblocks=0;
                  goto retry;
              }
              if (rc != BufferSize) {printf("fread returns rc=%d\n",rc);}

             break;
           }

         }   /* end of InFlag == 1  */

         if (verbose) printf("m=%d; 0x%x 0x%x \n", id, BufferL[8], BufferL[9]);
         s = transferMultiTxData (id, &Buffer[32] , stream, BufferSize-32);
       }

       if (verbose) printf("Done with code=%d\n",s);
         num++;
         if ((num/1000)*1000 == num) {

#ifdef WIN32
             time(&Time.tv_sec);
#else
             gettimeofday(&Time, NULL);
#endif

             diff = Time.tv_sec-InitialTime;
             printf ("sent %d blocks after %d\n", num, diff);
         }
       }

    }

}
