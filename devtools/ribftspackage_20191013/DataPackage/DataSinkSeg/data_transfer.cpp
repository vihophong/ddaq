

#include "common.h"
#include "message.h"


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <fcntl.h>

#include "transfer.h"

//    format of the data buffer area header

#define NBLOCKS 64                     /*  current number of buffers used - must be 2**n */
#define MAX_BUFFERS 128                /*  unchangeable maximum because of header structure */

typedef struct s_buffer_header {
    int buffer_offset;                 //    offset to first buffer
    int buffer_number;                 //    number of buffers
    int buffer_length;                 //    length of buffers
    int buffer_next;                   //    next buffer to be written
    int buffer_max;                    //    MAX_BUFFERS
    int buffer_spare1;
    int buffer_spare2;
    int buffer_spare3;
#ifdef WIN32
    int buffer_currentage;
    int padding1;
    int buffer_age[MAX_BUFFERS];
    int padding2[MAX_BUFFERS];
#else
    long long buffer_currentage;
    long long buffer_age[MAX_BUFFERS];
#endif
    int buffer_status[MAX_BUFFERS];
}  BUFFER_HEADER; 

#include "alloc_data_area.h"

//    address of shared buffer area
void * shmbufferarea[MAXPORTS];
int shmkey;

#define TS_TCP_PORT 10305   // default port
#define SHM_KEY 110205

extern int TransferSize[];
int tape_block_size[MAXPORTS];

extern int Tcp_Port[];
extern int OutFlag;
extern FILE* OutStream[];

char spinner[4];
int spin = 0;

int BlockSequence[MAXPORTS];

void byteswop32(char*, int);
void byteswop16(char*, int);

void initialise_buffer_area(int id)
{

    int i;
    BUFFER_HEADER * baseaddress;

    shmkey = Tcp_Port[id] - TS_TCP_PORT + SHM_KEY;

    if (tape_block_size[id] < TransferSize[id]) {
        shmbufferarea[id] =  alloc_data_area(shmkey, (TransferSize[id] * NBLOCKS) + sizeof(BUFFER_HEADER));
        tape_block_size[id] = TransferSize[id];
    }

    baseaddress = (BUFFER_HEADER *) shmbufferarea[id];
    
    baseaddress->buffer_offset = sizeof(BUFFER_HEADER);
    baseaddress->buffer_number = NBLOCKS;
    baseaddress->buffer_length = tape_block_size[id];
    baseaddress->buffer_next = 0;
    baseaddress->buffer_max = MAX_BUFFERS;
    baseaddress->buffer_currentage = 0;
    for (i = 0; i < MAX_BUFFERS; i++) baseaddress->buffer_status[i] = 0;
    for (i = 0; i < MAX_BUFFERS; i++) baseaddress->buffer_age[i] = 0;

        spinner[0] = '-';      //  Note - initialised like this because of problems with GNU compiler
        spinner[1] = '\\';
        spinner[2] = '|';
        spinner[3] = '/';

   for (i=0; i<MAXPORTS; i++) {BlockSequence[i] = 0;}

}

void store_buffer (int ID, int endian, char * p, int l)
{
     DATA_HEADER data_header;

     BUFFER_HEADER * baseaddress;
     char * bufferaddress;
     int offset, length;

     int i;
     int block_len = tape_block_size[ID];

           if ( (unsigned int)(l + sizeof(DATA_HEADER)) > (unsigned int) block_len ) {
               l = block_len - sizeof(DATA_HEADER);
           }

//    construct 24 byte block header

           memcpy (data_header.header_id,"EBYEDATA",8);
           data_header.header_sequence = 0;
           data_header.header_stream = (ID << 8) + 1;
           data_header.header_tape = 0;
           data_header.header_MyEndian = 1;
           data_header.header_DataEndian = endian;
           data_header.header_dataLen = l;

//    copy the buffer into the data buffer area in shared memory

         baseaddress = (BUFFER_HEADER *) shmbufferarea[ID];
                     
//    set the age of the buffer to be written as zero

         i = baseaddress->buffer_next;
         baseaddress->buffer_age[i] = 0;
                     
//    calculate the address of the buffer to use and copy the data

         offset = baseaddress->buffer_offset;
         length = baseaddress->buffer_length;
         bufferaddress = (char *) ((char *)shmbufferarea[ID] + offset + (length * i));

            memcpy (bufferaddress, data_header.header_id, sizeof(DATA_HEADER));
            memcpy (bufferaddress + sizeof(DATA_HEADER), p, l);
                          
//    set the age of the buffer and increment the age and the next buffer

         baseaddress->buffer_age[i] = baseaddress->buffer_currentage;
         baseaddress->buffer_currentage++;
                     
         baseaddress->buffer_next = ((baseaddress->buffer_next + 1) & (NBLOCKS - 1));

      if (OutFlag != 0)  fwrite(bufferaddress, 1, length, OutStream[ID]);

}


int ts_data_transfer(int ID, int endian, char * ptr, int len)
{
    HEADER* p;

    if (tape_block_size[ID] != TransferSize[ID]) {initialise_buffer_area(ID);}

//    check block sequence number

     p = (HEADER*) (ptr-sizeof(HEADER));

     if (ntohl(p->hdr_sequence) == BlockSequence[ID]) {
         BlockSequence[ID]++;   // good
     } else {
         printf ("block sequence error: ID %d, expected %d; received %d\n", ID, BlockSequence[ID], p->hdr_sequence);
         BlockSequence[ID] = ntohl(p->hdr_sequence)+1;
     }

     if (ntohl(p->hdr_datalength) != len) {printf("check#1: %d %d\n",ntohl(p->hdr_datalength),len);}
     if (len + 32 > ntohl(p->hdr_blocklength)) {printf("check#2: %d %d\n",ntohl(p->hdr_blocklength),len);}
     if (ntohl(p->hdr_id1) != HDR_ID1) {printf("check#3: 0x%x\n",ntohl(p->hdr_id1));}
     if (ntohl(p->hdr_id2) != HDR_ID2) {printf("check#4: 0x%x\n",ntohl(p->hdr_id2));}

     if (len > ((64*1024)-32)) {printf("%d: Unexpected block length %d\n",ID,len); return(0);}

     if (endian != 1) {
          byteswop16(ptr, len);        /*    for MIDAS format   */
          endian = 1;
     }

      store_buffer(ID, endian, ptr, len);

       printf ("%c\b", spinner[spin]); fflush(NULL);
             if (++spin == 4) spin=0;
 
      return (0);
}



void byteswop32(char* p, int l) 
{
    int len;
    char *ps;
    char *pd;
    char b0, b1, b2, b3;

   len = l/4;
   ps = p;
   pd = p;


   while (len > 0)
   {
       b0 = *ps++;
       b1 = *ps++;
       b2 = *ps++;
       b3 = *ps++;

       *pd++ = b3;
       *pd++ = b2;
       *pd++ = b1;
       *pd++ = b0;
   
       len--;
   }
}


void byteswop16(char* p, int l) 
{
    int len;
    char *ps;
    char *pd;
    char b0, b1;

   len = l/2;
   ps = p;
   pd = p;


   while (len > 0)
   {
       b0 = *ps++;
       b1 = *ps++;

       *pd++ = b1;
       *pd++ = b0;
   
       len--;
   }
}

