
#include "daq.h"

long daq::get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

void daq::WriteHeader(int isHeader){
    strcpy(header->comment,shmp->com_comments);
    header->nboard=shmp->nboard;
    header->MaxNChannels_t=(int)MaxNChannels;
    header->MAX_BLOCK_t=(int)MAX_BLOCK;
    header->WF_BLOCK_LENGTH_t=(int)WF_BLOCK_LENGTH;
    header->runno=shmp->runno;
    header->DGTZ_BLOCK_LENGTH_t=(int)DGTZ_BLOCK_LENGTH;
    if (isHeader==1)
        header->start_stop_time=shmp->start_time;
    else
        header->start_stop_time=shmp->stop_time;

    for(int b=0; b<shmp->nboard; b++) {
    header->channelMask[b]=dgtz_parms[b].ChannelMask;
    header->recordLength[b]=dgtz_parms[b].RecordLength;
        for (int ch=0;ch<MaxNChannels;ch++){
            if (!(header->channelMask[b] & (1<<ch))) continue;
            header->longGate[b][ch]=dpp_parms[b].lgate[ch];
            header->shortGate[b][ch]=dpp_parms[b].sgate[ch];
            header->preGate[b][ch]=dpp_parms[b].pgate[ch];
            header->preTrg[b][ch]=dgtz_parms[b].PreTriggerSize[ch];
            header->pulsePolarity[b][ch]=(uint8_t)dgtz_parms[b].PulsePolarity[ch];

            if (isHeader==0)
                header->TrgCnt[b][ch]=TrgCnt_e[b][ch];
        }
    }
    size_t ss;
    if (write_flag==1) ss=fwrite(header,sizeof(header_type),1,shmp->raw_fd);
    if (ss<1) printf("Error wniting header!\n");
    //printf("WRITE successfull %i-%i\n",(int)ss,sizeof(header_type));
}

CAEN_DGTZ_DPP_PSD_Params_t daq::getDPPParams(char* filename)
{
  CAEN_DGTZ_DPP_PSD_Params_t DPPParams;
  FILE *infile;
  infile=fopen(filename,"r");
  char line[1000];
  if (infile==NULL) perror("Error opening config file:");
  int channel=-1;  
  while (fgets(line,1000,infile)!=NULL){
    strcpy(header->inputFileContent[nline],line);
    nline++;
    char ident[500];
    char var[500];
    sscanf(line,"%s %s",ident,var);
    if (strcmp(ident,"DPPParams.purgap")==0) DPPParams.purgap=atoi(var);
    if (strcmp(ident,"DPPParams.blthr")==0) DPPParams.blthr=atoi(var);
    if (strcmp(ident,"DPPParams.bltmo")==0) DPPParams.bltmo=atoi(var);
    if (strcmp(ident,"DPPParams.trgho")==0) DPPParams.trgho=atoi(var);
    if (strcmp(ident,"DPPParams.purh")==0){
      if (strcmp(var,"CAEN_DGTZ_DPP_PSD_PUR_DetectOnly")==0){
    DPPParams.purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly;
      }else{
    DPPParams.purh = CAEN_DGTZ_DPP_PSD_PUR_Enabled;
      }
    }

    char abc[100];
    //    char xyz[100];
    if (line[0]=='['){
      sscanf(line,"[ %s ]",abc);
      //strncpy(xyz,abc,strlen(abc)-1);
      channel=atoi(abc);
    }
    if (strcmp(ident,"DPPParams.thr[ch]")==0) DPPParams.thr[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.nsbl[ch]")==0) DPPParams.nsbl[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.lgate[ch]")==0) DPPParams.lgate[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.sgate[ch]")==0) DPPParams.sgate[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.pgate[ch]")==0) DPPParams.pgate[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.selft[ch]")==0) {
      DPPParams.selft[channel]=atoi(var);
    }
    if (strcmp(ident,"DPPParams.tvaw[ch]")==0) DPPParams.tvaw[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.csens[ch]")==0) DPPParams.csens[channel]=atoi(var);
    if (strcmp(ident,"DPPParams.trgc[ch]")==0){
      if (strcmp(var,"CAEN_DGTZ_DPP_TriggerConfig_Threshold")==0){
    DPPParams.trgc[channel] = CAEN_DGTZ_DPP_TriggerConfig_Threshold;
      }else{
    DPPParams.trgc[channel] = CAEN_DGTZ_DPP_TriggerConfig_Peak;
      }
    }
  }
  header->inputFileContent_Length=nline;
  /*
   int ch;
        for(ch=0; ch<MaxNChannels; ch++) {
            DPPParams.thr[ch] = 500;        // Trigger Threshold
            DPPParams.nsbl[ch] = 2;
            DPPParams.lgate[ch] = 32;    // Long Gate Width (N*4ns)
            DPPParams.sgate[ch] = 24;    // Short Gate Width (N*4ns)
            DPPParams.pgate[ch] = 8;     // Pre Gate Width (N*4ns)
            DPPParams.selft[ch] = 1;
            DPPParams.trgc[ch] = CAEN_DGTZ_DPP_TriggerConfig_Threshold;
            DPPParams.tvaw[ch] = 50;
            DPPParams.csens[ch] = 0;
        }
        DPPParams.purh = CAEN_DGTZ_DPP_PSD_PUR_DetectOnly;
        DPPParams.purgap = 100;  // Purity Gap
        DPPParams.blthr = 3;     // Baseline Threshold
        DPPParams.bltmo = 100;   // Baseline Timeout
        DPPParams.trgho = 8;     // Trigger HoldOff
  */
    return DPPParams;
}
DigitizerParams_t daq::getDigitizerParams(char* filename)
{
  DigitizerParams_t Params;
  uint32_t Mask=0x0;
  FILE *infile;
  infile=fopen(filename,"r");
  char line[1000];
  if (infile==NULL) perror("Error opening config file:");
  int channel=-1;
  while (fgets(line,1000,infile)!=NULL){
    char ident[500];
    char var[500];
    sscanf(line,"%s %s",ident,var);    
    if (strcmp(ident,"SAVE_DIR")==0) strcpy(Params.saveDir,var);
    if (strcmp(ident,"SAVE_NAME")==0) strcpy(Params.saveName,var);
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
    char abc[100];
    char xyz[100];
    if (line[0]=='['){
      sscanf(line,"[%s]",abc);
      strncpy(xyz,abc,strlen(abc)-1);
      channel=atoi(xyz);
      //      printf("%d\n",channel);
    }
    if (strcmp(ident,"Params.ChannelEnabled[ch]")==0){
      if (strcmp(var,"YES")==0){
    Mask=Mask|(1<<channel);
      }
    }
    if (strcmp(ident,"Params.PulsePolarity[ch]")==0){
      if (strcmp(var,"CAEN_DGTZ_PulsePolarityNegative")==0){
    Params.PulsePolarity[channel]= CAEN_DGTZ_PulsePolarityNegative;
      }else{
    Params.PulsePolarity[channel]= CAEN_DGTZ_PulsePolarityPositive;
      }
    }
    if (strcmp(ident,"Params.PreTriggerSize[ch]")==0)  Params.PreTriggerSize[channel]=atoi(var);
    if (strcmp(ident,"Params.DCOffset[ch]")==0){
      Params.DCOffset[channel]= (int)((atof(var)+50) * 65535 / 100);
    }
  }

  Params.ChannelMask = Mask;                               // Channel enable mask
  return Params;
}



int daq::WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask) {
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

/* ###########################################################################
 *  Functions
 *  ########################################################################### */

int daq::ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams,char* filename)
{

  /* This function uses the CAENDigitizer API functions to perform the digitizer's initial configuration */
  int i, ret = 0;

  /* Reset the digitizer */
  ret |= CAEN_DGTZ_Reset(handle);

  if (ret) {
    printf("ERROR: can't reset the digitizer.\n");
    return -1;
  }

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
  //Write Register
  FILE *infile;
  infile=fopen(filename,"r");
  char line[1000];
  uint32_t address,data,mask;
  while (fgets(line,1000,infile)!=NULL){
    char ident[500];
    char var1[500];
    char var2[500];
    char var3[500];
    sscanf(line,"%s %s %s %s",ident,var1,var2,var3);
    if (strcmp(ident,"GENERIC_WRITE")==0){
      char *p;
      address=strtoul(var1, &p, 16);
      data=strtoul(var2, &p, 16);
      mask=strtoul(var3, &p, 16);
      ret|=WriteRegisterBitmask(handle,address,data,mask);
    }
  }

  if (ret) {
    printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");
    return ret;
  } else {
    return 0;
  }
}

void daq::daqinit(int nboard_t,char config_file_name_t[MAX_NBOARD][500])
{
    nline=0;
    header=new header_type_t;
    //create share memory
    if ((shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), 0600)) != -1)
    {
      shmp = (struct SHM_DGTZ_S *)shmat(shmid, 0, 0);

      shmp->runno=0; shmp->status=0;shmp->raw_fd=NULL;
      shmp->block_no=0;
      shmp->raw_filename[0]=0;
      shmp->start_time=0;  shmp->stop_time=0;
      for(int i=0;i<MAX_BLOCK;i++)
      {
        shmp->buffer[i].status = BLOCK_WAITING;
        shmp->buffer[i].blockno = 0;
      }
      shmp->com_flag=0;
      shmp->com_time=0;
      shmp->com_filename[0]=0;
      shmp->pid=0;
      shmp->ana_flag=0;
      shmp->ana_message[0]=0;
      shmp->ana_status=0;

      printf("Sucessfully attached to share memory with shmid=%i, size of %i\n-----------------\n",shmid,(int)sizeof(struct SHM_DGTZ_S));
    }
    else if ((shmid = shmget(SHMKEY, sizeof(struct SHM_DGTZ_S), IPC_CREAT|0600)) != -1)
    {
      shmp = (struct SHM_DGTZ_S *)shmat(shmid, 0, 0);

      //for(int i=0;i<BLOCK_LENGTH*2;i++)header[i]=0;
      shmp->runno=0; shmp->status=0;shmp->raw_fd=NULL;
      shmp->block_no=0;
      shmp->raw_filename[0]=0;
      shmp->start_time=0;  shmp->stop_time=0;
      for(int i=0;i<MAX_BLOCK;i++)
      {
        shmp->buffer[i].status = BLOCK_WAITING;
        shmp->buffer[i].blockno = 0;
      }
      shmp->com_flag=0;
      shmp->com_time=0;
      shmp->com_filename[0]=0;
      shmp->pid=0;      
      shmp->ana_flag=0;
      shmp->ana_message[0]=0;
      shmp->ana_status=0;

      printf("Sucessfully created share memory with shmid=%i, size of %i\n-----------------\n",shmid,(int)sizeof(struct SHM_DGTZ_S));
    }
    else
    {
      printf("DMA_CONT: Cannot create shared memory");
      exit(2);
    }
    shmp->pid=getpid();
    shmp->com_flag = 0;
    shmp->status = 0;

    for (int i=0;i<nboard_t;i++){
        strcpy(shmp->config_file_name[i],config_file_name_t[i]);
    }
    shmp->nboard=nboard_t;
    dgtz_parms=new DigitizerParams_t[nboard_t];
    dpp_parms=new CAEN_DGTZ_DPP_PSD_Params_t[nboard_t];
    handle=new int[nboard_t];
    ret=0;
    resetScaler();

}
void daq::resetScaler(){
    Nb=0;
    for (int i=0;i<shmp->nboard;i++){
        for (int j=0;j<MaxNChannels;j++){
            shmp->TrgCnt[i][j]=0;
            TrgCnt_e[i][j]=0;
        }
    }

    //Clear buffer
    /*
    for(int i=0;i<MAX_BLOCK;i++)
    {
        shmp->buffer[i].status = 0;
        shmp->buffer[i].blockno = 0;
        memset(shmp->buffer[i].data, 0, sizeof(uint32_t)*BLOCK_LENGTH );
    }
    */

}

void daq::connect()
{
    //load parms
    for(int b=0; b<shmp->nboard; b++) {
      dgtz_parms[b]=getDigitizerParams(shmp->config_file_name[b]);
      dpp_parms[b]=getDPPParams(shmp->config_file_name[b]);
      strcpy(subFilename,dgtz_parms[b].saveName);
    }

    //OPEN digitizer, get handle
    for(int b=0; b<shmp->nboard; b++) {
      ret = CAEN_DGTZ_OpenDigitizer(dgtz_parms[b].LinkType, dgtz_parms[b].LinkNum, dgtz_parms[b].BoardNum, dgtz_parms[b].VMEBaseAddress, &handle[b]);
      if (ret) {
        printf("Can not open digitizer!\n");
      }
      /* Once we have the handler to the digitizer, we use it to call the other functions */
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
      if (ret) {
        printf("Can't read board info\n");
        QuitProgram();
      }
     printf("Connected to CAEN Digitizer Model %s as board %i \nROC FPGA Release is %s\nAMC FPGA Release is %s\n ------------\n",
            BoardInfo.ModelName, b,BoardInfo.ROC_FirmwareRel,BoardInfo.AMC_FirmwareRel);

      // Check firmware revision (only DPP firmware can be used with this Demo) */
     int MajorNumber;
      sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
      if (MajorNumber != 131 && MajorNumber != 132&& MajorNumber != 136 ) {
        printf("This digitizer has not a DPP-PSD firmware\n");
      }
    }

    /* *************************************************************************************** */
    /* Program the digitizer (see function ProgramDigitizer)                                   */
    /* *************************************************************************************** */
    for(int b=0; b<shmp->nboard; b++) {
      ret = ProgramDigitizer(handle[b], dgtz_parms[b], dpp_parms[b],shmp->config_file_name[b]);
      if (ret) {
          printf("Failed to program the digitizer\n");
      }
    }
    /* WARNING: The mallocs MUST be done after the digitizer programming,
       because the following functions needs to know the digitizer configuration
       to allocate the right memory amount */
    /* Allocate memory for the readout buffer */
    ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[0], &sbuffer_t, &sAllocatedSize);
    /* Allocate memory for the events */
    ret |= CAEN_DGTZ_MallocDPPEvents(handle[0], sEvents_t, &sAllocatedSize);
    /* Allocate memory for the waveforms */
    ret |= CAEN_DGTZ_MallocDPPWaveforms(handle[0], &sWaveform_t, &sAllocatedSize);
    if (ret) {
        printf("Can't allocate memory buffers\n");
    }

    //set shmp DPP info

    for(int b=0; b<shmp->nboard; b++) {
        shmp->recordLength[b]=dgtz_parms[b].RecordLength;
        memcpy(shmp->preGate[b],dpp_parms[b].pgate,MaxNChannels*sizeof(int));
        memcpy(shmp->shortGate[b],dpp_parms[b].sgate,MaxNChannels*sizeof(int));
        memcpy(shmp->longGate[b],dpp_parms[b].lgate,MaxNChannels*sizeof(int));
        memcpy(shmp->preTrg[b],dgtz_parms[b].PreTriggerSize,MaxNChannels*sizeof(uint32_t));

        shmp->channelMask[b]=dgtz_parms[b].ChannelMask;
    }
    sprintf(shmp->raw_projectName,"%s",dgtz_parms[0].saveName);
    sprintf(shmp->raw_saveDir,"%s",dgtz_parms[0].saveDir);

    //set file name

}
void daq::QuitProgram(){
    for(int b=0; b<shmp->nboard; b++) {
      CAEN_DGTZ_SWStopAcquisition(handle[b]);
      CAEN_DGTZ_CloseDigitizer(handle[b]);
      //MUST CLOSE FILES
    }
    CAEN_DGTZ_FreeReadoutBuffer(&sbuffer_t);
    CAEN_DGTZ_FreeDPPEvents(handle[0], sEvents_t);
    CAEN_DGTZ_FreeDPPWaveforms(handle[0], sWaveform_t);
    printf("Disconnected!");
}
int temp=0;



void daq::daqloop(){
    write_flag=0;
    daq::connect();
    //shmp->runno;
    printf("Idle....\n");

    //enter readout loop
    while(1){//1st while
        if (shmp->com_flag==1){//1st if_ if (shmp->com_flag==1)
              if(strncmp(shmp->com_message,"open",3)==0){
                    sprintf(shmp->raw_filename,"%srun%i_%s.nig",shmp->raw_saveDir,shmp->runno,shmp->raw_projectName);
                    printf("OPEN FILE: %s\n",shmp->raw_filename);
                    shmp->raw_fd = fopen(shmp->raw_filename, "w+");                                        
                    if(shmp->raw_fd==NULL)
                    {
                      perror("open data file error!");
                      shmp->raw_fd=NULL;
                    }else
                    {
                      //Clear
                      for(int i=0;i<MAX_BLOCK;i++)
                      {
                          shmp->buffer[i].status = 0;
                          shmp->buffer[i].blockno = 0;
                      }

                          shmp->block_no = 0;
                      printf("Openned file named %s\n",shmp->raw_filename);
                      write_flag=1;
                    }
                    shmp->com_flag = 0;
                }else if(strncmp(shmp->com_message,"close",3)==0){
                  //WRITE footer here
                  WriteHeader(0);
                  //==================
                    fclose(shmp->raw_fd);
                    shmp->raw_fd = NULL;
                    //chmod(shmp->raw_filename,0440);
                    shmp->runno++;
                    printf("Closed files!\n");
                    shmp->com_flag = 0;
                    write_flag=0;
                }else if(strncmp(shmp->com_message,"start",3)==0){//2nd if - enter readout loop                
                resetScaler();
                for(int b=0; b<shmp->nboard; b++) {
                    CAEN_DGTZ_SWStartAcquisition(handle[b]);
                }
                shmp->start_time=time(0);//get current time
                shmp->com_flag = 0;
                shmp->status = 1;
                shmp->block_no=0;

                printf("Acquisition started!\n");
                if (shmp->raw_fd!=NULL) WriteHeader(1);

                PrevRateTime = get_time();
                while(1){//2nd while, Enter the Readout Cycle..........
                    //TIME OPERATION
                    /* Calculate throughput and trigger rate (every second) */
                    CurrentTime = get_time();
                    ElapsedTime = CurrentTime - PrevRateTime; /* milliseconds */
                    if (ElapsedTime > 1000) {
                      system("clear");

                      struct tm * start_timeinfo;
                      start_timeinfo = localtime ( &shmp->start_time );
                      printf("Start time:%s",asctime(start_timeinfo));

                      struct tm * current_timeinfo;
                      time_t current_time= time(0);
                      current_timeinfo = localtime ( &current_time);
                      printf("Current time:%s\n",asctime(current_timeinfo));

                      printf("Readout Rate=%.2f MB\n", (float)Nb/((float)ElapsedTime*1048.576f));
                      //if (temps==1) printf("OKKKKK\n");
                      for(int b=0; b<shmp->nboard; b++) {
                          printf("\nBoard %d:\n",b);
                          for(int i=0; i<MaxNChannels; i++) {
                              if (!(dgtz_parms[b].ChannelMask & (1<<i))) printf("[INACTIVE] "); else printf("[ACTIVE] ");
                              if (TrgCnt_rate[b][i]>0)
                                  printf("\tCh %d:\tTrgRate=%.2f KHz\t%", b*8+i, (float)TrgCnt_rate[b][i]/(float)ElapsedTime);
                              else
                                  printf("\tCh %d:\tNo Data", i);
                              //write scaler here
                              printf(" |\t %i events received\n",TrgCnt_e[b][i]);
                              TrgCnt_rate[b][i]=0;
                          }
                      }
                      Nb = 0;
                      PrevRateTime = CurrentTime;                      
                    }

                    //------------end of time operation---------------

                   for(int b=0; b<shmp->nboard; b++) {
                       // Read data from the board
                       ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, sbuffer_t,&sBufferSize);
                       if (ret) {
                           printf("Readout Error\n");
                           QuitProgram();
                       }
                       if (sBufferSize == 0) continue;
                       Nb += sBufferSize; //cout size of buffer
                       //printf("Nb=%i\n",Nb);
                       ret |= CAEN_DGTZ_GetDPPEvents(handle[b], sbuffer_t, sBufferSize, sEvents_t, NumEvents_t);
                       Events_t= (CAEN_DGTZ_DPP_PSD_Event_t**) (sEvents_t);

                       if (ret) {
                        printf("Data Error: %i\n",ret);
                        QuitProgram();
                       }

                       for(int ch=0; ch<MaxNChannels; ch++) {
                           if (!(dgtz_parms[b].ChannelMask & (1<<ch))) continue;
                           for(unsigned int ev=0; ev<NumEvents_t[ch]; ev++){

                                  if (shmp->block_no>(uint32_t)MAX_BLOCK-1) {
                                      //WRITE DATA HERE!!!!
                                      size_t s;
                                      /*
                                      printf("--------------------\n");
                                      for (int tt=0;tt<MAX_BLOCK;tt++){
                                          printf("ch no= %i--- block no = %i--- trgn= %i\n",shmp->buffer[tt].dgtzdata[1],shmp->block_no,shmp->buffer[tt].dgtzdata[2]);
                                      }
                                      */
                                      if (write_flag==1) s=fwrite(shmp->buffer,sizeof(buffer_type),(size_t)MAX_BLOCK,shmp->raw_fd);
                                      if (s<sizeof(MAX_BLOCK))
                                      {
                                          printf("WRITE FILE ERROR\n");
                                          strcpy(shmp->com_message,"stop");
                                          goto end_run;
                                      }
                                      //clear buffer here
                                      memset(shmp->buffer,0,MAX_BLOCK*sizeof(buffer_type));

                                      shmp->block_no=0;
                                  }

                               shmp->TrgCnt[b][ch]=(uint32_t)TrgCnt_e[b][ch];

                               shmp->ana_flag=1;

                               shmp->buffer[shmp->block_no].blockno=shmp->block_no;
                               shmp->buffer[shmp->block_no].dgtzdata[0] = (uint32_t)b;//Board number
                               shmp->buffer[shmp->block_no].dgtzdata[1] = (uint32_t)ch;//Channel number
                               shmp->buffer[shmp->block_no].dgtzdata[2] = (uint32_t)TrgCnt_e[b][ch];//event Number
                               shmp->buffer[shmp->block_no].dgtzdata[3] = Events_t[ch][ev].TimeTag;//TTT LSB
                               shmp->buffer[shmp->block_no].dgtzdata[4] = Events_t[ch][ev].Extras; //16bit TTT MSB + 16bit baseline value multiply by 4
                               if (Events_t[ch][ev].Baseline<0) Events_t[ch][ev].Baseline=0;
                               if (Events_t[ch][ev].ChargeLong<0) Events_t[ch][ev].ChargeLong=0;
                               if (Events_t[ch][ev].ChargeShort<0) Events_t[ch][ev].ChargeShort=0;
                               shmp->buffer[shmp->block_no].dgtzdata[5] = (uint32_t)Events_t[ch][ev].Baseline;                               
                               shmp->buffer[shmp->block_no].dgtzdata[6] = (uint32_t)Events_t[ch][ev].ChargeLong;
                               shmp->buffer[shmp->block_no].dgtzdata[7] = (uint32_t)Events_t[ch][ev].ChargeShort;
                               shmp->buffer[shmp->block_no].dgtzdata[8] = (uint32_t)Events_t[ch][ev].Pur;
                               shmp->buffer[shmp->block_no].dgtzdata[9] = (uint32_t)Events_t[ch][ev].Format;
                               shmp->buffer[shmp->block_no].dgtzdata[10] = 0;//bit 10 contain sample length, default 0

                               //copy to share memory-> cost computing time->ignore
                               //memcpy(shmp->buffer[shmp->block_no].dgtzdata,BinHeader,11*sizeof(uint32_t));

                               //general dpp information:
                               uint16_t *WaveLine;
                               CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &Events_t[ch][ev], sWaveform_t); //decode wf cost computing time
                               Waveform_t = (CAEN_DGTZ_DPP_PSD_Waveforms_t*)(sWaveform_t);
                               // Use waveform data here...
                               WaveLine = Waveform_t->Trace1; // Input Signal
                               shmp->buffer[shmp->block_no].dgtzdata[10]=Waveform_t->Ns;
                               size_t save_Length;
                               if (Waveform_t->Ns>WF_BLOCK_LENGTH) save_Length=WF_BLOCK_LENGTH; else save_Length=Waveform_t->Ns;

                               memcpy(&shmp->buffer[shmp->block_no].wfdata,WaveLine,save_Length*sizeof(uint16_t));


                               //----------------start DPP processing----------
                               //----------------No cfd infomation on data stream----------
                               CFD_delay=1;//in Tclk unit, 1 Tclk=2ns
                               CFD_fraction=0.5;
                               LED_threshold=20;
                               LED_threshold_LED=20;
                               gateOffset=dpp_parms[b].pgate[ch];
                               shortGate=dpp_parms[b].sgate[ch];
                               longGate=dpp_parms[b].lgate[ch];
                               nBaseline=16;
                               minVarBaseline=100; //criteria for baseline determination
                               mode_selection=4;
                               uint32_t emptyFlag=0;
                               //Check if theshold excess some value;                               
                               dpp *oj=new dpp((int)Waveform_t->Ns,WaveLine);
                               oj->baselineMean(nBaseline,minVarBaseline);
                               //pur rejection for negative signal
                               emptyFlag=(uint32_t)oj->emptyFlag(LED_threshold_LED);
                               if (emptyFlag==0){
                                 double timeData[2];
                                 int cShort,cLong;
                                 if (mode_selection==0) {
                                     oj->makeCFD(LED_threshold,CFD_delay,CFD_fraction);
                                     timeData[0]=oj->ledWithCorr(LED_threshold);
                                     //timeData[0]=oj->led(LED_threshold_LED);
                                     oj->chargeInter(cShort,cLong,timeData[0],gateOffset,shortGate,longGate);
                                     shmp->buffer[shmp->block_no].time_data[0]=timeData[0];
                                 }else if (mode_selection==1) {
                                     oj->makeCFD(LED_threshold,CFD_delay,CFD_fraction);
                                     timeData[1]=oj->cfdFast();
                                     oj->chargeInter(cShort,cLong,timeData[1],gateOffset,shortGate,longGate);
                                     shmp->buffer[shmp->block_no].time_data[1]=timeData[1];
                                 }else if (mode_selection==2){
                                     oj->makeCFD(LED_threshold,CFD_delay,CFD_fraction);
                                     timeData[1]=oj->cfdSlow();
                                     oj->chargeInter(cShort,cLong,timeData[1],gateOffset,shortGate,longGate);
                                     shmp->buffer[shmp->block_no].time_data[1]=timeData[1];
                                 }else if (mode_selection==3){
                                     oj->makeCFD(LED_threshold,CFD_delay,CFD_fraction);
                                     //timeData[0]=oj->fineLED;
                                     timeData[0]=oj->ledWithCorr(LED_threshold);
                                     timeData[1]=oj->cfdFast();
                                     oj->chargeInter(cShort,cLong,timeData[1],gateOffset,shortGate,longGate);
                                     shmp->buffer[shmp->block_no].time_data[0]=timeData[0];
                                     shmp->buffer[shmp->block_no].time_data[1]=timeData[1];
                                 }else if (mode_selection==4){
                                     oj->makeCFD(LED_threshold,CFD_delay,CFD_fraction);
                                     //timeData[0]=oj->fineLED;
                                     timeData[0]=oj->ledWithCorr(LED_threshold);
                                     timeData[1]=oj->cfdSlow();
                                     oj->chargeInter(cShort,cLong,timeData[1],gateOffset,shortGate,longGate);
                                     shmp->buffer[shmp->block_no].time_data[0]=timeData[0];
                                     shmp->buffer[shmp->block_no].time_data[1]=timeData[1];
                                 }
                                 int baseline=oj->bL;
                                 if (baseline<0) baseline=0;
                                 if (cShort<0) cShort=0;
                                 if (cLong<0) cLong=0;
                                 shmp->buffer[shmp->block_no].dgtzdata[5] = (uint32_t)baseline;
                                 shmp->buffer[shmp->block_no].dgtzdata[6] = (uint32_t)cLong;
                                 shmp->buffer[shmp->block_no].dgtzdata[7] = (uint32_t)cShort;
                               }
                               delete oj;

/*
                               int tg=shmp->block_no;
                             printf("AAAch no= %i--- block no = %i--- trgn= %i\n",shmp->buffer[tg].dgtzdata[1],shmp->block_no,shmp->buffer[tg].dgtzdata[2]);
*/
                               TrgCnt_e[b][ch]++;
                               TrgCnt_rate[b][ch]++;
                               shmp->block_no++;
                           } // loop on events
                       } // loop on channels
                    }// loop on Board
                   //check stop command here
                  end_run:
                  if(strncmp(shmp->com_message,"stop",3)==0)
                  {
                      for(int b=0; b<shmp->nboard; b++) {
                          // Stop Acquisition
                          CAEN_DGTZ_SWStopAcquisition(handle[b]);
                      }                                            

                      shmp->status = 0;
                      shmp->stop_time=time(0);
                      struct tm * stop_timeinfo;
                      stop_timeinfo = localtime (&shmp->stop_time);
                      printf("Acquisition Stopped at %s\n",asctime(stop_timeinfo));
                      for (int b=0;b<shmp->nboard;b++){
                          printf("Board No %i\n",b);
                          for (int ch=0;ch<MaxNChannels;ch++){
                              if (!(shmp->channelMask[b] & (1<<ch))) continue;
                              printf("          Active channel %i received %i events\n",ch,TrgCnt_e[b][ch]);
                          }
                      }
                      shmp->com_flag = 0;
                      break;
                  }
                }//2nd while
            }//2nd if - else if(strncmp(shmp->com_message,"start",3)==0)
        }else{
            usleep(10000);
        }//1st if_ if (shmp->com_flag==1)
    }//1st while

}

