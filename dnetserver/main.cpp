#include "../ddaq/daq.h"
#include "TSocket.h"
#include "TPServerSocket.h"
int main(int argc, char *argv[])
{
    int port=9090;
    char addr[512];
    if (argc==3){
        strcpy(addr,argv[1]);
        port=atoi(argv[2]);
    }else{
        printf("Please enter address and port number!\n");
        return 0;
    }
    daq daq_t;
    if ((daq_t.shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1){
        daq_t.shmp = (struct SHM_DGTZ_S *)shmat(daq_t.shmid, 0, 0);
    }else{
        printf("Can not attach to share memory!!!!!\n");
    }
    daq_t.shmp->ana_status=1;
    daq_t.shmp->ana_flag=1;

    TSocket *socket = new TSocket(addr,port);
    char str[32];
    socket->Recv(str,32);
    while(1)
    {
        if (daq_t.shmp->ana_flag==1){
            socket->SendRaw(daq_t.shmp,sizeof(SHM_DGTZ_S));
            daq_t.shmp->ana_flag=0;
        }
    }

}

