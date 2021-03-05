

#ifdef WIN32
#include <winsock2.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <time.h>

#include "transfer.h"
#include "dataspy.h"

//    24 byte data block header - used for data blocks in shared memory and data blocks on disc or tape.

typedef struct s_data_header {
    char header_id[8];
    int header_sequence;
    short  header_stream;
    short  header_tape;
    short  header_MyEndian;
    short  header_DataEndian;
    int header_dataLen;
} DATA_HEADER;

//    32 byte header - used when transfering data blocks using TCP

typedef struct s_block_header {              /* format of data block  */
        unsigned short hdr_flags;          /* see below */
        unsigned short hdr_stream;         /* =0 for forced ack request or 1=>MAX_STREAM  */
        unsigned short hdr_endian;         /*   data byte order */
        unsigned short hdr_ID;
        unsigned int hdr_sequence;        /* for this stream */
        unsigned int hdr_blocklength;     /*  total length of this block including the header */
        unsigned int hdr_datalength;      /*  length of user data in the block  */
        unsigned int hdr_offset;          /*  very large blocks may be fragmented  */
        unsigned int hdr_id1;             /*  for spy to locate header  =0x19062002 */
        unsigned int hdr_id2;             /*  for spy to locate header  =0x09592400 */
} XFER_HEADER;


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
    fprintf(stderr,"Usage\n%s -n server -p [port] -i [id] -I [ID] -s [buffer size]\n",progname);
    fprintf(stderr,"\tserver is the IP address or name of destination\n");
    fprintf(stderr,"\tport is the base TCP port to use\n");
    fprintf(stderr,"\tid is the source data shared memory area ID to use\n");
    fprintf(stderr,"\tID is the destination TCP port offset to use\n");
    fprintf(stderr,"\tmode is the protocol transfer option\n");
    fprintf(stderr,"Defaults are (none); 10305; 0; 0; 64K\n");
    exit(1);
}
        
int main(int argc, char **argv) {

    char Buffer [64+(64*1024)];    /*  a 64Kbyte data buffer + a bit extra to handle block headers  */
    DATA_HEADER data_header;
    int BufferLength;
    int DataLength;

    char *TS_Server= NULL;
    int Port=10305;
    int ID=0;
    int id=0;
    int Mode=3;
    unsigned int stream = 1;
    int verbose=0;
    int s;
    int i;
    int BufferSize=64*1024;

    int DelayTicks=10;

#ifndef WIN32
    struct timespec time_request;
#endif

    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(tolower(argv[i][1])) {
                    case 'n':
                        TS_Server = argv[++i];
                        break;
                    case 'p':
                        Port = atoi(argv[++i]);
                        break;
                    case 'I':
                        ID = atoi(argv[++i]);
                        break;
                    case 'i':
                        id = atoi(argv[++i]);
                        break;
                    case 's':
                        BufferSize = atoi(argv[++i])*1024;
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

    if (TS_Server == NULL)  Usage(argv[0]);

//    (void) transferSetVerbose(verbose);
    (void) transferMultiBlockSize(ID, BufferSize);
    (void) transferMultiMode(ID, Mode);
    (void) transferMultiPort(ID, Port);
    (void) transferMultiInit(ID, TS_Server);

    i = dataSpyOpen (id);
    
    for (;;)
    {

/*    obtain a data buffer from shared memory. This will have a 24 byte header */

       DataLength = dataSpyRead (id, (char*)&Buffer[sizeof(HEADER)-sizeof(DATA_HEADER)], BufferSize);  /* read into buffer at offset 8 bytes; real data starts at 8+24 = 32  */
       if (verbose) printf("read done with code=%d\n",DataLength);

       if (DataLength > 0) {

              memcpy (data_header.header_id, (char*)&Buffer[sizeof(HEADER)-sizeof(DATA_HEADER)], sizeof(DATA_HEADER));
              BufferLength = data_header.header_dataLen;   /* the actual application data less headers  */
              if (verbose) printf("buffer length=%d\n",BufferLength);
         
/*    transfer this data buffer.  This requires that we allow 32 bytes for use by the protocol overheads    */

              s = transferMultiTxData (ID, &Buffer[sizeof(HEADER)], stream, BufferLength);   /*   write data from offset 32 bytes  */

              if (verbose) printf("transfer done with code=%d\n",s);
       } else {
           if (DelayTicks) {
#ifdef WIN32
               Sleep((DWORD)DelayTicks);
#endif
#ifdef LINUX
               time_request.tv_sec = DelayTicks/1000;
               time_request.tv_nsec = (DelayTicks - (time_request.tv_sec * 1000)) * 1000000;
               i = nanosleep(&time_request, NULL);
#endif
            }
       }
    }
}
