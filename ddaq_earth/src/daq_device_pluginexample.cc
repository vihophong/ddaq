#include "daq_device_pluginexample.h"

#include <strings.h>

 
using namespace std;

daq_device_pluginexample::daq_device_pluginexample(const int eventtype
    , const int subeventid
    , const int n_words
    , const int trigger_enabled)
{

  m_eventType  = eventtype;
  m_subeventid = subeventid;
  number_of_words = n_words;
  if (trigger_enabled) 
    {
      th = new pulserTriggerHandler();
      registerTriggerHandler(th);
    }
  else
    {
      th = 0;
    }
}

daq_device_pluginexample::~daq_device_pluginexample()
{
  clearTriggerHandler();
  delete th;
}



// the put_data function

int daq_device_pluginexample::put_data(const int etype, int * adr, const int length )
{

  int len = 0;

  if (etype != m_eventType )  // not our id
    {
      return 0;
    }


  sevt =  (subevtdata_ptr) adr;
  // set the initial subevent length
  sevt->sub_length =  SEVTHEADERLENGTH;

  // update id's etc
  sevt->sub_id =  m_subeventid;
  sevt->sub_type=4;
  sevt->sub_decoding = ID4EVT;
  sevt->reserved[0] = 0;
  sevt->reserved[1] = 0;

  unsigned int data;

  int  *d = (int *) &sevt->data;

  int ia;
 
  for ( ia = 0; ia < number_of_words; ia++)
    {
	 *d++  = ia;
	 len++;
    }
  
  
  sevt->sub_padding = len%2;
  len = len + (len%2);
  sevt->sub_length += len;
  return  sevt->sub_length;
}


void daq_device_pluginexample::identify(std::ostream& os) const
{
  
  os  << "Plugin Device  Event Type: " << m_eventType << " Subevent id: " << m_subeventid 
      << " n_words: " << number_of_words; 

    if (th) 
      {
	os << " ** Trigger enabled";
      }

  os << endl;

}

int daq_device_pluginexample::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (number_of_words + SEVTHEADERLENGTH);
}

int  daq_device_pluginexample::init()
{

  return 0;
}

// the rearm() function
int  daq_device_pluginexample::rearm(const int etype)
{
  if (etype != m_eventType) return 0;
  return 0;
}


