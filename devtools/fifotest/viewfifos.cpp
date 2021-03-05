#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <pthread.h>

#include <iostream>

#define FIFOPATH "/tmp/fifo"
int main(int argc, char *argv[])
{
    std::cout<<"View FIFO"<<std::endl;

    /* Open FIFO FD */
    int fifopipe;
    if((fifopipe = open(FIFOPATH, O_RDWR)) == -1){
      printf("makefifo: Can't open %s\n", FIFOPATH);
    }
    int bufin=0;
    while(1){
        //usleep(1000000);
        ssize_t nbytesread = read(fifopipe, (char *)&bufin, sizeof(bufin));
        if (nbytesread==0) {
            std::cout<<"fifo read error!"<<std::endl;
            return 0;
        }
        std::cout<<bufin<<std::endl;
    }
    return 0;
}
