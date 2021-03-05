#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include "syncSpyClass.hpp"
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */
#include <unistd.h>
int main(){
  SpyDataSink MyDataSink;
    MyDataSink= SpyDataSink( "/dev/shm/SHM_110205");
    while(1){
      std::vector<uint32_t> buffer;
      int x=0;
      x=MyDataSink.Spy(buffer);
      std::cout<<"Size1="<<std::dec<<x<<" size2="<<buffer.size()<<std::endl;
      for (int j=0;j<buffer.size();j++){
	std::cout<<std::hex<<buffer[j]<<std::endl;
      }
      usleep(100000);
    }


}
