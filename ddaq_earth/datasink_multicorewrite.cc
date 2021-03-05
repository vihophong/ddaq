#include <future>
#include <iostream>
#include <string>
#include <stdlib.h>


#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <stdio.h>
#include <iomanip>

#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <sys/socket.h>
#include <netdb.h>

#include <vector>
#include <map>
#include <queue>


#include "zmq.hpp"
#include "zmq_addon.hpp"

#define MAX_SUBSCRIBE_LIST 200
#define MAX_N_CLIENT 100
#define DATASINKCOMPORT 7777

static bool sink_data = false;
static std::string TheFileRule = "rcdaq-%08d.evt";
static std::string TheRunType = "default";


double CurrentMBytes = 0;

static std::string CurrentFileName;

static bool flag_outfile=false;
static int TheRun=0;
static int outfile_fd = 0;
static float MaxFileSize=-1;//max file size in MB

static int buffer_begin[MAX_SUBSCRIBE_LIST];
static int recv_buffers[MAX_SUBSCRIBE_LIST];
static int elapsed_buffers[MAX_SUBSCRIBE_LIST];
static double totalbytes = 0;

static zmq::message_t recv_msgs1;
static zmq::message_t recv_msgs2;
static zmq::message_t recv_msgs3;
static zmq::message_t recv_msgs4;

ssize_t DataSize;

pthread_mutex_t DataSinkSem;

pthread_mutex_t WriteSem;
pthread_mutex_t WriteProtectSem;

pthread_t ThreadDataSink;
pthread_t ThreadWrite;

static zmq::context_t ctx;

static std::vector<std::pair<std::string, std::string>> RunTypes;
static std::vector<std::pair<std::string, std::string>>::iterator RunTypes_it;

int open_file(const int run_number, int *fd)
{

  char d[512];
  sprintf( d, TheFileRule.c_str(),
       run_number);
  CurrentFileName=std::string(d);

  // test if the file exists, do not overwrite
  int ifd =  open(d, O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE ,
          S_IRWXU | S_IROTH | S_IRGRP );
  if (ifd < 0)
    {
      std::cout << " error opening file " << d << std::endl;
      perror ( d);
      *fd = 0;
      return -1;
      flag_outfile = false;
    }

  *fd = ifd;
  return 0;
}

void *writedata( void *arg)
{
    while(1){
        pthread_mutex_lock( &WriteSem);

        int blockcount = ( recv_msgs4.size() + 8192 -1)/8192;
        int bytecount = blockcount*8192;
        CurrentMBytes += (double) bytecount/1000000.;
        if (flag_outfile&&outfile_fd){
            ssize_t nbytes= write ( outfile_fd, (char*)recv_msgs4.data() , bytecount );
            std::cout<<"write"<<nbytes<<"/"<<recv_msgs4.size()<<std::endl;
            totalbytes += (double) nbytes/1000000.;//in mb
            if (MaxFileSize>0&&totalbytes>MaxFileSize){
                close(outfile_fd);
                TheRun++;
                CurrentMBytes = 0;
                int status = open_file ( TheRun, &outfile_fd);
                if ( !status)
                {
                    std::cout << "Opened output file - SubFile " << TheRun << std::endl;
                }
                else
                {
                    std::cout << "Could not open output file - SubFile " << TheRun << std::endl;
                    exit(0);
                }
                totalbytes = 0;
            }
        }
        pthread_mutex_unlock(&WriteProtectSem);
    }
}

void *datasink( void *arg)
{
    //! recieved data from daqread

    zmq::socket_t* datasinkget;
if ( getenv("PUSHPULL")  )
    datasinkget=new zmq::socket_t(ctx, zmq::socket_type::pull);
else
    datasinkget=new zmq::socket_t(ctx, zmq::socket_type::sub);

    datasinkget->bind("tcp://*:5555");
    //datasinkget->bind("ipc:///tmp/0");

    if ( getenv("PUSHPULL")  ){
        std::cout<<"PUSHPULL scheme set"<<std::endl;
    }else{
        datasinkget->set(zmq::sockopt::subscribe, "");
    }

    //! send to online
    //zmq::context_t ctx2;
    zmq::socket_t datasinksend(ctx, zmq::socket_type::pub);
    datasinksend.bind("tcp://*:6666");

    zmq::pollitem_t items[]={
        { *datasinkget, 0, ZMQ_POLLIN, 0 },
               { datasinksend,  0, ZMQ_POLLIN, 0 }
    };

    for (int i=0;i<MAX_SUBSCRIBE_LIST;i++){ buffer_begin[i]=0; recv_buffers[i]=0; }
    while (1) {
        if (!sink_data) continue;
        pthread_mutex_lock(&WriteProtectSem);
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            auto result = datasinkget->recv(recv_msgs1,zmq::recv_flags::none);
            if (!recv_msgs1.more()){
                perror("Error recv 1st message");
            }else{
                auto result = datasinkget->recv(recv_msgs2,zmq::recv_flags::none);
                if (!recv_msgs2.more()){
                    perror("Error recv 2nd message");
                }else{
                    auto result = datasinkget->recv(recv_msgs3,zmq::recv_flags::none);
                    if (!recv_msgs3.more()){
                        perror("Error recv 3rd message");
                    }else{
                        auto result = datasinkget->recv(recv_msgs4,zmq::recv_flags::none);
                        int id=atoi(recv_msgs1.to_string().erase(0,1).data());
                        int* bufferno=(int*) recv_msgs2.data();
                        int* totalsize=(int*) recv_msgs3.data();
                        if (buffer_begin[id]==0) buffer_begin[id]=*bufferno;
                        recv_buffers[id]++;
                        int elapsedbuff=*bufferno-buffer_begin[id]+1;
                        elapsed_buffers[id]=elapsedbuff;
                        double perct=(double)recv_buffers[id]/(double)elapsedbuff*100;
                        std::cout <<"nmessage_t = "<<*result<<"\tid = "<<id<<"\telapsed buffers = "<<recv_buffers[id]<<"/"<<elapsedbuff<<" = "<<perct
                                 <<" % | buffer no. = "<<  *bufferno<<"\ttotal size="<< *totalsize <<"\t"<<recv_msgs4.size()<<std::endl;

                        pthread_mutex_unlock( &WriteSem);
                        datasinksend.send(recv_msgs1,zmq::send_flags::sndmore);
                        datasinksend.send(recv_msgs2,zmq::send_flags::sndmore);
                        datasinksend.send(recv_msgs3,zmq::send_flags::sndmore);
                        datasinksend.send(recv_msgs4,zmq::send_flags::none);
                    }
                }

            }
        }
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    /*
     * No I/O threads are involved in passing messages using the inproc transport.
     * Therefore, if you are using a ØMQ context for in-process messaging only you
     * can initialise the context with zero I/O threads.
     *
     * Source: http://api.zeromq.org/4-3:zmq-inproc
     */

    pthread_mutex_init( &DataSinkSem, 0);
    pthread_mutex_init( &WriteSem, 0);
    pthread_mutex_init( &WriteProtectSem, 0);

    // pre-lock it except the "protect" ones
    pthread_mutex_lock( &DataSinkSem);
    pthread_mutex_lock( &WriteSem);
    pthread_mutex_lock( &WriteProtectSem);


    int status;
    status = pthread_create(&ThreadDataSink, NULL,
                            datasink,
                            (void *) 0);
    if (status )
    {
        std::cout << "error in send monitor data thread create " << status << std::endl;
        exit(0);
    }

    status = pthread_create(&ThreadWrite, NULL,
                            writedata,
                            (void *) 0);
    if (status )
    {
        std::cout << "error in send monitor data thread create " << status << std::endl;
        exit(0);
    }

    RunTypes.push_back(make_pair(TheRunType,TheFileRule));
    //! zmq communication
    //zmq::context_t ctx;
    zmq::socket_t comm_socket(ctx, zmq::socket_type::rep);
    char tmpcmd[500];
    sprintf(tmpcmd,"tcp://*:%d",DATASINKCOMPORT);
    comm_socket.bind(tmpcmd);
    std::cout << "bind ZMQ tcp socket for communication: " << tmpcmd << std::endl;

    while(1){
        std::vector<zmq::message_t> recv_msgs;
        zmq::recv_result_t resultmess =
          zmq::recv_multipart(comm_socket, std::back_inserter(recv_msgs));
        assert(resultmess && "recv failed");
        int nmess=(int)*resultmess;

        //pthread_mutex_lock( &DataSinkSem);
        std::string outtmp="datasink: \n";
        if (recv_msgs[0].to_string()=="daq_begin")
        {
            sink_data=true;
            if (nmess==2){
                TheRun=atoi(recv_msgs[0].to_string().data());
            }else{
                TheRun++;
            }
            CurrentMBytes = 0;
            if (flag_outfile) {
                int status = open_file(TheRun,&outfile_fd);
                if (status==0) outtmp += "File " + CurrentFileName + " is writing\n";
                else outtmp += "Error openning file : "+ CurrentFileName + "\n";
            }

            for (int i=0;i<MAX_SUBSCRIBE_LIST;i++){ buffer_begin[i]=0; recv_buffers[i]=0; elapsed_buffers[i]=0;}
            totalbytes = 0;
        }
        else if (recv_msgs[0].to_string()=="daq_end")
        {
            sink_data=false;
        }

        else if (recv_msgs[0].to_string()=="daq_open")
        {
            flag_outfile = true;
        }
        else if (recv_msgs[0].to_string()=="daq_close")
        {
            if (flag_outfile) {
                close(outfile_fd);
                outtmp += "File " + CurrentFileName + " is closed\n";
                flag_outfile=false;
            }else{
                outtmp += "No openned file!\n";
            }
        }
        else if (recv_msgs[0].to_string()=="daq_define_runtype")
        {
            if (nmess==3) RunTypes.push_back(make_pair(recv_msgs[1].to_string(),recv_msgs[2].to_string()));
            else outtmp+="Invalid command!\n";
        }
        else if (recv_msgs[0].to_string()=="daq_set_runtype")
        {
            int find_runtype=0;
            for (RunTypes_it=RunTypes.begin();RunTypes_it!=RunTypes.end();RunTypes_it++){
                if (recv_msgs[1].to_string()==RunTypes_it->first){
                    TheFileRule = RunTypes_it->second;
                    TheRunType = recv_msgs[1].to_string();
                    find_runtype++;
                }
            }
            if (find_runtype>0){
                outtmp+="Set Run type : "+TheRunType+"\n";
            }else{
                outtmp+="No such a Run type defined!\n";
            }
        }
        else if (recv_msgs[0].to_string()=="daq_set_maxvolume")
        {
            if (nmess==2) MaxFileSize=atof(recv_msgs[1].to_string().data());
            else outtmp+="Invalid command!\n";
        }
        else if (recv_msgs[0].to_string()=="daq_status")
        {
            outtmp+="Current Run:\n"+std::to_string(TheRun)+"\n";
            if (flag_outfile)
                outtmp+="Logging enabled\n";
            outtmp+="Current Volume = "+std::to_string(CurrentMBytes)+" MB\n";
            if (MaxFileSize>0) outtmp+="Automatically open a new file every "+std::to_string(MaxFileSize)+" MB\n";
            for (int i=0;i<MAX_SUBSCRIBE_LIST;i++) {
                if (recv_buffers[i]>0){
                    outtmp+="Recv from RCDAQ_SERVER No. "+std::to_string(i)+" : "+std::to_string(recv_buffers[i]) +" / "
                            +std::to_string(elapsed_buffers[i])+ " = "+std::to_string((double)recv_buffers[i]/(double)elapsed_buffers[i]*100)+" %\n";
                }
            }
            outtmp+="Defined Run types: \n";
            for (RunTypes_it=RunTypes.begin();RunTypes_it!=RunTypes.end();RunTypes_it++){
                outtmp+=RunTypes_it->first+" "+RunTypes_it->second+"\n";
            }
            outtmp+="Current Run type:" + TheRunType + " - Current file rule:" + TheFileRule + "\n";
        }else{
            outtmp+="Invalid command:\n";
        }
        for (int i=0;i<nmess;i++) outtmp+=recv_msgs[i].to_string()+" ";
        comm_socket.send(zmq::buffer(outtmp.data(),outtmp.length()));
        //pthread_mutex_unlock( &DataSinkSem);
    }

    if (flag_outfile) {
        close(outfile_fd);
        std::cout<<"File " << CurrentFileName <<" closed"<<std::endl;
    }

    return 0;
}

