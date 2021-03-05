#ifndef __ELOGHANDLER_H
#define __ELOGHANDLER_H


#include <string>



class ElogHandler {

public:

  //** Constructors

  ElogHandler (const std::string h, const int p, const std::string name);

  virtual ~ElogHandler() {};

  virtual int BegrunLog ( const int run, std::string who, std::string filename);
  virtual int EndrunLog ( const int run, std::string who, const int events, const double volume=0, time_t starttime=0);

  virtual std::string getHost()        const {return hostname;};
  virtual int getPort()               const {return port;};
  virtual std::string getLogbookName() const {return logbookname;};
  

protected:

  std::string hostname;
  std::string logbookname;
  int port;


};

#endif


