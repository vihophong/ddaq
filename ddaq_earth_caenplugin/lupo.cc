
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>

#include <sstream>
#include <strings.h>

#include <lupo_lib.h>

int get_value_hex(const char *arg){
    std::istringstream sarg ( arg);
    int value;
    std::string s = arg;

    int x =  s.find("0x",0);


    if (x == -1 )
      {
        sarg >> value;
      }
    else
      {
        sarg >> std::hex >> value;
      }

    return value;
}
using namespace  std;
int main(int argc, char *argv[])
{
    if (argc!=5) {
        std::cout<<"Usage: ./lupo linknumber boardnumber VMEaddress outputnumber"<<std::endl;
        return 0;
    }
    int linknumber  = atoi(argv[1]);
    int boardnumber  = atoi(argv[2]);
    int vmeaddr = get_value_hex(argv[3]);
    int outputno = atoi(argv[4]);
    lupoLib* lupo = new lupoLib(vmeaddr);
    lupo->lupoConnect(linknumber,boardnumber);
    if ( !lupo->getConnectStatus() ){
          cout<<"Error with connection!"<<endl;
          exit(0);
     }
    lupo->sendPulse(outputno);
    cout<<"A pulse sent to output No.: "<<outputno<<endl;
    return 0;
}

