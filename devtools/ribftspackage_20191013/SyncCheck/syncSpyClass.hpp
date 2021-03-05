/***********************************************************************************
 >  *        File:   syncSpy.cpp                                                 *
 >  * Description:   read the "DataSink" (MIDAS data shared program) 
 >  *                                  shared memory directly                *
 >  *      Author:   Alvaro Tolosa Delgado  IFIC/CSIC spain                           *
 >  *Site Contact:   alvaro.tolosa@ific.uv.es                                           *
 >  * Last Change:   02/11/2016                                                    *
 >  *******************************************************************************/

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>
#include<vector>
#define BUFFERSIZE 1024
class SpyDataSink
{

public:
  SpyDataSink(){};
  SpyDataSink(const std::string & Addr , int size ){
    MyAddr=Addr;
	_Size=size;
	LastTimestamp=1;
//	std::ifstream   input(MyAddr.c_str(), std::ios::binary );
  };
  void Test(){
    std::cout<<"Ready to connect on:"<<MyAddr<<std::endl;
  }
  int Spy(char * buffer){

    	std::vector<uint32_t> MyBuffer;
	uint32_t DummyBuffer;
     bool StoreData=false;
    std::ifstream   input(MyAddr.c_str(), std::ios::binary );
   uint16_t lineCounter(0);
   while(input.read((char *)&DummyBuffer,sizeof(DummyBuffer)) && lineCounter<_Size){

	if(lineCounter<6){
  	        memcpy(buffer+lineCounter*4,&DummyBuffer,sizeof(DummyBuffer));
		MyBuffer.push_back(DummyBuffer);
		lineCounter++;
     	}
	else{

//	else if(DummyBuffer==0x12){
//		StoreData=true;
//	}
//    	if(StoreData ){
//	std::cout << std::dec <<lineCounter<< " "<< MyAddr.c_str()  << std::hex<<"----0x" << DummyBuffer<<"\t" ;
		if(lineCounter>401){memcpy(buffer+(lineCounter-402)*4,&DummyBuffer,sizeof(DummyBuffer));MyBuffer.push_back(DummyBuffer);}
//if(DummyBuffer>100)std::cout<<"LastTimestamp "<< LastTimestamp << " DummyBuffer 0x"<< std::hex<<DummyBuffer<< " lineCounter "<< std::dec<<lineCounter<<std::endl; 
		if(lineCounter==402){   // lokk to first TS low to check already read buffers
		//	std::cout<<"LastTimestamp "<< LastTimestamp << "DummyBuffer"<< DummyBuffer<<std::endl; 
			if(LastTimestamp==DummyBuffer){ return 0;}
			else LastTimestamp=DummyBuffer;
		}
		lineCounter++;
		

 //     		if(lineCounter<30) 	std::cout << std::hex <<"----0x" << DummyBuffer<<"\t";
 //     		if(lineCounter%10==0)	std::cout<< lineCounter <<std::endl;
  	}
     uint32_t ToClear=0;
//     input.write((char *)&ToClear,sizeof(ToClear)); //erase memory
 }
//	if(MyAddr=="/dev/shm/SHM_110207"){
/*	int count=0;
	int count2=0;
	for(auto it:MyBuffer){
		std::cout<<std::dec <<count2<<" " << std::hex<<" 0x"<<std::setw(8) << std::setfill('0') << it<<"\t";
		if(count>4){std::cout  <<std::endl;count=0;}
		count++;
		count2++;
		
	}
*/
//}



	

 if(0!=lineCounter)  std::cout<<"Total Words read "<< lineCounter <<std::endl;
   input.close();
 //  buffer=(char *)&MyBuffer[0];
//std::ofstream ofs;
//ofs.open(MyAddr.c_str(), std::ofstream::out | std::ofstream::trunc);
//ofs.close();
/*   std::ofstream   output(MyAddr.c_str(), std::ios::binary );
 	output.seekp(12);
    uint32_t ToClear=0;
     output.write((char *)&ToClear,sizeof(ToClear)); //erase memory
	output.close();*/
   return lineCounter;
  

  };
private:
  std::string MyAddr;
	uint32_t LastTimestamp;
	int _Size;
};
