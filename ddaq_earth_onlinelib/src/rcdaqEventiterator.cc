//
// rcdaqeventIterator   mlp 4/19/1997
//
// this iterator reads events froma data file. 

#define MONITORINGPORT 6666

#include "rcdaqEventiterator.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "oncsBuffer.h"
//#include "gzbuffer.h"
//#include "lzobuffer.h"
#include "Event.h"

#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>




using namespace std;


// there are two similar constructors, one with just the
// filename, the other with an additional status value
// which is non-zero on return if anything goes wrong. 

rcdaqEventiterator::~rcdaqEventiterator()
{
     if (_sockfd) close (_sockfd);
     if (bp != NULL ) delete [] bp;
     if (bptr != NULL ) delete bptr;
}  


rcdaqEventiterator::rcdaqEventiterator()
{
  string host = "localhost";
    
  if ( getenv("RCDAQHOST")  )
    {
      host = getenv("RCDAQHOST");
    }

  int status;
  setup (host.c_str(), status);
}  

rcdaqEventiterator::rcdaqEventiterator(const char *ip)
{
  int status;
  setup (ip, status);
}  

rcdaqEventiterator::rcdaqEventiterator(const char *ip, int &status)
{
  setup (ip, status);
}  


int rcdaqEventiterator::setup(const char *ip, int &status)
{
  _defunct = 0;

  
  struct hostent *p_host;
  p_host = gethostbyname(ip);
  
  //std::cout << __FILE__ << " " << __LINE__ << "  " << ip << "  " << p_host->h_name << "  " << p_host->h_addr << std::endl;

  
  if ( ! p_host ) 
    {
      status = -2;
      _defunct = 1;
      return -2;
    }

  //  std::cout << p_host->h_name << "  " << p_host->h_addr << std::endl;

  bptr = 0;
  bp = 0;
  allocatedsize = 0;
  _theIP = p_host->h_name;
  status = 0;
  last_read_status = 0;
  current_index = 0;

  ctx = new zmq::context_t;
  //  Prepare subscriber
  online = new zmq::socket_t(*ctx, zmq::socket_type::sub);
  char tmpcmd[500];
  if (strcmp(ip,"")==0){
      sprintf(tmpcmd,"tcp://localhost:%d",MONITORINGPORT);
  }else{
      sprintf(tmpcmd,"tcp://%s:%d",ip,MONITORINGPORT);
  }
  std::cout<<"ZMQ connected "<<tmpcmd<<std::endl;
  online->connect(tmpcmd);
  online->set(zmq::sockopt::subscribe, "");//subcribe all

  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  bcopy(p_host->h_addr, &(server.sin_addr.s_addr), p_host->h_length);
  server.sin_port = htons(MONITORINGPORT);

  return 0;
}

void rcdaqEventiterator::identify (OSTREAM &os) const
{ 
  os << getIdTag();
  if ( _defunct ) os << " *** defunct";
  os << std::endl;

};

const char * rcdaqEventiterator::getIdTag () const
{ 
  static char line[180];
  strcpy (line, " -- rcdaqEventiterator reading from ");
  strcat (line, _theIP.c_str());
  return line;
};


// and, finally, the only non-constructor member function to
// retrieve events from the iterator.

Event * rcdaqEventiterator::getNextEvent()
{


  if ( _defunct ) return 0;
  Event *evt = 0;
  // if we had a read error before, we just return
  if (last_read_status) return NULL;

  // see if we have a buffer to read
  if (bptr == 0) 
    {
      if ( (last_read_status = read_next_buffer()) !=0 )
    {
	  return NULL;
	}
    }
  while (last_read_status == 0)
    {
      if (bptr) evt =  bptr->getEvent();
      if (evt) return evt;
      last_read_status = read_next_buffer();
    }
  return NULL;
}

// -----------------------------------------------------
// this is a private function to read the next buffer
// if needed. 

int rcdaqEventiterator::read_next_buffer()
{
  int ip = 0;
  if (bptr) 
    {
      delete bptr;
      bptr = 0;
    }

  //! read buffer size
  std::vector<zmq::message_t> recv_msgs;
  zmq::recv_result_t result =
          zmq::recv_multipart(*online, std::back_inserter(recv_msgs));
  assert(result && "recv failed");
  int id = atoi(recv_msgs[0].to_string().erase(0,1).data());
  int* reportbufferno = (int*) recv_msgs[1].data();
  int* reportsize = (int*) recv_msgs[2].data();
  buffer_size = recv_msgs[3].size();//buffer size raw
  //! allocate PHDWORD bp
  int i;
  if (bp)
    {
      if  (buffer_size > allocatedsize*4)
	{
	  delete [] bp;
	  i = (buffer_size +8191) /8192;
	  allocatedsize = i * 2048;
          bp = new PHDWORD[allocatedsize];
	}
    }
  else
    {
      i = (buffer_size +8191) /8192;
      allocatedsize = i * BUFFERBLOCKSIZE/4;
      bp = new PHDWORD[allocatedsize];
    }
  // not nice but Ok for now
  memcpy(bp,recv_msgs[3].data(),buffer_size);
  //PHDWORD* d=(PHDWORD*) recv_msgs[3].data();
  return buffer::makeBuffer( bp, allocatedsize, &bptr);
}