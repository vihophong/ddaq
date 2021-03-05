
#include "rcdaq_actions.h"

#include "parseargument.h"
#include "rcdaq.h"
#include "daq_device.h"
#include "rcdaq_plugin.h"
#include "all.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <rpc/pmap_clnt.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pthread.h"
#include "signal.h"

#include <vector>


#include <set>
#include <iostream>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#define RCDAQ_VERS 1

using namespace std;

static pthread_mutex_t M_output;

std::vector<RCDAQPlugin *> pluginlist;

static unsigned long  my_servernumber = 0;

//void rcdaq_1(struct svc_req *rqstp, register SVCXPRT *transp);


//-------------------------------------------------------------

int server_setup(int argc, char **argv)
{
  return 0;
}

//-------------------------------------------------------------

void plugin_register(RCDAQPlugin * p )
{
  pluginlist.push_back(p);
}

void plugin_unregister(RCDAQPlugin * p )
{
  // do nothing for now
  // we need to implement this once we add unloading of plugins
  // as a feature (don't see why at this point)
}

int daq_load_plugin( const char *sharedlib, std::ostream& os)
{
  void * v = dlopen(sharedlib, RTLD_GLOBAL | RTLD_NOW | RTLD_NOLOAD);
  if (v)
    {
      std::cout << "Plugin "
        << sharedlib << " already loaded" << std::endl;
      os << "Plugin "
        << sharedlib << " already loaded" << std::endl;
      return 0;
    }

  //! PHONG: at this step, plugin_register is called
  void * voidpointer = dlopen(sharedlib, RTLD_GLOBAL | RTLD_NOW);
  if (!voidpointer)
    {
      std::cout << "Loading of the plugin "
        << sharedlib << " failed: " << dlerror() << std::endl;
      os << "Loading of the plugin "
        << sharedlib << " failed: " << dlerror() << std::endl;
      return -1;

    }
  os << "Plugin " << sharedlib << " successfully loaded" << std::endl;
  cout  << "Plugin " << sharedlib << " successfully loaded" << std::endl;
  return 0;

}

//-------------------------------------------------------------

// for certain flags we add some info to the status output
// with this routine. It is called from daq_status.

int daq_status_plugin (const int flag, std::ostream& os )
{

  // in case we do have plugins, list them
  // if not, we just say "no plugins loded"
  if (   pluginlist.size() )
    {
      os << "List of loaded Plugins:" << endl;
    }
  else
    {
      os << "No Plugins loaded" << endl;
    }


  std::vector<RCDAQPlugin *>::iterator it;

  for ( it=pluginlist.begin(); it != pluginlist.end(); ++it)
    {
      (*it)->identify(os, flag);
    }
  return 0;
}

//-------------------------------------------------------------


shortResult * create_device_mod(deviceblock *db)
{
  static shortResult  result, error;
  static char e_string[512];
  static char r_string[512];
  error.str = e_string;
  result.str = r_string;
  strcpy ( r_string, " ");

  if ( daq_running() )
    {
      strcpy(e_string, "Run is active");
      error.content = 1;
      error.what    = 0;
      error.status  = -1;
      return &error;
    }

  strcpy(e_string, "Device needs at least 2 parameters");
  error.content = 1;
  error.what    = 0;
  error.status  = -1;

  result.status = 0;
  result.what   = 0;
  result.content= 0;


  int eventtype;
  int subid;

  int ipar[16];
  int i;


  if ( db->npar < 3)
    {
      strcpy(r_string, "Device needs at least 2 parameters");
      return &error;
    }


  //strcpy(r_string, "Wrong number of parameters");

  // and we decode the event type and subid
  eventtype  = get_value ( db->argv1); // event type
  subid = get_value ( db->argv2); // subevent id


  // now we will see what device we are supposed to set up.
  // we first check if it is one of our built-in ones, such as
  // device_random, or device_file or so.


  if ( strcasecmp(db->argv0,"device_random") == 0 )
    {

      // this happens to be the most complex contructor part
      // so far since there are a few variants, 2-6 parameters
      switch ( db->npar)
    {
    case 3:
          add_readoutdevice ( new daq_device_random( eventtype,
                                                     subid ));
          break;

        case 4:
          add_readoutdevice ( new daq_device_random( eventtype,
                                                     subid,
                             get_value ( db->argv3)));
          break;

        case 5:
          add_readoutdevice ( new daq_device_random( eventtype,
                                                     subid,
                             get_value ( db->argv3),
                             get_value ( db->argv4)));
          break;

        case 6:
          add_readoutdevice ( new daq_device_random( eventtype,
                                                     subid,
                             get_value ( db->argv3),
                             get_value ( db->argv4),
                             get_value ( db->argv5)));
          break;

        case 7:
          add_readoutdevice ( new daq_device_random( eventtype,
                                                     subid,
                             get_value ( db->argv3),
                             get_value ( db->argv4),
                             get_value ( db->argv5),
                             get_value ( db->argv6)));
          break;

       default:
          return &error;
          break;
        }

      return &result;
    }

  if ( strcasecmp(db->argv0,"device_deadtime") == 0 )
    {

      // this happens to be the most complex contructor part
      // so far since there are a few variants, 2-6 parameters
      switch ( db->npar)
    {
    case 3:
          add_readoutdevice ( new daq_device_deadtime( eventtype,
                               subid ));
          break;

        case 4:  // plus number of ticks
          add_readoutdevice ( new daq_device_deadtime( eventtype,
                               subid,
                               get_value ( db->argv3)));
          break;

        case 5:  // plus number of words
          add_readoutdevice ( new daq_device_deadtime( eventtype,
                               subid,
                               get_value ( db->argv3),
                               get_value ( db->argv4)));
          break;

        case 6:   // plus trigger flag
          add_readoutdevice ( new daq_device_deadtime( eventtype,
                               subid,
                               get_value ( db->argv3),
                               get_value ( db->argv4),
                               get_value ( db->argv5)));
          break;

       default:
          return &error;
          break;
        }

      return &result;
    }


  else if ( strcasecmp(db->argv0,"device_file") == 0 )
    {

      if ( db->npar < 4) return &error;

      if ( db->npar >= 5)
    {
      // we give the size in kbytes but we want it in words
      int s = 1024*(get_value(db->argv4)+3)/4;
      if ( s < 4*1024) s = 4*1024;    // this is the default size
      add_readoutdevice ( new daq_device_file( eventtype,
                           subid,
                           db->argv3,
                           0, // no delete
                           s ) );
      return &result;
    }
      else
    {

      add_readoutdevice ( new daq_device_file( eventtype,
                           subid,
                           db->argv3));
      return &result;
    }

    }
  // --------------------------------------------------------------------------
  // although this logic is very similar to device_file, since the consequences
  // of a misplaced parameter are so severe, we make a new device
  else if ( strcasecmp(db->argv0,"device_file_delete") == 0 )
    {

      if ( db->npar < 4) return &error;

      if ( db->npar >= 5)
    {
      // we give the size in kbytes but we want it in words
      int s = 1024*(get_value(db->argv4)+3)/4;
      if ( s < 4*1024) s = 4*1024;    // this is the default size
      add_readoutdevice ( new daq_device_file( eventtype,
                           subid,
                           db->argv3,
                           1,  // we add the delete flag
                           s ) );
      return &result;
    }
      else
    {

      add_readoutdevice ( new daq_device_file( eventtype,
                           subid,
                           db->argv3,
                           1) );
      return &result;
    }

    }


  // --------------------------------------------------------------------------


  else if ( strcasecmp(db->argv0,"device_filenumbers") == 0 )
    {

      if ( db->npar < 4) return &error;

      if ( db->npar >= 5)
    {
      int s = 1024*(get_value(db->argv4)+3)/4;
      if ( s < 256) s = 256;    // this is the default size

      add_readoutdevice ( new daq_device_filenumbers( eventtype,
                              subid,
                              db->argv3,
                              0,  // no delete
                              s));
      return &result;
    }
      else
    {

      add_readoutdevice ( new daq_device_filenumbers( eventtype,
                              subid,
                              db->argv3));
      return &result;
    }

    }

  // --------------------------------------------------------------------------


  else if ( strcasecmp(db->argv0,"device_filenumbers_delete") == 0 )
    {

      if ( db->npar < 4) return &error;

      if ( db->npar >= 5)
    {
      int s = 1024*(get_value(db->argv4)+3)/4;
      if ( s < 256) s = 256;    // this is the default size

      add_readoutdevice ( new daq_device_filenumbers( eventtype,
                              subid,
                              db->argv3,
                              1,  // we add the delete flag
                              s));
      return &result;
    }
      else
    {

      add_readoutdevice ( new daq_device_filenumbers( eventtype,
                              subid,
                              db->argv3,
                              1) );
      return &result;
    }

    }


  else if ( strcasecmp(db->argv0,"device_command") == 0 )
    {

      if ( db->npar < 4) return &error;

      if ( db->npar >= 5)
    {
      int s = (get_value(db->argv4)+3)/4;
      if ( s < 1280) s = 1280;    // this is the default size
      add_readoutdevice ( new daq_device_file( eventtype,
                           subid,
                           db->argv3,
                           get_value(db->argv4) ) );
      return &result;
    }
      else
    {

      add_readoutdevice ( new daq_device_command( eventtype,
                           subid,
                           db->argv3));
      return &result;
    }

    }
  else if ( strcasecmp(db->argv0,"device_rtclock") == 0 )
    {

      add_readoutdevice ( new daq_device_rtclock( eventtype,
                           subid));

      return &result;
    }


  // nada, it was none of the built-in ones if we arrive here.

  // we now go through through the list of plugins and see if any one of
  // our plugins can make the device.

  // there are three possibilities:
  //  1) the plugin can make the device, all done. In that case, return = 0
  //  2) the plugin can make the device but the parameters are wrong, return  = 1
  //  3) the plugin dosn not knwo about tha device, we keep on going...  return = 2

  // we keep doing that until we either find a plugin that knows the device or  we run
  // out of plugins

  std::vector<RCDAQPlugin *>::iterator it;

  int status;
  for ( it=pluginlist.begin(); it != pluginlist.end(); ++it)
    {
      status = (*it)->create_device(db);
      // in both of the following cases we are done here:
      if (status == 0) return &result;  // sucessfully created
      else if ( status == 1) return &error;  // it's my device but wrong params
      // in all other cases we continue and try the next plugin
    }

  result.content=1;
  strcpy(r_string, "Unknown device");
  return &result;
}

//-------------------------------------------------------------

int
main (int argc, char **argv)
{

  int i;

  pthread_mutex_init(&M_output, 0);

  int servernumber = 0;

  if ( argc > 1)
    {
      servernumber = get_value(argv[1]);
    }
  int commport=BASECOMMPORT+servernumber;
  std::cout << "Server number is " << servernumber << std::endl;


  zmq::context_t* ctx=new zmq::context_t;
  rcdaq_init(ctx, M_output,servernumber);

  server_setup(argc, argv);

  my_servernumber = servernumber;  // remember who we are for later

  char hostname[1024];
  i = gethostname(hostname, 1024);
  i = daq_set_name(hostname);

  //! zmq communication
  //zmq::context_t ctx;
  zmq::socket_t comm_socket(*ctx, zmq::socket_type::rep);

  char tmpcmd[500];
  sprintf(tmpcmd,"tcp://*:%d",commport);
  comm_socket.bind(tmpcmd);
  std::cout << "bind ZMQ tcp socket for communication: " << tmpcmd << std::endl;


  static std::ostringstream outputstream;
  while(1){
      std::vector<zmq::message_t> recv_msgs;
      zmq::recv_result_t resultmess =
        zmq::recv_multipart(comm_socket, std::back_inserter(recv_msgs));
      assert(resultmess && "recv failed");
      int nmess=(int)*resultmess;
      if (recv_msgs[0].to_string()=="daq_begin")
      {
          pthread_mutex_lock(&M_output);
          int runno=0;
          if (nmess==2) runno=atoi(recv_msgs[1].to_string().data());
          outputstream.str("");
          int status = daq_begin (runno, outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_end"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_end(outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_open"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_open(outputstream);
          if (status) comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          else comm_socket.send(zmq::str_buffer("Enabled logging"));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_close"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_close(outputstream);
          if (status) comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          else comm_socket.send(zmq::str_buffer("Disabled logging"));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_shutdown"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_shutdown(my_servernumber, RCDAQ_VERS, outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_define_runtype"){
          pthread_mutex_lock(&M_output);
          if (nmess==3)
              int status = daq_define_runtype(recv_msgs[1].to_string().data(),recv_msgs[2].to_string().data());
          std::string outtmp=recv_msgs[1].to_string()+" "+recv_msgs[2].to_string();
          comm_socket.send(zmq::buffer(outtmp.data(),outtmp.length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_set_runtype"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2){
              int status = daq_setruntype(recv_msgs[1].to_string().data(),outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_set_maxvolume"){
          pthread_mutex_lock(&M_output);
          int maxvolume=2000;
          if (nmess==2) maxvolume=atoi(recv_msgs[1].to_string().data());
          outputstream.str("");
          int status = daq_setmaxvolume (maxvolume, outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_status"){
          pthread_mutex_lock(&M_output);
          int long_flag=1;
          if (nmess==2) long_flag=atoi(recv_msgs[1].to_string().data());
          outputstream.str("");
          int status = daq_status(long_flag, outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="load"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int status = daq_load_plugin(recv_msgs[1].to_string().data(),outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_get_runtype"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_getruntype(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_list_runtypes"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_list_runtypes(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_setname"){
          pthread_mutex_lock(&M_output);
          if (nmess==2)
          {
              int status = daq_set_name(recv_msgs[1].to_string().data());
              std::string outtmp=recv_msgs[1].to_string()+" set";
              comm_socket.send(zmq::buffer(outtmp.data(),outtmp.length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_getname"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_get_name(outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_fake_trigger"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==3)
          {
              int inp1=atoi(recv_msgs[1].to_string().data());
              int inp2=atoi(recv_msgs[2].to_string().data());
              std::string outtmp="DAQ fake trigger with parms: "+recv_msgs[1].to_string()+" and "+recv_msgs[2].to_string()+" set";
              int status = daq_fake_trigger(inp1,inp2);
              comm_socket.send(zmq::buffer(outtmp.data(),outtmp.length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_list_readlist"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_list_readlist(outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_clear_readlist"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_list_readlist(outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_set_maxevents"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_setmaxevents(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_set_maxbuffersize"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_setmaxbuffersize(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_set_adaptivebuffering"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_setadaptivebuffering(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_webcontrol"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==2)
          {
              int inp=atoi(recv_msgs[1].to_string().data());
              int status = daq_webcontrol(inp,outputstream);
              comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="daq_get_lastfilename"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          int status = daq_getlastfilename(outputstream);
          comm_socket.send(zmq::buffer(outputstream.str().data(),outputstream.str().length()));
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="elog"){
          pthread_mutex_lock(&M_output);
          outputstream.str("");
          if (nmess==4)
          {
              int inp=atoi(recv_msgs[2].to_string().data());
              std::string outtmp="Elog handler with parms: "+recv_msgs[1].to_string()+" and "+recv_msgs[2].to_string()+" and "+recv_msgs[3].to_string()+" set";
              int status = daq_set_eloghandler(recv_msgs[1].to_string().data(),inp,recv_msgs[3].to_string().data());
              comm_socket.send(zmq::buffer(outtmp.data(),outtmp.length()));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else if (recv_msgs[0].to_string()=="create_device"){
          pthread_mutex_lock(&M_output);
          if (nmess>1){
              struct deviceblock dbmod;
              dbmod.npar=0;
              char **arglist[NSTRINGPAR];
              arglist[0] = &dbmod.argv0;
              arglist[1] = &dbmod.argv1;
              arglist[2] = &dbmod.argv2;
              arglist[3] = &dbmod.argv3;
              arglist[4] = &dbmod.argv4;
              arglist[5] = &dbmod.argv5;
              arglist[6] = &dbmod.argv6;
              arglist[7] = &dbmod.argv7;
              arglist[8] = &dbmod.argv8;
              arglist[9] = &dbmod.argv9;
              arglist[10] = &dbmod.argv10;
              arglist[11] = &dbmod.argv11;
              arglist[12] = &dbmod.argv12;
              arglist[13] = &dbmod.argv13;
              char empty[2] = {' ',0};
              int i;
              for ( i = 0; i < NSTRINGPAR; i++)
                {
                  *arglist[i] = empty;
                }

              for (int i=1;i<nmess;i++){
                  *arglist[dbmod.npar]=new char[500];
                  strcpy(*arglist[dbmod.npar],(char*) recv_msgs[i].to_string().data());//argv[i];
                  dbmod.npar++;
                  if ( dbmod.npar >= NSTRINGPAR)
                    {
                        cout << "Too many parameters to handle for me" << endl;
                        break;
                    }
              }
              shortResult* res=create_device_mod(&dbmod);
              comm_socket.send(zmq::buffer(res->str,strlen(res->str)));
          }else{
              comm_socket.send(zmq::str_buffer("Invalid command"));
          }
          pthread_mutex_unlock(&M_output);
      }else{
          comm_socket.send(zmq::str_buffer("Invalid command"));
      }
  }
  exit (1);
  /* NOTREACHED */
}

