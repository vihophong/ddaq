

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#endif

#include "common.h"
#include "message.h"

#include "transfer.h"

#ifdef UNIX
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctype.h>
#include <fcntl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef UNIX
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define SOCKET int
#endif

extern int ts_data_transfer (int, int, char *, int);

extern int Tcp_Port[];
extern int SocketConnected[];   // =0 not connected; =1 listening; =2 connected; =-1 not in use
extern int TransferSize[];
extern int Done[];
extern char data_buffer[][N_DRIVER_DATALENGTH*sizeof(int)];
extern int ReadEnable[];
extern int ACKneeded [];

int nfds = 0;

SOCKET listen_socket[MAXPORTS];
SOCKET msg_socket[MAXPORTS];

int rc [MAXPORTS];

#define MINSNDBUFSIZE 32          /*  units Kbytes  */
#define MAXSNDBUFSIZE 4*1024      /* default max attempted */
#define MINRCVBUFSIZE 32          /*  units Kbytes  */
#define MAXRCVBUFSIZE 4*1024      /* default max attempted */

#ifdef SINK

char message_buffer[MESSAGE_BUFFER_LENGTH];

#ifdef WIN32
void ReportError () {

  LPVOID lpMsgBuf;
  DWORD ErrNo;
  
       ErrNo = WSAGetLastError();
       FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      ErrNo,
                      0,
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL
                      );
                      
       printf("Error %d - %s\n", ErrNo, lpMsgBuf);
       LocalFree (lpMsgBuf);
}
#endif

#ifdef UNIX
void ReportError () {
    printf("Error %d - %s\n", errno, strerror(errno));
}
#endif

void report_message(int level)
{
    fprintf(stdout,"%s\n", message_buffer);
}

void message(int level, const char * msg)
{
    (void) sprintf(message_buffer, "%s", msg);
    report_message(level);
}

void LogMessage(int level)
{
      report_message(level);
}

#endif
/*******************************************************************************
    same parameter list as send
    loops until all data has been sent  (or ERROR)
    Note - we only at most send ACKs so there shuld be no need to retry
*/
int do_send(SOCKET S, char * p, int l, int t)
{
   int rc, done, left;
#ifdef VERBOSE
   printf("send %d bytes\n", l);
#endif
    done = 0;
    left =  l;
    while (done < l) {
        rc = send(S, p+done, left, t);
#ifdef VERBOSE
        printf("%d", rc);
#endif
        if (rc == SOCKET_ERROR) return rc;
        done += rc;
        left -= rc;
    }
#ifdef VERBOSE
    printf("Complete\n");
#endif

    return l;
}

/*******************************************************************************/

int ReadBlock(int i, char *Buffer, int size) {
   int rc;
   SOCKET S;

#ifdef VERBOSE
    printf("ReadBlock %d bytes ",size);
#endif
   
        S = msg_socket[i];
   
        rc = recv(S, Buffer, size, 0);

#ifdef UNIX
        if (rc == SOCKET_ERROR && errno == EWOULDBLOCK)
            return 0;  /*  should not happen because of select */
#endif
#ifdef WIN32
        if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;  /*  should not happen because of select */
#endif
            
        if (rc == SOCKET_ERROR) {
            sprintf(message_buffer,"thread %d recv() failed: ",i);
            report_message(MSG_FAILURE);
            ReportError();
#ifdef WIN32
            closesocket(S);
#endif
#ifdef UNIX
            close(S);
#endif
            sprintf(message_buffer,"thread %d connection terminated",i);
            report_message(MSG_INFORMATION);
            return -1;
        }
        if (rc == 0) {
            sprintf(message_buffer,"thread %d client closed connection",i);
            report_message(MSG_INFORMATION);
#ifdef WIN32
            closesocket(S);
#endif
#ifdef UNIX
            close(S);
#endif
            return 0;
        }
#ifdef VERBOSE
    printf("received %d bytes\n",rc);
#endif
        return rc;
}

/*******************************************************************************/

int WriteResponse(int i, char *Buffer, int size) {
   int rc;
   SOCKET S;

       S = msg_socket[i];
       
        rc = do_send(S, Buffer, size, 0);

        if (rc == SOCKET_ERROR) {
            sprintf(message_buffer,"thread %d send() failed: ",i);
            report_message(MSG_FAILURE);
            ReportError();
#ifdef WIN32
            closesocket(S);
#endif
#ifdef UNIX
            close(S);
#endif
            sprintf(message_buffer,"thread %d connection terminated",i);
            report_message(MSG_INFORMATION);
            return -1;
        }
        
        if (rc == 0) {
            sprintf(message_buffer,"thread %d client closed connection",i);
            report_message(MSG_INFORMATION);
#ifdef WIN32
            closesocket(S);
#endif
#ifdef UNIX
            close(S);
#endif
            return 0;
        }
        return rc;
}

/*******************************************************************************/

void StartUp_Socket (int i) {

    int yes = 1;
    char * interface = NULL;
    struct sockaddr_in local;
    int rcvsize, rcvreq, sndsize, sndreq, temp;
#ifdef WIN32
    int length;
    u_long enable = 1;
#else
    socklen_t length;
#endif
    
    SocketConnected[i]=0;

    memset(&local,0,sizeof(local));   //   zero structure
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (!interface)?INADDR_ANY:inet_addr(interface); 
    local.sin_port = htons((unsigned short)Tcp_Port[i]);    // Port MUST be in Network Byte Order

    listen_socket[i] = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    
    if (listen_socket[i] == INVALID_SOCKET) {
        sprintf(message_buffer,"socket() failed: ");
        report_message(MSG_FAILURE);
        ReportError();
#ifdef UNIX
        exit(16);
#endif
#ifdef WIN32
        WSACleanup();
        ExitProcess(16);
#endif
    }
    
#ifdef UNIX
    if (setsockopt(listen_socket[i],SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == SOCKET_ERROR) {
#endif

#ifdef WIN32
    if (setsockopt(listen_socket[i], SOL_SOCKET, SO_REUSEADDR, (const char FAR *)&yes, sizeof(yes)) != 0) {
#endif

        ReportError();
        sprintf(message_buffer,"setsockopt() failed for SO_REUSEADDR: ");
        report_message(MSG_FAILURE);
#ifdef UNIX
        exit(16);
#endif
#ifdef WIN32
        WSACleanup();
        ExitProcess(16);
#endif
    }

/*    optimize TCP receive buffer size    */

    length=sizeof(rcvsize);
    if (getsockopt(listen_socket[i],SOL_SOCKET,SO_RCVBUF,(char *)&rcvsize,&length) == SOCKET_ERROR) {
        ReportError();
        sprintf(message_buffer, "getsockopt() failed:  ");
        report_message(MSG_WARNING);
    }
    temp = rcvsize;
    
    for (rcvreq = MINRCVBUFSIZE; rcvreq <= MAXRCVBUFSIZE; rcvreq = rcvreq+MINRCVBUFSIZE) {
        rcvsize = rcvreq*1024;
        if (setsockopt(listen_socket[i],SOL_SOCKET,SO_RCVBUF,(char *)&rcvsize,sizeof(rcvsize)) == SOCKET_ERROR) {
//            ReportError();
//            sprintf(message_buffer, "setsockopt() failed for SO_RCVBUF:  ");
//            report_message(MSG_WARNING);
            break;
        }
    }

    length=sizeof(rcvsize);
    if (getsockopt(listen_socket[i],SOL_SOCKET,SO_RCVBUF,(char *)&rcvsize,&length) == SOCKET_ERROR) {
        ReportError();
        sprintf(message_buffer, "getsockopt() failed:  ");
        report_message(MSG_WARNING);;
    }
    
    sprintf(message_buffer, "TCP socket receive buffer was %d - now %d",temp,rcvsize);
    report_message(MSG_INFORMATION);

/*    repeat for send buffer which seems to also be needed     */

    length=sizeof(sndsize);
    if (getsockopt(listen_socket[i],SOL_SOCKET,SO_SNDBUF,(char *)&sndsize,&length) == SOCKET_ERROR) {
        ReportError();
        sprintf(message_buffer, "getsockopt() failed:  ");
        report_message(MSG_WARNING);
    }
    temp = sndsize;

    for (sndreq = MINSNDBUFSIZE; sndreq <= MAXSNDBUFSIZE; sndreq = sndreq+MINSNDBUFSIZE) {
        sndsize = sndreq*1024;
        if (setsockopt(listen_socket[i],SOL_SOCKET,SO_SNDBUF,(char *)&sndsize,sizeof(sndsize)) == SOCKET_ERROR) {
//            ReportError();
//            sprintf(message_buffer, "setsockopt() failed for SO_RCVBUF:  ");
//            report_message(MSG_WARNING);
              break;
        }
    }

    length=sizeof(sndsize);
    if (getsockopt(listen_socket[i],SOL_SOCKET,SO_SNDBUF,(char *)&sndsize,&length) == SOCKET_ERROR) {
        ReportError();
        sprintf(message_buffer, "getsockopt() failed:  ");
        report_message(MSG_WARNING);;
    }
    
    sprintf(message_buffer, "TCP socket send buffer was %d - now %d",temp,sndsize);
    report_message(MSG_INFORMATION);

    // bind() associates a local address and port combination with the socket just created. 

    if (bind(listen_socket[i],(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR) {
        sprintf(message_buffer,"bind() failed: ");
        report_message(MSG_FAILURE);
        ReportError();
#ifdef UNIX
        exit(16);
#endif
#ifdef WIN32
        WSACleanup();
        ExitProcess(16);
#endif
    }

    if (listen(listen_socket[i],5) == SOCKET_ERROR) {
       sprintf(message_buffer,"listen() failed: ");
       report_message(MSG_FAILURE);
       ReportError();
#ifdef UNIX
        exit(16);
#endif
#ifdef WIN32
        WSACleanup();
        ExitProcess(16);
#endif
    }

#ifdef WIN32
    if (setsockopt(listen_socket[i], IPPROTO_TCP, TCP_NODELAY, (char *)&yes, sizeof(int)) != 0) {
        sprintf(message_buffer,"setsockopt() failed for TCP_NODELAY: ");
        report_message(MSG_FAILURE);
        ReportError();
        WSACleanup();
        ExitProcess(16);
    }
#endif

    //    set the socket to non-blocking mode

#ifdef WIN32
    if (ioctlsocket(listen_socket[i], FIONBIO, &enable) == SOCKET_ERROR) {
        sprintf(message_buffer,"ioctlsocket() failed: ");
        report_message(MSG_FAILURE);
        WSACleanup();
        ExitProcess(16);
    }
#endif
#ifdef UNIX
     fcntl(listen_socket[i], F_SETFL, O_NONBLOCK);
#endif

#ifdef UNIX
    if (listen_socket[i] > nfds) nfds = listen_socket[i];
#endif

    sprintf(message_buffer, "MIDAS Data Link thread %d using TCP port %d.", i, Tcp_Port[i]);
    LogMessage(MSG_INFORMATION);
}

/*******************************************************************************/

void Write_Socket (int i) {

    char Ack[sizeof(ACK)];
    ACK * ackptr;

#ifdef VERBOSE
    printf ("Write_Socket %d %d\n", i, SocketConnected[i]);
#endif
    //
    //    socket is ready to write - send ack
    //
    ackptr = (ACK*) &Ack[0];
    ackptr->acq_flags = htons(1);
    if (rc[i] < 0) {
        ackptr->acq_code = htons(1);
    } else {
        ackptr->acq_code = htons(0);
    }
    if (WriteResponse(i,Ack,sizeof(Ack)) <= 0) {
        SocketConnected[i]=0;
        return;
    } 
    ACKneeded[i]=0;
}

/*******************************************************************************/

void Read_Socket (int i) {

    HEADER * blockptr;
    int * dataptr;
    int len;
    int endian;
    int stream;
    int received;
#ifdef VERBOSE
    int j;
#endif
    int ID;
    
#ifdef VERBOSE
    printf ("Read_Socket %d %d\n", i, SocketConnected[i]);
#endif
    //
    // socket is ready to read, i.e., there is data on the socket.
    //
    len = Done[i];
    received = ReadBlock(i,&data_buffer[i][len],TransferSize[i]-len);
    if (received <= 0)  {
        SocketConnected[i]=0;
        return;
    }
    Done[i] += received;
    if (Done[i] < TransferSize[i]) return;

//    a data block is now complete

    blockptr = (HEADER*) &data_buffer[i][0];
    dataptr = (int*) &data_buffer[i][0];
    stream=ntohs(blockptr->hdr_stream);
    len=ntohl(blockptr->hdr_datalength);
    endian=blockptr->hdr_endian;

    ID = i;

#ifdef VERBOSE
    for (j = 0; j < 64;) {
        printf(" 0x%08x", dataptr[j]);
        j++;
        if ((j/8)*8 == j) printf("\n");
    }
    printf("\n");
#endif
    if (len >= 0) {
#ifdef VERBOSE
    printf("thread %d block length %d\n", i, len);
#endif
       rc[i] =ts_data_transfer(ID, endian, &data_buffer[i][sizeof(HEADER)], len);
    } else {
        len = ntohl(blockptr->hdr_blocklength);
        if (len < 1024 || len > (1025*1024)) {
            sprintf(message_buffer,"buffer size NOT changed to %d",len);
        } else {
            TransferSize[i] = len;
            sprintf(message_buffer,"buffer size changed to %d",len);
        }
        report_message(MSG_INFORMATION);
    }
    Done[i] =0;
    if ((ntohs(blockptr->hdr_flags) & 2) != 2) {
        ACKneeded[i]=1;
    }
}

/*******************************************************************************/

void Listen_Socket (int i) {

    struct sockaddr_in from;
#ifdef WIN32
    int fromlen;
    int yes=1;
#endif
#ifdef UNIX
    unsigned int fromlen;
    int yes=1;
#endif

#ifdef VERBOSE
    printf ("Listen_Socket %d %d\n", i, SocketConnected[i]);
#endif
    //
    // close the previous client socket. 
    //
#ifdef WIN32
    if (SocketConnected[i]==2) closesocket(msg_socket[i]);
#endif
#ifdef UNIX
    if (SocketConnected[i]==2) close(msg_socket[i]);
#endif
                                   
    if (SocketConnected[i]==2) {
        sprintf(message_buffer,"thread %d closed existing connection", i);
        report_message(MSG_INFORMATION);
    }

    SocketConnected[i]=0;
            
    fromlen = sizeof(from);
            
    msg_socket[i] = accept(listen_socket[i],(struct sockaddr*)&from, &fromlen);
    if (msg_socket[i] == INVALID_SOCKET) {
        sprintf(message_buffer,"thread %d accept() failed: ", i);
        report_message(MSG_FAILURE);
        ReportError();
#ifdef UNIX
        exit(16);
#endif
#ifdef WIN32
        WSACleanup();
        ExitProcess(16);
#endif
     }
     sprintf(message_buffer,"thread %d accepted connection from %s, port %d", i, inet_ntoa(from.sin_addr),htons(from.sin_port));
     report_message(MSG_INFORMATION);

#ifdef UNIX
     if (msg_socket[i] > nfds) nfds = msg_socket[i];
#endif
                        
     SocketConnected[i] =2;
     ACKneeded[i]=0;
#ifdef XDAQ
     TransferSize[i]=XDAQ_BufferSize;
#else
     TransferSize[i]=1024;
#endif
     Done[i]=0;
     ReadEnable[i] = 1;

}

/*******************************************************************************/


void link_server () {

    char *interface= NULL;
    int i;
    int r;
    fd_set readfds, writefds;

#ifdef WIN32
    WSADATA wsaData;
    int retval;
#endif

    message(MSG_INFORMATION, (char *) "Starting the network interface");
    
#ifdef WIN32
//    startup for Winsock V2.2 
    if ((retval = WSAStartup(MAKEWORD( 2, 2),&wsaData)) != 0) {
        WSASetLastError(retval);
        sprintf(message_buffer,"WSAStartup failed: ");
        report_message(MSG_FAILURE);
        ReportError();
        WSACleanup();
        ExitProcess(16);
    }
#endif

    for (i=0; i<MAXPORTS; i++) {
        if (Tcp_Port[i]==0) {SocketConnected[i]=-1;} else {StartUp_Socket(i);}
    }
    
    message(MSG_INFORMATION, (char *) "Entering server loop");

    
    //    We only accept once connection at a time for each port. As soon as another client connects on a port we 
    //    disconnect the first one and start talking to the new client on that port.
    
    for (;;)
    {
       
        //
        // A socket in the listen() state becomes ready to read when a
        // client connects to it. An accept() will complete without blocking.
        // Since select sets the sockets that are ready to be read from or
        // written to, we have to include listen_socket in the fdset each time
        // through the loop.
        //

        for (i=0; i<MAXPORTS; i++) {
            if (SocketConnected[i]==0) {
               sprintf(message_buffer,"thread %d listening on port %d", i, Tcp_Port[i]);
               report_message(MSG_INFORMATION);
               SocketConnected[i] = 1;
               ReadEnable[i] =0;
               ACKneeded[i] =0;
            }
        }

#ifdef UNIX
        do {
#endif
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            for (i=0; i<MAXPORTS; i++) {
                if (SocketConnected[i] >= 1) FD_SET(listen_socket[i],&readfds);
                if (SocketConnected[i] == 2) {
                    if (ReadEnable[i]) FD_SET(msg_socket[i],&readfds);
                    if (ACKneeded[i])  FD_SET(msg_socket[i],&writefds); /* send ACK: enable write */
                }
            }
            r = select(nfds+1,&readfds,&writefds,NULL,NULL);
#ifdef UNIX
        } while (r == SOCKET_ERROR && errno == EINTR);
#endif

        if (r == SOCKET_ERROR) {
            sprintf(message_buffer,"select() failed: ");
            report_message(MSG_FAILURE);
            ReportError();
        }
        if (r == 0){
            sprintf(message_buffer,"Select returned no fds ready");
            report_message(MSG_ERROR);
        }

        for (i=0; i<MAXPORTS; i++) {
            if (FD_ISSET(listen_socket[i], &readfds)) Listen_Socket(i);
            if (SocketConnected[i] == 2) {
                if (ReadEnable[i] && FD_ISSET(msg_socket[i], &readfds)) Read_Socket(i);
                if (ACKneeded[i] && FD_ISSET(msg_socket[i], &writefds)) Write_Socket(i);
            }
        }
    }
}

/*******************************************************************************/
