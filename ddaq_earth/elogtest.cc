
#include <iostream>
#include <eloghandler.h>

int main(int argc, char *argv[])
{
  if (argc==2){
      int theport=atoi(argv[1]);
      ElogHandler *x = new ElogHandler("localhost", theport, "RCDAQLog");

      x->BegrunLog(104,"mmm", "run_xxx");
      x->EndrunLog(104,"mmm", 24567);
      delete x;
       std::cout<<"Wrote elog entries on localhost, elog name RCDAQLog, port="<<theport<<std::endl;
  }else{
      std::cout<<"please specify Elog port"<<std::endl;
  }

}
