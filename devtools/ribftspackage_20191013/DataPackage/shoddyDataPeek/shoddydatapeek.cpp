#include <iostream>

#include <fstream>



#include <stdint.h>

int main()
{

  std::ifstream input( "/dev/shm/SHM_110205", std::ios::binary );

  uint32_t buffer[4]; //read 4-words each time

  uint16_t lineCounter(0);

  while(input.read((char *)&buffer,sizeof(buffer)) ){

  if( buffer[0] || buffer[1]|| buffer[2]|| buffer[3])

  std::cerr << std::hex <<"0x"<< buffer[0] << "\t" <<"0x"<< buffer[1]<< "\t" <<"0x"<< buffer[2]<< "\t" <<"0x"<< buffer[3] << "\t"<< std::dec << lineCounter <<
std::endl;

  ++lineCounter;

  }

input.close();
return 0;

}
