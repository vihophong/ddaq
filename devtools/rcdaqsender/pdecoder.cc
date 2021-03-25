
#include "myanalysis.cc"
#include <stdint.h>

int CFD_delay=1;//in Tclk unit, 1 Tclk=2ns
double CFD_fraction=0.5;
double LED_threshold=100;
double LED_threshold_LED=100;
double LED_threshold_740=40;
double LED_threshold_LED_740=40;
int gateOffset=10;
int shortGate=20;
int longGate=150;
int nBaseline=16;
int minVarBaseline=100; //criteria for baseline determination
int mode_selection=0;


//! dont care about the following code
uint16_t seperateint(int inint,bool islsb){
  if (islsb) return (uint16_t) (inint&0xFFFF);
  else return (uint16_t) (inint>>16);
}

NIGIRIHit* data;

int init_done = 0;
int pinit()
{
  if (init_done) return 1;
  init_done = 1;
  gROOT->ProcessLine(".L libDataStruct.so");
  data = new NIGIRIHit;
  Init();
  return 0;
}

int process_event (Event * e)
{  

  //! lupo(tdc) packet
  Packet *tdcp=e->getPacket(TDC_PACKET);
  if(tdcp){
      int* temp;
      int* gg;
      gg=(int*) tdcp->getIntArray(temp);
      int size=tdcp->getPadding();
      data->Clear();
      data->evt_type = TDC_EVENT_TYPE;
      data->b = TDC_BOARD_N;
      data->ch = 0;
      UInt_t tslsb = (UInt_t)gg[3];
      UInt_t tsmsb = (UInt_t)gg[2];
      data->ts = (((ULong64_t)tsmsb<<32)&0xFFFF00000000)|(ULong64_t)tslsb;//resolution is 16 ns!
      data->clong= gg[1];//daq counter
      data->evt = gg[0];
      //! more tdc data here!
      
      ProcessEvent(data);
      delete tdcp;
  }


  //! v1740 packet
  Packet *p1740_1=e->getPacket(V1740_PACKET_1);
  if (p1740_1)
    {
      int* temp;
      int* gg;
      gg=(int*) p1740_1->getIntArray(temp);
      int size=p1740_1->getPadding();
      //! content
      int k=V1740_HDR+V1740_N_MAX_CH;
      for (int i=0;i<V1740_N_CH;i++){
        //! header
        data->Clear();
        data->evt_type = V1740_EVENT_TYPE;
        data->b = 0;//for sorter
        data->evt = gg[2]+1;//this event start from 0
        data->overrange = (Char_t) gg[1];//intepret as channel(group) mask
        UInt_t tslsb = (UInt_t)gg[5];
        UInt_t tsmsb = (UInt_t)gg[4];
        data->ts = (((ULong64_t)tsmsb<<32)&0xFFFF00000000)|(ULong64_t)tslsb;//resolution is 16 ns!
        data->ch = i;//for sorter
	//data->ch = gg[1];//for sorter
        ProcessEvent(data);
    /*
      if (gg[2]==1000) {
        for (int ii=0;ii<2;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
        for (int ii=nsample-3;ii<nsample;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
      }
    */
      }
       delete p1740_1;
  }
  Packet *p1740_2=e->getPacket(V1740_PACKET_2);
  if (p1740_2)
    {
      int* temp;
      int* gg;
      gg=(int*) p1740_2->getIntArray(temp);
      int size=p1740_2->getPadding();
      //! content
      int k=V1740_HDR+V1740_N_MAX_CH;
      for (int i=0;i<V1740_N_CH;i++){
        //! header
        data->Clear();
        data->evt_type = V1740_EVENT_TYPE;
        data->b = 1;//for sorter
        data->evt = gg[2]+1;//this event start from 0
        data->overrange = (Char_t) gg[1];//intepret as channel(group) mask
        UInt_t tslsb = (UInt_t)gg[5];
        UInt_t tsmsb = (UInt_t)gg[4];
        data->ts = (((ULong64_t)tsmsb<<32)&0xFFFF00000000)|(ULong64_t)tslsb;//resolution is 16 ns!
        data->ch = i;//for sorter
	//data->ch = gg[1];//for sorter
        ProcessEvent(data);
    /*
      if (gg[2]==1000) {
        for (int ii=0;ii<2;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
        for (int ii=nsample-3;ii<nsample;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
      }
    */
      }
       delete p1740_2;
  }

  Packet *p1740_3=e->getPacket(V1740_PACKET_3);
  if (p1740_3)
    {
      int* temp;
      int* gg;
      gg=(int*) p1740_3->getIntArray(temp);
      int size=p1740_3->getPadding();
      //! content
      int k=V1740_HDR+V1740_N_MAX_CH;
      for (int i=0;i<V1740_N_CH;i++){
        //! header
        data->Clear();
        data->evt_type = V1740_EVENT_TYPE;
        data->b = 2;//for sorter
        data->evt = gg[2]+1;//this event start from 0
        data->overrange = (Char_t) gg[1];//intepret as channel(group) mask
        UInt_t tslsb = (UInt_t)gg[5];
        UInt_t tsmsb = (UInt_t)gg[4];
        data->ts = (((ULong64_t)tsmsb<<32)&0xFFFF00000000)|(ULong64_t)tslsb;//resolution is 16 ns!
        data->ch = i;//for sorter
	//data->ch = gg[1];//for sorter
        ProcessEvent(data);
    /*
      if (gg[2]==1000) {
        for (int ii=0;ii<2;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
        for (int ii=nsample-3;ii<nsample;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
      }
    */
      }
       delete p1740_3;
  }
  Packet *p1740_4=e->getPacket(V1740_PACKET_4);
  if (p1740_4)
    {
      int* temp;
      int* gg;
      gg=(int*) p1740_4->getIntArray(temp);
      int size=p1740_4->getPadding();
      //! content
      int k=V1740_HDR+V1740_N_MAX_CH;
      for (int i=0;i<V1740_N_CH;i++){
        //! header
        data->Clear();
        data->evt_type = V1740_EVENT_TYPE;
        data->b = 3;//for sorter
        data->evt = gg[2]+1;//this event start from 0
        data->overrange = (Char_t) gg[1];//intepret as channel(group) mask
        UInt_t tslsb = (UInt_t)gg[5];
        UInt_t tsmsb = (UInt_t)gg[4];
        data->ts = (((ULong64_t)tsmsb<<32)&0xFFFF00000000)|(ULong64_t)tslsb;//resolution is 16 ns!
        data->ch = i;//for sorter
	//data->ch = gg[1];//for sorter
        ProcessEvent(data);
    /*
      if (gg[2]==1000) {
        for (int ii=0;ii<2;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
        for (int ii=nsample-3;ii<nsample;ii++) cout<<i<<" "<<nsample<<" "<<ii<<" "<<WaveLine[ii]<<endl;
      }
    */
      }
       delete p1740_4;
  }



  return 0;
}

int pclose(){
    CloseMe();
    cout<<"Finish!"<<endl;
    return 0;
}


