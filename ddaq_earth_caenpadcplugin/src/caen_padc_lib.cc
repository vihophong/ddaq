#include "caen_padc_lib.h"

//!* *************************************************************************************** */
//!* caen_padc lib
//!* *************************************************************************************** */


int OpenPeakSensing(int *handle, PeakSensingConfig_t ConfigVar)
{
    int board;
    for (board = 0; board < ConfigVar.Nhandle; board++) {
        printf("Board %d: ", board);
        printf("link ID: %d;", ConfigVar.BoardConfigVar[board]->LinkNum);
        printf(" Node ID: %d;", ConfigVar.BoardConfigVar[board]->ConetNode);
        printf(" Base Address: %x\n", ConfigVar.BoardConfigVar[board]->BaseAddress);
        if (CAEN_PADC_OpenPeakSensing(ConfigVar.BoardConfigVar[board]->LinkType, ConfigVar.BoardConfigVar[board]->LinkNum, ConfigVar.BoardConfigVar[board]->ConetNode, ConfigVar.BoardConfigVar[board]->BaseAddress, handle + board)!=CAEN_PADC_Success) {
        //if (CAEN_PADC_OpenPeakSensing(CAEN_PADC_OpticalLink, 3,1,0, &handllee)!=CAEN_PADC_Success) {
            printf("Fail to open PADC module!!!\n");
            return 1;
        }
    }
    return 0;
}

int ProgramPeakSensing(int handle, PeakSensingBoardConfig_t *BoardConfigVar) {
    int group;
    int ret = CAEN_PADC_Success;
    // reset
    ret |= CAEN_PADC_Reset(handle);
    // Front panel I/O level
    ret |= CAEN_PADC_SetIOLevel(handle, BoardConfigVar->FPIOtype);
    ret |= CAEN_PADC_SetAcquisitionMode(handle, CAEN_PADC_SW_CONTROLLED);
    ret |= CAEN_PADC_SetSWTriggerMode(handle,CAEN_PADC_TRGMODE_ACQ_ONLY);
    ret |= CAEN_PADC_SetExtTriggerInputMode(handle, CAEN_PADC_TRGMODE_ACQ_ONLY);
    // Record length: if the "gate" mode is set, the r.l. must be set as small as possible in order not to lose triggers
    ret |= BoardConfigVar->TrigMode? CAEN_PADC_SetRecordLength(handle, 1) : CAEN_PADC_SetRecordLength(handle, BoardConfigVar->RecordLength);
    // set DAC mode (sliding scale or fixed)
    ret |= BoardConfigVar->SlScaleEnable? CAEN_PADC_SlScaleEnable(handle) : CAEN_PADC_SlScaleDisable(handle);
    // set Trigger mode (edge+register value or gate)
    ret |= CAEN_PADC_SetTriggerMode(handle,BoardConfigVar->TrigMode);
    // set Channel mode (bin resolution)
    ret |= CAEN_PADC_SetChMode(handle, BoardConfigVar->ChMode);
    // enable zero suppression
    ret |= BoardConfigVar->ZSEnable? CAEN_PADC_ZSEnable(handle) : CAEN_PADC_ZSDisable(handle);
    // set polarity
    ret |= CAEN_PADC_SetPolarity(handle, BoardConfigVar->Polarity);
    // set input range (0: 8V; 1: 4V)
    ret |= CAEN_PADC_SetInputRange(handle, BoardConfigVar->InputRange);
    // set group-dependent variables
    ret |= CAEN_PADC_GetMaxChannels(handle,&BoardConfigVar->ChMax);
    for (group = 0; group < BoardConfigVar->ChMax/8; group++) {
        ret |= CAEN_PADC_SetGroupZSThreshold(handle, group, BoardConfigVar->ZSThreshold[group]);
        ret |= CAEN_PADC_SetChannelEnableMask(handle, group,(uint32_t)(BoardConfigVar->EnableMask[group]));
    }
    if (ret) printf("Errors found during the programming of the digitizer\n");
    return ret;
}


int MallocCounter(Counter_t *Counter) {
    int i;
    for (i = 0; i < 64; i++) if ((Counter->Amp_Cnt[i] = (double*)malloc(16384*sizeof(double))) == NULL) return 1;
    return 0;
}



int ResetCounter(Counter_t *Counter) {
    int i;
    Counter->Pu_Ev = 0;
    Counter->NotRej_Ev = 0;
    Counter->Tot_Ev = 0;
    Counter->MB_Cnt = 0;
    Counter->MB_TS = 0;
    for (i = 0; i < 64; i++) {
        Counter->Tot_Cnt[i] = 0;
        Counter->Pu_Cnt[i] = 0;
        Counter->Of_Cnt[i] = 0;
        memset(Counter->Amp_Cnt[i],0., 16384 * sizeof(double));
    }
    return 0;
}


int AddEvents(CAEN_PADC_Event_t *Event, uint32_t NumEvents, Counter_t *Counter, int MaxChannels) {
    int group, chan, chanptr = 0;
    uint32_t ev;
    int index = 0;
    for (ev = 0; ev < NumEvents; ev++) {
        Counter->Tot_Ev++;
        for (group = 0; group < MaxChannels / 16; group++) {
            chanptr = 0;
            // PU info in group header while the global one comes from mb. Only the first header is looked for PU (they are all the same anyhow)
            if (Event[ev].PS_Group[group].PileUp) {
                for (chan = 0; chan < MaxChannels; chan++) {
                    if (((Event[ev].PS_Group[chan / 16].ChMask >> (chan % 16)) & 0x1)) {
                        //Counter->Tot_Cnt[chan]++;
                        //Counter->Pu_Cnt[chan]++; // il contatore di PU � incrementato SOLO se il canale � presente (non ZS)
                    }
                }
                Counter->Pu_Ev++;
                break;
            }
            else {
                if(group==0) Counter->NotRej_Ev++;
                for (chan = 0; chan < 16; chan++) {
                    if ((Event[ev].PS_Group[group].ChMask >> chan) & 0x1) {
                        Counter->Tot_Cnt[(16 * group) + chan]++;
                        if ((int)((*(Event[ev].PS_Group[group].DataPtr + chanptr)) & 0x8000)) {
                            Counter->Of_Cnt[(16 * group) + chan]++;
                            chanptr++;
                            continue;
                        }
                        index = (int)((*(Event[ev].PS_Group[group].DataPtr + chanptr)) & 0x3fff);
                        Counter->Amp_Cnt[(16 * group) + chan][index > 0 ? index : 0]++;
                        chanptr++;
                    }
                }
            }
        }
    }
    return 0;
}



ERROR_CODES OpenConfigFile(FILE **f_ini, char *ConfigFileName) {
    ERROR_CODES return_code = ERR_NONE;
    printf("Open configuration file %s\n", ConfigFileName);
    if ((*f_ini = fopen(ConfigFileName, "r")) == NULL) return_code = ERR_CONF_FILE_NOT_FOUND;
    return return_code;
}

ERROR_CODES ParseConfigFile(FILE *f_ini, PeakSensingConfig_t *ConfigVar) {
    ERROR_CODES return_code = ERR_NONE;
    char *str, str1[1000];
    char strline[1000];
    int line = 0;
    int i, j, group=-1, ch = -1, board = -1, val, Off = 0;
    uint64_t val64;
    float fval;
    int DefConfig = 0;
    /* Default settings */
    memset(ConfigVar, 0, sizeof(*ConfigVar));
    ConfigVar->Nhandle = 0;
    #ifdef WIN32
        sprintf(ConfigVar->OutFilePath,"%s%s",path,OUTFILE_PATH);
    #else
        sprintf(ConfigVar->OutFilePath, "%s%s", getenv("HOME"), OUTFILE_PATH);
    #endif
    sprintf(ConfigVar->OutFileName, "%s", OUTFILE_NAME);
//    sprintf(ConfigVar->GnuPlotPath, "%s%s", path, PLOTTER_PATH);
    /* read config file and assign parameters */
    while (fgets(strline, 1000, f_ini) != NULL) { // get a line
        line++;
        if (!strline || strline[0] == '#' || strline[0] == ' ' || strline[0] == '\t' || strline[0] == '\n' || strline[0] == '\r') continue;
        str = strtok(strline, " \r\t\n");
        if (str[0] == '[') {
            fscanf(f_ini, "%d", &val);
            if (strstr(str, "COMMON")) { ch = -1; group = -1; board = -1; }
            else if (strstr(str, "BOARD")) { ch = -1; group = -1; board = (int)strtol(strtok(NULL, " \r\t\n"), NULL, 10); }
            else if (strstr(str, "GROUP")) { ch = -1; group = (int)strtol(strtok(NULL, " \r\t\n"), NULL, 10);}
            continue;
        }

        // OPEN: malloc memory for the board config variable, init it to default and read the details of physical path to the digitizer
        if (!strcmp(strcpy(str1, str), "OPEN")) {
            // malloc board config variable
            ConfigVar->BoardConfigVar[ConfigVar->Nhandle] = (PeakSensingBoardConfig_t*)malloc(sizeof(PeakSensingBoardConfig_t));
            // initialize parameters
            //ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->ExtTriggerMode = CAEN_PADC_TRGMODE_ACQ_ONLY;
            //ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->StartMode = CAEN_PADC_SW_CONTROLLED;
            ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->FPIOtype = CAEN_PADC_IOLevel_TTL;
            for(i=0;i<8;i++) ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->EnableMask[i] = 0x0;
            //ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->GainFactor = 0;
            ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->RecordLength = 256;
            // end of initalization
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No 1st argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "USB") == 0) ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->LinkType = CAEN_PADC_USB;
            else if (strcmp(str, "PCI") == 0) ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->LinkType = CAEN_PADC_OpticalLink;
            else { printf("%s %s: Invalid connection type\n", str, str1); return ERR_PADC_OPEN; }
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No 1st argument for %s. The command will be ignored\n", str1); continue; }
            ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->LinkNum = (int)strtol(str, NULL, 10);
            if (ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->LinkType == CAEN_PADC_USB) {
                ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->ConetNode = 0;
                if ((str = strtok(NULL, " \r\t\n")) == NULL) ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->BaseAddress = 0;
                else ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->BaseAddress = (int)strtoul(str, NULL, 0);
            } else {
                if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No 3rd argument for %s. The command will be ignored\n", str1); continue; }
                ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->ConetNode = (int)strtol(str, NULL, 10);
                if ((str = strtok(NULL, " \r\t\n")) == NULL) ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->BaseAddress = 0;
                else ConfigVar->BoardConfigVar[ConfigVar->Nhandle]->BaseAddress = (int)strtoul(str, NULL, 0);
            }
            ConfigVar->Nhandle++;
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // gnuplot exec path
        if (!strcmp(strcpy(str1, str), "GNUPLOT_PATH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            strcpy(ConfigVar->GnuPlotPath, str);
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable gnuplot (YES/NO)
        if (!strcmp(strcpy(str1, str), "PERIODIC_PLOT")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "YES") == 0) ConfigVar->PlotEnable = 1;
            else if (strcmp(str, "NO") == 0) ConfigVar->PlotEnable = 0;
            else { printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG; }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // gnuplot refresh time
        if (!strcmp(strcpy(str1, str), "STAT_REFRESH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            ConfigVar->PlotRefreshTime = (int)strtol(str, NULL, 10);
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable raw output (YES/NO)
        if (!strcmp(strcpy(str1, str), "OUTFILE_RAW")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "YES") == 0) ConfigVar->OFRawEnable = 1;
            else if (strcmp(str, "NO") == 0) ConfigVar->OFRawEnable = 0;
            else {printf("%s = %s: invalid option\n",str1,str); return ERR_PARSE_CONFIG;}
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable list output (YES/NO)
        if (!strcmp(strcpy(str1, str), "OUTFILE_LIST")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "YES") == 0) ConfigVar->OFListEnable = 1;
            else if (strcmp(str, "NO") == 0) ConfigVar->OFListEnable = 0;
            else { printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG; }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable histo output (YES/NO)
        if (!strcmp(strcpy(str1, str), "OUTFILE_HISTO")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "YES") == 0) ConfigVar->OFHistoEnable = 1;
            else if (strcmp(str, "NO") == 0) ConfigVar->OFHistoEnable = 0;
            else { printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG; }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Output file path
        if (!strcmp(strcpy(str1, str), "OUTFILE_PATH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            #ifdef WIN32
                sprintf(ConfigVar->OutFilePath, "%s%s",path,str);
            #else
                sprintf(ConfigVar->OutFilePath, "%s%s", getenv("HOME"),str);
            #endif
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Output file name
        if (!strcmp(strcpy(str1, str), "OUTFILE_NAME")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            strcpy(ConfigVar->OutFileName, str);
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Output file max size
        if (!strcmp(strcpy(str1, str), "OUTFILE_MAXSIZE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            ConfigVar->MaxFileSize = (int)strtol(str, NULL, 10);
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable sliding scale (YES/NO)
        if (!strcmp(strcpy(str1, str), "SLSCALE_ENABLE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "YES") == 0) ConfigVar->BoardConfigVar[i]->SlScaleEnable = 1;
                    else if (strcmp(str, "NO") == 0) ConfigVar->BoardConfigVar[i]->SlScaleEnable = 0;
                    else {printf("%s = %s: invalid option\n",str1,str); return ERR_PARSE_CONFIG;}
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Enable zero-suppression (YES/NO)
        if (!strcmp(strcpy(str1, str), "ZS_ENABLE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "YES") == 0) ConfigVar->BoardConfigVar[i]->ZSEnable = 1;
                    else if (strcmp(str, "NO") == 0) ConfigVar->BoardConfigVar[i]->ZSEnable = 0;
                    else { printf("%s = %s: invalid option\n",str1,str); return ERR_PARSE_CONFIG; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Signal Polarity
        if (!strcmp(strcpy(str1, str), "POLARITY")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "POSITIVE") == 0) ConfigVar->BoardConfigVar[i]->Polarity = 1;
                    else if (strcmp(str, "NEGATIVE") == 0) ConfigVar->BoardConfigVar[i]->Polarity = 0;
                    else { printf("%s = %s: invalid option\n",str1,str); return ERR_PARSE_CONFIG; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Signal Input Range
        if (!strcmp(strcpy(str1, str), "INPUT_RANGE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "4V") == 0) ConfigVar->BoardConfigVar[i]->InputRange = 1;
                    else if (strcmp(str, "8V") == 0) ConfigVar->BoardConfigVar[i]->InputRange = 0;
                    else { printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "SPECTRUM_CHANNEL")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "1k") == 0) ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_10BITMODE;
                    else if (strcmp(str, "2k") == 0) ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_11BITMODE;
                    else if (strcmp(str, "4k") == 0) ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_12BITMODE;
                    else if (strcmp(str, "8k") == 0) ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_13BITMODE;
                    else if (strcmp(str, "16k") == 0) ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_14BITMODE;
                    else { printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG; }
                    /*switch ((int)strtol(str, NULL, 10)) {
                    case 14:
                        ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_14BITMODE;
                        break;
                    case 13:
                        ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_13BITMODE;
                        break;
                    case 12:
                        ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_12BITMODE;
                        break;
                    case 11:
                        ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_11BITMODE;
                        break;
                    case 10:
                        ConfigVar->BoardConfigVar[i]->ChMode = CAEN_PADC_10BITMODE;
                        break;
                    default:
                        printf("%s = %s: invalid option\n",str1,str);
                        return ERR_PARSE_CONFIG;
                        break;
                    }*/
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        /*if (!strcmp(strcpy(str1, str), "SIGNAL_MODE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    ConfigVar->BoardConfigVar[i]->SignalMode = (int)strtol(str, NULL, 10);
                    if(ConfigVar->BoardConfigVar[i]->SignalMode>4) printf("%s: invalid option\n", str);
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }*/

        /*
        // trigger mode (YES/NO)
        if (!strcmp(strcpy(str1, str), "TRIGGER_MODE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "GATE") == 0) ConfigVar->BoardConfigVar[i]->TrigMode = CAEN_PADC_GATE;
                    else if (strcmp(str, "PULSE") == 0) ConfigVar->BoardConfigVar[i]->TrigMode = CAEN_PADC_PULSE;
                    else {printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG;}
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }*/

        // Enable sliding scale (YES/NO)
        if (!strcmp(strcpy(str1, str), "GATE_PARALIZABLE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "YES") == 0) ConfigVar->BoardConfigVar[i]->GateParalizable = 1;
                    else if (strcmp(str, "NO") == 0) ConfigVar->BoardConfigVar[i]->GateParalizable = 0;
                    else {printf("%s = %s: invalid option\n", str1, str); return ERR_PARSE_CONFIG;}
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "GATE_VALUE")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            fval = strtof(str, NULL);
            for (j = 0; j < ConfigVar->Nhandle; j++) if (j == board || board == -1) ConfigVar->BoardConfigVar[j]->GateValue = fval;
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // continuous software trigger (YES/NO)
        /*if (!strcmp(strcpy(str1, str), "CONT_SWTRIGGER")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            if (strcmp(str, "YES") == 0) ConfigVar->ContTrigger = 1;
            else if (strcmp(str, "NO") == 0) ConfigVar->ContTrigger = 0;
            else {printf("%s = %s: invalid option\n",str1,str); return ERR_PARSE_CONFIG;}
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }*/

        // Front Panel LEMO I/O level (NIM, TTL)
        if (!strcmp(strcpy(str1, str), "FPIO_LEVEL")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "TTL") == 0) ConfigVar->BoardConfigVar[i]->FPIOtype = CAEN_PADC_IOLevel_TTL;
                    else if (strcmp(str, "NIM") == 0) ConfigVar->BoardConfigVar[i]->FPIOtype = CAEN_PADC_IOLevel_NIM;
                    else { printf("%s = %s: invalid option\n", str1,str); return ERR_PARSE_CONFIG;}
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        /*
        // External Trigger (DISABLED, ACQUISITION_ONLY, ACQUISITION_AND_TRGOUT)
        if (!strcmp(strcpy(str1, str), "EXTERNAL_TRIGGER")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "DISABLED") == 0)
                        ConfigVar->BoardConfigVar[i]->ExtTriggerMode = CAEN_PADC_TRGMODE_DISABLED;
                    else if (strcmp(str, "ACQUISITION_ONLY") == 0)
                        ConfigVar->BoardConfigVar[i]->ExtTriggerMode = CAEN_PADC_TRGMODE_ACQ_ONLY;
                    else if (strcmp(str, "ACQUISITION_AND_TRGOUT") == 0)
                        ConfigVar->BoardConfigVar[i]->ExtTriggerMode = CAEN_PADC_TRGMODE_ACQ_AND_EXTOUT;
                    else { printf("%s: Invalid Parameter\n", str); break; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        // Software Trigger (DISABLED, ACQUISITION_ONLY, TRGOUT_ONLY, ACQUISITION_AND_TRGOUT)
        if (!strcmp(strcpy(str1, str), "SW_TRIGGER")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "DISABLED") == 0)
                        ConfigVar->BoardConfigVar[i]->SWTriggerMode = CAEN_PADC_TRGMODE_DISABLED;
                    else if (strcmp(str, "ACQUISITION_ONLY") == 0)
                        ConfigVar->BoardConfigVar[i]->SWTriggerMode = CAEN_PADC_TRGMODE_ACQ_ONLY;
                    else if (strcmp(str, "TRGOUT_ONLY") == 0)
                        ConfigVar->BoardConfigVar[i]->SWTriggerMode = CAEN_PADC_TRGMODE_EXTOUT_ONLY;
                    else if (strcmp(str, "ACQUISITION_AND_TRGOUT") == 0)
                        ConfigVar->BoardConfigVar[i]->SWTriggerMode = CAEN_PADC_TRGMODE_ACQ_AND_EXTOUT;
                    else { printf("%s: Invalid Parameter\n", str); break; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "START_ACQ")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            for (i = 0; i < ConfigVar->Nhandle; i++) {
                if (i == board || board == -1) {
                    if (strcmp(str, "SW") == 0) ConfigVar->BoardConfigVar[i]->StartMode = CAEN_PADC_SW_CONTROLLED;
                    else if (strcmp(str, "S_IN") == 0) ConfigVar->BoardConfigVar[i]->StartMode = CAEN_PADC_S_IN_CONTROLLED;
                    else if (strcmp(str, "FIRST_TRG") == 0) ConfigVar->BoardConfigVar[i]->StartMode = CAEN_PADC_FIRST_TRG_CONTROLLED;
                    //else if (strcmp(str, "LVDS") == 0) ConfigVar->BoardConfigVar[i]->StartMode = CAEN_PS_LVDS_CONTROLLED;
                    else { printf("%s: Invalid Parameter\n", str); break; }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }
        */
        if (!strcmp(strcpy(str1, str), "GATE_WIDTH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            fval = strtof(str, NULL);
            for (j = 0; j < ConfigVar->Nhandle; j++) if (j == board || board == -1) {
                if (fval < 0.) printf("Warning: gate width cannot be negative\n");
                else if (fval == 0.)  ConfigVar->BoardConfigVar[j]->TrigMode = CAEN_PADC_GATE;
                else {
                    ConfigVar->BoardConfigVar[j]->TrigMode = CAEN_PADC_PULSE;
                    ConfigVar->BoardConfigVar[j]->RecordLength = (uint32_t)(fval*1000/16);
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        /*if (!strcmp(strcpy(str1, str), "GAIN_FACTOR")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            val = (int)strtol(str, NULL, 10);
            for (j = 0; j < ConfigVar->Nhandle; j++) if (j == board || board == -1) ConfigVar->BoardConfigVar[j]->GainFactor = val;
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }*/

        if (!strcmp(strcpy(str1, str), "ENABLE_INPUT")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            val64 = (uint64_t)strtoull(str, NULL, 16);
            for (j = 0; j < ConfigVar->Nhandle; j++) if (j == board || board == -1) {
                for (i=0;i<8;i++) ConfigVar->BoardConfigVar[j]->EnableMask[i] = (val64 >> (8*i)) & 0xff;
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "ENABLE_GRAPH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            val64 = (uint64_t)strtoull(str, NULL, 16);
            ConfigVar->TrackPlotted = (uint8_t)val64;
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "GROUP_GRAPH")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            val = (int)strtol(str, NULL, 10);
            for (j = 0; j < ConfigVar->Nhandle; j++) if (j == board || board == -1) ConfigVar->GroupPlotted = val;
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        if (!strcmp(strcpy(str1, str), "ZS_THRESHOLD")) {
            if ((str = strtok(NULL, " \r\t\n")) == NULL) { printf("No argument for %s. The command will be ignored\n", str1); continue; }
            val = (int)strtol(str, NULL, 10);
            for (j = 0; j < ConfigVar->Nhandle; j++) {
                if (j == board || board == -1) {
                    for (i = 0; i < 2*MAX_FPGA_GROUPS; i++) {
                        if (group == -1 || i == group) {
                            ConfigVar->BoardConfigVar[j]->ZSThreshold[i] = val;
                        }
                    }
                }
            }
            if ((str = strtok(NULL, " \r\t\n")) != NULL) printf("WARNING: too many arguments in %s. the first exceeding argument is %s\n", str1, str);
            continue;
        }

        printf("%s: invalid setting at line %d\n", str, line);
        return_code = ERR_PARSE_CONFIG;
        break;
    }
    return return_code;
}

