#ifndef __ONCSSUB_IDCAENV1730PHA_H__
#define __ONCSSUB_IDCAENV1730PHA_H__


#include "oncsSubevent.h"
#include <vector>
using namespace std;

typedef struct{
    int size;
    int board_id;
    bool board_fail_flag;
    int pattern;
    int dual_ch_mask;
    int counter;
    int timetag;
    int print(){
        cout<<"board aggregate:"<<endl;
        cout<<"boardaggr_t->size = "<<size<<endl;
        cout<<"boardaggr_t->board_id = "<<board_id<<endl;
        cout<<"boardaggr_t->board_fail_flag = "<<board_fail_flag<<endl;
        cout<<"boardaggr_t->pattern = "<<pattern<<endl;
        cout<<"boardaggr_t->dual_ch_mask = "<<dual_ch_mask<<endl;
        cout<<"boardaggr_t->counter = "<<counter<<endl;
        cout<<"boardaggr_t->timetag = "<<timetag<<endl;
        cout<<"--------------"<<endl;
        return 0;
    }
}boardaggr_t;

typedef struct{
    int groupid;
    bool formatinfo;
    int size;
    bool dual_trace_flag;
    bool energy_enable_flag;
    bool trigger_time_stamp_enable_flag;
    bool extra2_enable_flag;
    bool waveformsample_enable_flag;
    short extra_option_enable_flag;
    short analogprobe1_selection;
    short analogprobe2_selection;
    short digitalprobe_selection;
    int n_samples;
    int print(){
        cout<<"channel aggregate:"<<endl;
        cout<<"channelaggr_t->id = "<<groupid<<endl;
        cout<<"channelaggr_t->formatinfo = "<<formatinfo<<endl;
        cout<<"channelaggr_t->size = "<<size<<endl;
        cout<<"channelaggr_t->dual_trace_flag = "<<dual_trace_flag<<endl;
        cout<<"channelaggr_t->energy_enable_flag = "<<energy_enable_flag<<endl;
        cout<<"channelaggr_t->trigger_time_stamp_enable_flag = "<<trigger_time_stamp_enable_flag<<endl;
        cout<<"channelaggr_t->extra2_enable_flag = "<<extra2_enable_flag<<endl;
        cout<<"channelaggr_t->waveformsample_enable_flag = "<<waveformsample_enable_flag<<endl;
        cout<<"channelaggr_t->extra_option_enable_flag = "<<extra_option_enable_flag<<endl;
        cout<<"channelaggr_t->analogprobe1_selection = "<<analogprobe1_selection<<endl;
        cout<<"channelaggr_t->analogprobe2_selection = "<<analogprobe2_selection<<endl;
        cout<<"channelaggr_t->digitalprobe_selection = "<<digitalprobe_selection<<endl;
        cout<<"channelaggr_t->n_samples = "<<n_samples<<endl;
        cout<<"--------------"<<endl;
        return 0;
    }
}channelaggr_t;
typedef struct{
    int ch;
    int trigger_time_tag;
    int n_samples;
    int extras2;
    int extras;
    bool pileup_rollover_flag;
    int energy;
    vector <int> ap1_sample;
    vector <int> ap2_sample;
    vector <int> dp_sample;
    vector <int> trg_sample;

    int print(){
        cout<<"channel data:"<<endl;
        cout<<"channel_t->ch = "<<ch<<endl;
        cout<<"channel_t->trigger_time_tag = "<<trigger_time_tag<<endl;
        cout<<"channel_t->n_samples = "<<n_samples<<endl;
        cout<<"channel_t->extras2 = "<<extras2<<endl;
        cout<<"channel_t->extras = "<<extras<<endl;
        cout<<"channel_t->pileup_rollover_flag = "<<pileup_rollover_flag<<endl;
        cout<<"channel_t->energy = "<<energy<<endl;
        cout<<"channel_t->ap1_sample_size = "<<ap1_sample.size()<<endl;
        cout<<"channel_t->ap2_sample_size = "<<ap2_sample.size()<<endl;
        cout<<"channel_t->dp_sample_size = "<<dp_sample.size()<<endl;
        cout<<"channel_t->trg_sample_size = "<<trg_sample.size()<<endl;
        cout<<"--------------"<<endl;
        return 0;
    }
}channel_t;

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idcaenv1730pha : public  oncsSubevent_w4 {
#else
class  oncsSub_idcaenv1730pha : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idcaenv1730pha( subevtdata_ptr);

  int    iValue(const int sample, const int ch);
  int    iValue(const int,const char *);
  void  dump ( OSTREAM& os = COUT) ;
  int *getIntArray(int *nwout, const char *what="");
  int getHitFormat() const;

  int convert();
protected:
  channel_t channel_data;
  channelaggr_t channelaggr_data;
  boardaggr_t boardaggr_data;

  int pos;

};



#endif /* __oncsSub_idcaenv1730pha_H__ */
