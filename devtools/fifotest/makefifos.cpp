#include <iostream>
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

#include <fcntl.h>

#define FIFOPATH "/tmp/fifo"
int main(int argc, char *argv[])
{
    std::cout<<"Make FIFO"<<std::endl;
    /* Make FIFO to communicate with babier */
    //unlink(FIFOPATH);
    //if(mkfifo(FIFOPATH, 0666) == -1){
    //  printf("makefifo: Can't make %s\n",FIFOPATH);
    //}

    /* Open FIFO FD */
    int fifopipe;
    if((fifopipe = open(FIFOPATH, O_RDWR)) == -1){
      printf("makefifo: Can't open %s\n", FIFOPATH);
    }


    long pipe_size = (long)fcntl(fifopipe, F_GETPIPE_SZ);
    if (pipe_size == -1) {
        perror("get pipe size failed.");
    }
    printf("default pipe size: %ld\n", pipe_size);

    int ret = fcntl(fifopipe, F_SETPIPE_SZ, 1024*1024);//set maimum fifo size 1 MB
    if (ret < 0) {
        perror("set pipe size failed.");
    }
    pipe_size = (long)fcntl(fifopipe, F_GETPIPE_SZ);
    if (pipe_size == -1) {
        perror("get pipe size failed.");
    }
    printf("new pipe size: %ld\n", pipe_size);


    int icnt=0;
    while(1){
        usleep(1000000);
        write(fifopipe, (char *)&icnt, sizeof(icnt));
        std::cout<<icnt<<std::endl;
        icnt++;
    }

    return 0;
}
