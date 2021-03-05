#include <iostream>
#include <sstream>
#include <getopt.h>

#include <sstream>
#include "parseargument.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"
using namespace std;

void showHelp()
{
  std::cout << "   help                                 show this help text"  << std::endl;
  std::cout << "   daq_status [-s] [-l]                 display status [short] [long]" << std::endl;
  std::cout << "   daq_open                             enable logging" << std::endl;
  std::cout << "   daq_begin [run-number]             	start taking data for run-number, or auto-increment" << std::endl;
  std::cout << "   daq_end                              end the run " << std::endl;
  std::cout << "   daq_close                            disable logging" << std::endl;
  std::cout << "   daq_setfilerule file-rule            set the file rule default rcdaq-%08d-%04d.evt" << std::endl;
  std::cout << std::endl;
  std::cout << "   daq_list_readlist                    display the current readout list" << std::endl;
  std::cout << "   daq_clear_readlist                   clear the current readout list " << std::endl;
  std::cout << std::endl;
  std::cout << "   daq_define_runtype type file-rule    define a run type, such as \"calibration\"" << std::endl;
  std::cout << "   daq_set_runtype type                 activate a predefined run type" << std::endl;
  std::cout << "   daq_get_runtype [-l]                 list the active runtype (if any)" << std::endl;
  std::cout << "   daq_list_runtypes [-s]               list defined run types" << std::endl;
  std::cout << "   daq_get_lastfilename                 return the last file name written, if any" << std::endl;
  std::cout << std::endl;

  std::cout << "   daq_set_maxevents nevt               set automatic end at so many events" << std::endl;
  std::cout << "   daq_set_maxvolume n_MB               set automatic end at n_MB MegaByte" << std::endl;
  std::cout << std::endl;

  std::cout << "   load  shared_library_name            load a \"plugin\" shared library" << std::endl;
  std::cout << "   create_device [device-specific parameters] " << std::endl;
  std::cout << std::endl;

  std::cout << "   daq_setname <string>                 define an identifying string for this RCDAQ instance" << std::endl;
  std::cout << "   daq_setrunnumberfile file            define a file to maintain the current run number" << std::endl;
  std::cout << "   daq_set_maxbuffersize n_KB           adjust the size of buffers written to n KB" << std::endl;
  std::cout << "   daq_set_adaptivebuffering seconds    enable adaptive buffering at n seconds (0 = off)" << std::endl;
  std::cout << std::endl;

  std::cout << "   daq_webcontrol <port number>         restart web controls on a new port (default 8080)" << std::endl;
  std::cout << std::endl;
  std::cout << "   elog elog-server port                specify coordinates for an Elog server" << std::endl;
  std::cout << std::endl;
  std::cout << "   daq_shutdown                         terminate the rcdaq backend" << std::endl;
  exit(0);
}

int  verbose_flag = 0;

int main (int argc, char *argv[])
{
    if ( argc <= 1 ) showHelp();

    char lh[] = "localhost";
    char *host = lh;

    if ( getenv("RCDAQHOST")  )
      {
        host = getenv("RCDAQHOST");
      }

    char defaultport[] = "4444";
    char *port = defaultport;

    if ( getenv("RCDAQPORT")  )
      {
        port = getenv("RCDAQPORT");
      }

    zmq::context_t ctx;
    zmq::socket_t client(ctx, zmq::socket_type::req);
    char tmpcmd[500];
    sprintf(tmpcmd,"tcp://%s:%s",host,port);
    //std::cout<<tmpcmd<<std::endl;
    client.connect(tmpcmd);

    for (int i=1;i<argc;i++){
        if (i!=argc-1) client.send(zmq::buffer(argv[i],strlen(argv[i])), zmq::send_flags::sndmore);
        else client.send(zmq::buffer(argv[i],strlen(argv[i])));
    }
    zmq::message_t mess;
    client.recv(mess);
    std::cout<<mess.to_string()<<std::endl;
    return 0;
}
