
#ifdef WIN32
#include <windows.h>
#endif


#include <string.h>
#include <stdio.h>

#define MAXPORTS 8
#define N_DRIVER_DATALENGTH 16384    // in units of words  (ie 4096 => 16 Kbyte)

#ifdef UNIX
#include <stdlib.h> /* getenv, exit */
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#endif

extern "C" {
#include <sys/types.h>
}

int Tcp_Port[MAXPORTS];
int SocketConnected[MAXPORTS];
int TransferSize[MAXPORTS];
int Done[MAXPORTS];
char data_buffer[MAXPORTS][N_DRIVER_DATALENGTH*sizeof(int)];
int ReadEnable[MAXPORTS];
int ACKneeded [MAXPORTS];

int Length[MAXPORTS];
int Endian[MAXPORTS];

#define TS_TCP_PORT 10305      // default port

void link_server();
void initialise_buffer_area(int);

int OutFlag =0;     /* no file output by default  */
FILE *OutStream[MAXPORTS];
char *FileName= (char*)"/tmp/out.dat";

int IDs = 1;


void Usage(char *progname) {
    fprintf(stderr,"Usage\n%s -f [file path name] -p [port] - i [IDs]\n",progname);
    fprintf(stderr,"\tfile path name is root of the file name used to store data  (default null and no data is stored)\n");
    fprintf(stderr,"\tport is the base TCP port to use (default 10305)\n");
    fprintf(stderr,"\tIDs is the number of data streams to handle (default 1)\n");
    exit(1);
}

int main(int argc, char *argv[], char *envp[])
{

   int i;
   char fname[32];

//    defaults for configuration information which may be supplied via the command line

   for (i=0; i<MAXPORTS; i++) {Tcp_Port[i] = 0; TransferSize[i] = 65536;}

   Tcp_Port[0] = TS_TCP_PORT;

//    command line arguments

    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(argv[i][1]) {
                    case 'f':
                        FileName = argv[++i];
                        OutFlag = 1;
                        break;
                    case 'p':
                        Tcp_Port[0] = atoi(argv[++i]);
                        break;
                    case 'i':
                        IDs = atoi(argv[++i]);
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

   for (i=1; i<IDs; i++) {
       Tcp_Port[i] = Tcp_Port[0]+i;
   }

   for (i=0; i<MAXPORTS; i++) {
       if (Tcp_Port[i] != 0) printf("Configuration: TCP port %d = %d\n", i, Tcp_Port[i]);
   }

   if (OutFlag != 0) {
      for (i=0; i<IDs; i++) {
         printf("Writing to %s%d\n",FileName,i);
         sprintf(fname,"%s%d",FileName,i);
         OutStream[i] = fopen(fname,"w");
      }
   }

   for (i=0; i<MAXPORTS; i++) {
       if (Tcp_Port[i] != 0)initialise_buffer_area(i);
   }

   link_server();

#ifdef UNIX
   exit(0);
#endif

#ifdef WIN32
   ExitProcess(0);
#endif

   return(0);
}

