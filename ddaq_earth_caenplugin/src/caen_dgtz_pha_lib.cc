#include "caen_dgtz_pha_lib.h"

//!* *************************************************************************************** */
//!* V1730 lib
//!* *************************************************************************************** */

DigitizerParams_t getDigitizerParams(char* filename)
{
  DigitizerParams_t Params;
  //! set default parms
  Params.LinkType = CAEN_DGTZ_OpticalLink;
  Params.VMEBaseAddress = 0;
  Params.LinkNum = 0;
  Params.BoardNum = 0;
  Params.RecordLength = 10000;
  Params.ChannelMask = 0xFFFF;
  Params.EventAggr = 0;//default let library to choose reasonable value
  Params.AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_List;//default let library to choose reasonable value
  Params.IOlev = CAEN_DGTZ_IOLevel_NIM;//default let library to choose reasonable value
  Params.ExtTriggerInputMode = CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;//default let library to choose reasonable value
  Params.nreg = 0;
  for (int i=0;i<MaxNChannels;i++){
      Params.PulsePolarity[i]=CAEN_DGTZ_PulsePolarityPositive;
      Params.DCOffset[i]=(int)((20+50) * 65535 / 100);
      Params.PreTriggerSize[i]=1000; //2000 ns
  }

  Params.analogProbes[0]=3;
  Params.analogProbes[1]=0;
  Params.digitalProbes=13;

//  Params.virtualProbeMode = CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL;
//  Params.phaVirtualProbe1 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_trapezoid;
//  Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_Input;
//  Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_Peaking;

  uint32_t Mask=0x0;
  FILE *infile;
  infile=fopen(filename,"r");
  char line[1000];
  if (infile==NULL) perror("Error opening config file:");
  int channel=-1;

  Params.nreg=0;
  while (fgets(line,1000,infile)!=NULL){
    char ident[500];
    char var[500];
    sscanf(line,"%s %s",ident,var);
    if (strcmp(ident,"Params.LinkType")==0){
      if (strcmp(var,"CAEN_DGTZ_OpticalLink")==0){
        Params.LinkType = CAEN_DGTZ_OpticalLink;
      }else{
        Params.LinkType = CAEN_DGTZ_USB;
      }
    }
    if (strcmp(ident,"Params.LinkNum")==0) Params.LinkNum=atoi(var);
    if (strcmp(ident,"Params.BoardNum")==0) Params.BoardNum=atoi(var);
    if (strcmp(ident,"Params.VMEBaseAddress")==0) Params.VMEBaseAddress=atoi(var);
    if (strcmp(ident,"Params.IOlev")==0){
      if (strcmp(var,"CAEN_DGTZ_IOLevel_TTL")==0){
        Params.IOlev = CAEN_DGTZ_IOLevel_TTL;
      }else{
        Params.IOlev = CAEN_DGTZ_IOLevel_NIM;
      }
    }
    if (strcmp(ident,"Params.AcqMode")==0){
      if (strcmp(var,"CAEN_DGTZ_DPP_ACQ_MODE_Mixed")==0){
        Params.AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Mixed;
      }else if (strcmp(var,"CAEN_DGTZ_DPP_ACQ_MODE_List")==0){
        Params.AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_List;
      }else{
        Params.AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope;
      }
    }
    if (strcmp(ident,"Params.RecordLength")==0) Params.RecordLength=atoi(var);
    if (strcmp(ident,"Params.EventAggr")==0) Params.EventAggr=atoi(var);
    if (strcmp(ident,"Params.ExtTriggerInputMode")==0){
        if (strcmp(var,"CAEN_DGTZ_TRGMODE_DISABLED")==0){
            Params.ExtTriggerInputMode = CAEN_DGTZ_TRGMODE_DISABLED;
        }else if (strcmp(var,"CAEN_DGTZ_TRGMODE_EXTOUT_ONLY")==0){
            Params.ExtTriggerInputMode = CAEN_DGTZ_TRGMODE_EXTOUT_ONLY;
        }else if (strcmp(var,"CAEN_DGTZ_TRGMODE_ACQ_ONLY")==0){
            Params.ExtTriggerInputMode = CAEN_DGTZ_TRGMODE_ACQ_ONLY;
        }else{
            Params.ExtTriggerInputMode= CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT;
        }
    }
    if (strcmp(ident,"Params.analogProbe1")==0) Params.analogProbes[0]=atoi(var);
    if (strcmp(ident,"Params.analogProbe2")==0) Params.analogProbes[1]=atoi(var);
    if (strcmp(ident,"Params.digitalProbes")==0) Params.digitalProbes=atoi(var);
/*
    if (strcmp(ident,"Params.virtualProbeMode")==0) {
         if (strcmp(var,"CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE")==0){
             Params.virtualProbeMode = CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE;
         }else{
             Params.virtualProbeMode = CAEN_DGTZ_DPP_VIRTUALPROBE_DUAL;
         }
     }
     if (strcmp(ident,"Params.phaVirtualProbe1")==0) {
         if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Input")==0){
             Params.phaVirtualProbe1 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Input;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Delta")==0){
             Params.phaVirtualProbe1 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Delta;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Delta2")==0){
             Params.phaVirtualProbe1 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_Delta2;
         }else{
             Params.phaVirtualProbe1 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE1_trapezoid;
         }
     }
     if (strcmp(ident,"Params.phaVirtualProbe2")==0) {
         if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_Input")==0){
             Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_Input;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_S3")==0){
             Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_S3;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_DigitalCombo")==0){
             Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_DigitalCombo;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_trapBaseline")==0){
             Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_trapBaseline;
         }else{
             Params.phaVirtualProbe2 = CAEN_DGTZ_DPP_PHA_VIRTUALPROBE2_None;
         }
     }

     if (strcmp(ident,"Params.phaDigitalProbe")==0) {
         if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_trgWin")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_trgWin;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_Armed")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_Armed;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_PkRun")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_PkRun;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_PURFlag")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_PURFlag;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_Peaking")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_Peaking;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TVAW")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TVAW;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_BLHoldoff")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_BLHoldoff;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TRGHoldOff")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TRGHoldoff;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TRGVal")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_TRGVal;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_ACQVeto")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_ACQVeto;
         }else if (strcmp(var,"CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_BFMVeto")==0){
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_BFMVeto;
         }else{
             Params.phaDigitalProbe = CAEN_DGTZ_DPP_PHA_DIGITAL_PROBE_ExtTRG;
         }
     }
     */

    char abc[100];
    char xyz[100];
    if (line[0]=='['){
      sscanf(line,"[%s]",abc);
      strncpy(xyz,abc,strlen(abc)-1);
      channel=atoi(xyz);
      //      printf("%d\n",channel);
    }
    if (strcmp(ident,"Params.ChannelEnabled")==0){
      if (strcmp(var,"YES")==0){
          if (channel==-1){for (int i=0;i<MaxNChannels;i++) Mask=Mask|(1<<i);}
          else{Mask=Mask|(1<<channel);}
      }
    }
    if (strcmp(ident,"Params.PulsePolarity")==0){
      if (strcmp(var,"CAEN_DGTZ_PulsePolarityNegative")==0){
          if (channel==-1){for (int i=0;i<MaxNChannels;i++) Params.PulsePolarity[i]= CAEN_DGTZ_PulsePolarityNegative;}
          else{Params.PulsePolarity[channel]= CAEN_DGTZ_PulsePolarityNegative;}
      }else{
          if (channel==-1){for (int i=0;i<MaxNChannels;i++) Params.PulsePolarity[i]= CAEN_DGTZ_PulsePolarityPositive;}
          else{Params.PulsePolarity[channel]= CAEN_DGTZ_PulsePolarityPositive;}
      }
    }
    if (strcmp(ident,"Params.PreTriggerSize")==0){
        if (channel==-1){for (int i=0;i<MaxNChannels;i++) Params.PreTriggerSize[i]=atoi(var);}
        else{Params.PreTriggerSize[channel]=atoi(var);}
    }
    if (strcmp(ident,"Params.DCOffset")==0){
        if (channel==-1){for (int i=0;i<MaxNChannels;i++) Params.DCOffset[i]= (int)((atof(var)+50) * 65535 / 100);}
        else{Params.DCOffset[channel]= (int)((atof(var)+50) * 65535 / 100);}
    }

    char var1[500];
    char var2[500];
    char var3[500];
    sscanf(line,"%s %s %s %s",ident,var1,var2,var3);
    if (strcmp(ident,"GENERIC_WRITE")==0){
        char *p;
        Params.reg_address[Params.nreg]=strtoul(var1, &p, 16);
        Params.reg_data[Params.nreg]=strtoul(var2, &p, 16);
        Params.reg_mask[Params.nreg]=strtoul(var3, &p, 16);
        Params.nreg++;
    }
  }
  Params.ChannelMask = Mask;                               // Channel enable mask
  return Params;
}


CAEN_DGTZ_DPP_PHA_Params_t getDPPPHAParams(char* filename)
{
  CAEN_DGTZ_DPP_PHA_Params_t DPPParams;
  //! set default dpp parms
  for(int ch=0; ch<MaxNChannels; ch++) {
      DPPParams.thr[ch] = 100;   // Trigger Threshold (in LSB)
      DPPParams.k[ch] = 3000;     // Trapezoid Rise Time (ns)
      DPPParams.m[ch] = 900;      // Trapezoid Flat Top  (ns)
      DPPParams.M[ch] = 50000;      // Decay Time Constant (ns)
      DPPParams.ftd[ch] = 500;    // Flat top delay (peaking time) (ns)
      DPPParams.a[ch] = 4;       // Trigger Filter smoothing factor (number of samples to average for RC-CR2 filter) Options: 1; 2; 4; 8; 16; 32
      DPPParams.b[ch] = 200;     // Input Signal Rise time (ns)
      DPPParams.trgho[ch] = 1200;  // Trigger Hold Off
      DPPParams.nsbl[ch] = 4;     //number of samples for baseline average calculation. Options: 1->16 samples; 2->64 samples; 3->256 samples; 4->1024 samples; 5->4096 samples; 6->16384 samples
      DPPParams.nspk[ch] = 0;     //Peak mean (number of samples to average for trapezoid height calculation). Options: 0-> 1 sample; 1->4 samples; 2->16 samples; 3->64 samples
      DPPParams.pkho[ch] = 2000;  //peak holdoff (ns)
      DPPParams.blho[ch] = 500;   //Baseline holdoff (ns)
      DPPParams.enf[ch] = 1.0; // Energy Normalization Factor
      DPPParams.decimation[ch] = 0;  //decimation (the input signal samples are averaged within this number of samples): 0 ->disabled; 1->2 samples; 2->4 samples; 3->8 samples
      DPPParams.dgain[ch] = 0;    //decimation gain. Options: 0->DigitalGain=1; 1->DigitalGain=2 (only with decimation >= 2samples); 2->DigitalGain=4 (only with decimation >= 4samples); 3->DigitalGain=8( only with decimation = 8samples).
      DPPParams.otrej[ch] = 0;
      DPPParams.trgwin[ch] = 0;  //Enable Rise time Discrimination. Options: 0->disabled; 1->enabled
      DPPParams.twwdt[ch] = 100;  //Rise Time Validation Window (ns)
  }
  FILE *infile;
  infile=fopen(filename,"r");
  char line[1000];
  if (infile==NULL) perror("Error opening config file:");
  int channel=-1;
  while (fgets(line,1000,infile)!=NULL){
    char ident[500];
    char var[500];
    sscanf(line,"%s %s",ident,var);

    char abc[100];
    char xyz[100];
    if (line[0]=='['){
      sscanf(line,"[%s]",abc);
      strncpy(xyz,abc,strlen(abc)-1);
      channel=atoi(xyz);
      //      printf("%d\n",channel);
    }
    if (strcmp(ident,"DPPPHA.tpz_risetime_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.k[i]=atoi(var);}else{ DPPParams.k[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_flattop_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.m[i]=atoi(var);}else{ DPPParams.m[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_decaytime_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.M[i]=atoi(var);}else{ DPPParams.M[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_flattopdelay_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.ftd[i]=atoi(var);}else{ DPPParams.ftd[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_nsamplepeak")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.nspk[i]=atoi(var);}else{ DPPParams.nspk[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_peakholdoff_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.pkho[i]=atoi(var);}else{ DPPParams.pkho[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.tpz_energyfinegain")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.enf[i]=atof(var);}else{ DPPParams.enf[channel]=atof(var);}}
    if (strcmp(ident,"DPPPHA.tpz_baselineadjuster")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.trapbsl[i]=atoi(var);}else{ DPPParams.trapbsl[channel]=atoi(var);}}

    if (strcmp(ident,"DPPPHA.trg_threshold")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.thr[i]=atoi(var);}else{ DPPParams.thr[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.trg_holdoff_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.trgho[i]=atoi(var);}else{ DPPParams.trgho[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.trg_smoothfactor")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.a[i]=atoi(var);}else{ DPPParams.a[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.trg_inputrisetime_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.b[i]=atoi(var);}else{ DPPParams.b[channel]=atoi(var);}}

    if (strcmp(ident,"DPPPHA.pileup_zerocrossingacceptwindow")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.twwdt[i]=atoi(var);}else{ DPPParams.twwdt[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.coincidence_trgwindow_ns")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.trgwin[i]=atoi(var);}else{ DPPParams.trgwin[channel]=atoi(var);}}

    if (strcmp(ident,"DPPPHA.inp_nbaselinemean")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.nsbl[i]=atoi(var);}else{ DPPParams.nsbl[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_digitalgain")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.dgain[i]=atoi(var);}else{ DPPParams.dgain[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_decimationfactor")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.decimation[i]=atoi(var);}else{ DPPParams.decimation[channel]=atoi(var);}}

    if (strcmp(ident,"DPPPHA.inp_energyskimming")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.enskim[i]=atoi(var);}else{ DPPParams.enskim[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_energyskimminglld")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.eskimlld[i]=atoi(var);}else{ DPPParams.eskimlld[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_energyskimminguld")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.eskimuld[i]=atoi(var);}else{ DPPParams.eskimuld[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_baselinerestorerclipping")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.blrclip[i]=atoi(var);}else{ DPPParams.blrclip[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.inp_ttfiltercompensation")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.dcomp[i]=atoi(var);}else{ DPPParams.dcomp[channel]=atoi(var);}}

    if (strcmp(ident,"DPPPHA.depr_blho")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.blho[i]=atoi(var);}else{ DPPParams.blho[channel]=atoi(var);}}
    if (strcmp(ident,"DPPPHA.depr_otrej")==0) {if (channel==-1){for (int i=0;i<MaxNChannels;i++) DPPParams.otrej[i]=atoi(var);}else{ DPPParams.otrej[channel]=atoi(var);}}
  }
  return DPPParams;
}

int CheckBoardFailureStatus2(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo) {

    int ret = 0;
    uint32_t status = 0;
    ret = CAEN_DGTZ_ReadRegister(handle, 0x8104, &status);
    if (ret != 0) {
            printf("Error: Unable to read board failure status.\n");
            return -1;
    }
#ifdef _WIN32
    Sleep(200);
#else
    usleep(200000);
#endif
    //read twice (first read clears the previous status)
    ret = CAEN_DGTZ_ReadRegister(handle, 0x8104, &status);
    if (ret != 0) {
            printf("Error: Unable to read board failure status.\n");
            return -1;
    }

    if(!(status & (1 << 7))) {
            printf("Board error detected: PLL not locked.\n");
            return -1;
    }

    return 0;
}

int ResetDigitizer(int handle)
{
    int ret = 0;
    /* Reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);

    if (ret) {
      printf("ERROR: can't reset the digitizer.\n");
      return -1;
    }
}
int WriteRegisterBitmask2(int32_t handle, uint32_t address, uint32_t data, uint32_t mask) {
    int32_t ret = CAEN_DGTZ_Success;
    uint32_t d32 = 0xFFFFFFFF;

    ret = CAEN_DGTZ_ReadRegister(handle, address, &d32);
    if(ret != CAEN_DGTZ_Success)
        return ret;

    data &= mask;
    d32 &= ~mask;
    d32 |= data;
    ret = CAEN_DGTZ_WriteRegister(handle, address, d32);
    return ret;
}
int ProgramDigitizerDPPPHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams)
{

  /* This function uses the CAENDigitizer API functions to perform the digitizer's initial configuration */
  int i, ret = 0;



  ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);

  // Set the digitizer acquisition mode (CAEN_DGTZ_SW_CONTROLLED or CAEN_DGTZ_S_IN_CONTROLLED)
  ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);

  // Set the I/O level (CAEN_DGTZ_IOLevel_NIM or CAEN_DGTZ_IOLevel_TTL)
  ret |= CAEN_DGTZ_SetIOLevel(handle, Params.IOlev);

  // Set the digitizer's behaviour when an external trigger arrives:

  ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle,Params.ExtTriggerInputMode);

  // Set the enabled channels
  ret |= CAEN_DGTZ_SetChannelEnableMask(handle, Params.ChannelMask);

  // Set how many events to accumulate in the board memory before being available for readout
  ret |= CAEN_DGTZ_SetDPPEventAggregation(handle, Params.EventAggr, 0);

  /* Set the mode used to syncronize the acquisition between different boards.
     In this example the sync is disabled */
  ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);

  // Set the DPP specific parameters for the channels in the given channelMask
  ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);

  // Set the number of samples for each waveform
  ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength);
  //ret |= CAEN_DGTZ_SetRecordLength(handle, 3000,1);
  //ret |= CAEN_DGTZ_SetDPPTriggerMode(handle,CAEN_DGTZ_DPP_TriggerMode_Coincidence);

  ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle,ANALOG_TRACE_1,Params.analogProbes[0]);
  ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle,ANALOG_TRACE_2,Params.analogProbes[1]);
  ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle,DIGITAL_TRACE_1,Params.digitalProbes);

  //ret |= CAEN_DGTZ_SetDPP_PHA_VirtualProbe(handle,Params.virtualProbeMode,Params.phaVirtualProbe1,Params.phaVirtualProbe2,Params.phaDigitalProbe);

  for(i=0; i<MaxNChannels; i++) {
    if (Params.ChannelMask & (1<<i)) {
      // Set a DC offset to the input signal to adapt it to digitizer's dynamic range
      ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, Params.DCOffset[i]);
      // Set the Pre-Trigger size (in samples)
      ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, Params.PreTriggerSize[i]);

      // Set the polarity for the given channel
      ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity[i]);
    }
  }

  //! Write registers
  for (uint32_t ii=0;ii<Params.nreg;ii++){
       ret|=WriteRegisterBitmask2(handle,Params.reg_address[ii],Params.reg_data[ii],Params.reg_mask[ii]);
  }


  if (ret) {
    printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
    return ret;
  } else {
    return 0;
  }

}

