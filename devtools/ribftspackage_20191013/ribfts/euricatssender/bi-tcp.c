/* lib/bi-tcp.c
 *
 * last modified : 10/12/09 16:58:05 
 *
 * babirl network library
 * not only TCP but also UDP
 *
 * Hidetada Baba (RIKEN)
 * baba@ribf.riken.jp
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#define DB(x)


/** Make TCP server socket (receiver), automatically bind and listen.
 *  Return socket number.
 *  @param port port number should be binded
 *  @return socket number
 */
int mktcpsock(unsigned short port){
  int sock = 0;
  int sockopt = 1;
  struct sockaddr_in saddr;

  memset((char *)&saddr,0,sizeof(saddr)); 
  if((sock = socket(PF_INET,SOCK_STREAM,0)) < 0){ 
    perror("bi-tcp.mktcpsock: Can't make socket.\n"); 
    return 0;
  }
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
       &sockopt, sizeof(sockopt));

  saddr.sin_family = AF_INET; 
  saddr.sin_addr.s_addr = INADDR_ANY; 
  saddr.sin_port = htons(port); 
  if(bind(sock,(struct sockaddr *)&saddr,sizeof(saddr)) < 0){ 
    perror("bi-tcp.mktcpsock: Can't bind socket.\n"); 
    return 0;
  }
  if(listen(sock,100) < 0){
    perror("bi-tcp.mktcpsock: Can't listen socket.");
    return 0;
  }

  return sock;
}

/** Make TCP client socket (sender), automatically connect.
 *  Return socket number.
 *  @param host server hostname
 *  @param port port number should be connected
 *  @return socket number
 */
int mktcpsend(char *host, unsigned short port){
  int sock = 0;
  struct hostent *hp;
  struct sockaddr_in saddr;

  if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
    perror("bi-tcp.mktcpsend: Can't make socket.\n");
    return 0;
  }

  memset((char *)&saddr,0,sizeof(saddr));

  if((hp = gethostbyname(host)) == NULL){
    printf("bi-tcp.mktcpsend : No such host (%s)\n", host);
    return 0;
  }

  memcpy(&saddr.sin_addr,hp->h_addr,hp->h_length);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);

  //DB(printf("host : %s  port : %d\n",host,port);)

  if(connect(sock,(struct sockaddr *)&saddr,sizeof(saddr)) < 0){
    perror("bi-tcp.mktcpsend: Error in tcp connect.\n");
    close(sock);
    return 0;
  }

  return sock;
}

int mktcpsend_tout(char *host, unsigned short port, int tout){
  int sock = 0, ret;
  struct hostent *hp;
  struct sockaddr_in saddr;
  fd_set set;
  struct timeval tv;

  if((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
    perror("bi-tcp.mktcpsend: Can't make socket.\n");
    return 0;
  }

  memset((char *)&saddr,0,sizeof(saddr));

  if((hp = gethostbyname(host)) == NULL){
    printf("bi-tcp.mktcpsend : No such host (%s)\n", host);
    return 0;
  }

  memcpy(&saddr.sin_addr,hp->h_addr,hp->h_length);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0){
    DB(printf("error in fcntl to nonblock\n"));
    return 0;
  }
  //DB(printf("host : %s  port : %d\n",host,port);)

  ret = connect(sock,(struct sockaddr *)&saddr,sizeof(saddr));
  DB(printf("ret1 = %d\n", ret));
  if(ret < 0){
    tv.tv_sec = tout;
    tv.tv_usec = 0; 
    FD_ZERO(&set); FD_SET(sock, &set);
    ret = select(sock+1, NULL, &set, NULL, &tv);
    DB(printf("ret2 = %d\n", ret));
    if(ret <= 0){
      DB(printf("bi-tcp.mktcpsend: Error in tcp connect.\n"));
      close(sock);
      return 0;
    }else{
      /* connect OK */
      ret = connect(sock,(struct sockaddr *)&saddr,sizeof(saddr));
      DB(printf("ret3 = %d\n", ret));
      if(ret < 0){
	close(sock);
	return 0;
      }
    }
  }
  fcntl(sock, F_SETFL, 0);

  return sock;
}

/** Make UDP server socket, automatically binded.
 *  Return socket number.
 *  @param port port number should be connected
 *  @param pointer for saddr sockaddr_in
 *  @return socket number
 */
int mkudpsock(int port, struct sockaddr_in *saddr){
  int sock = 0;
  int sockopt = 1;

  if((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
    perror("mkudpsock: can't make udp socket\n");
    return 0;
  }

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
       &sockopt, sizeof(sockopt));

  saddr->sin_family = AF_INET;
  saddr->sin_addr.s_addr = INADDR_ANY;
  saddr->sin_port = htons(port);
  if(bind(sock, (struct sockaddr *)saddr, sizeof(struct sockaddr_in)) < 0){
// for checkin error message. KY
//    int errbind = errno;
//    char *str = strerror(errbind);
//    perror(*str);
//
    perror("mkudpsock: can't bind socket.\n");
    return 0;
  }

  return sock;
}

/** Make UDP client socket.
 *  Return socket number.
 *  @param port port number should be connected
 *  @param pointer for caddr sockaddr_in
 *  @param hostname of server
 *  @return socket number
 */
int mkudpsend(int port, struct sockaddr_in *caddr, char *hostname){
  int sock = 0;
  int sockopt = 1;
  struct hostent *hp;

  if((sock = socket(AF_INET, SOCK_DGRAM,0)) < 0){
    perror("mkudpsend: Can't make udp socket.\n");
    return 0;
  }

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
       &sockopt, sizeof(sockopt));

  if((hp = gethostbyname(hostname)) == NULL){
    return 0;
  }else{
    memcpy((char *)&caddr->sin_addr, (char *)hp->h_addr, hp->h_length);
    caddr->sin_family = AF_INET;
    caddr->sin_port = htons(port);
  }

  return sock;
}

/** Make UDP multisender socket.
 *  Return socket number.
 *  @return socket number
 */
int mkmultisend(void){
  int sock = 0;
  int sockopt = 1;

  if((sock = socket(AF_INET, SOCK_DGRAM,0)) < 0){
    perror("mkudpsend: Can't make udp socket.\n");
    return 0;
  }
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
	     &sockopt, sizeof(sockopt));

  return sock;
}

/** Register client address for multisender.
 *  Return 1 = suceed, 0 = failed.
 *  @param port Port number of server
 *  @param caddr sockaddr_in
 *  @param host hostname of server
 *  @return 1 = sucdeed, 0 = failed
 */
int registmultisend(int port, struct sockaddr_in *caddr, char *hostname){
  struct hostent *hp;

  if((hp = gethostbyname(hostname)) == NULL){
    return 0;
  }else{
    memcpy((char *)&caddr->sin_addr, (char *)hp->h_addr, hp->h_length);
    caddr->sin_family = AF_INET;
    caddr->sin_port = htons(port);
  }

  return 1;
}


/** Get parameters from babild or babinfo.
 *  @param sock Socket number
 *  @param com  Command
 *  @param dest Buffer address
 *  @return Size of obtained buffer (char size)
 */
int eb_get(int sock, int com, char *dest){
  int len;

  len = sizeof(com);
  send(sock, (char *)&len, sizeof(len), 0);
  send(sock, (char *)&com, len, 0);

  recv(sock, (char *)&len, sizeof(len), MSG_WAITALL);
  recv(sock, dest, len, MSG_WAITALL);

  //DB(printf("bi-tcp: com=%d, len=%d\n", com, len));

  return len;
}

int eb_set(int sock, int com, char *src, int size){
  int len, ret;

  len = sizeof(com) + size;
  send(sock,(char *)&len, sizeof(len), 0);
  send(sock, (char *)&com, sizeof(com), 0);
  send(sock, src, size, 0);

  recv(sock,(char *)&len, sizeof(len), MSG_WAITALL);
  recv(sock, (char *)&ret, len, MSG_WAITALL);

  return ret;
}

