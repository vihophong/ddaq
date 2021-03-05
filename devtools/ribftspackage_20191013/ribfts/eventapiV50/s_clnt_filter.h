/* filter structure for CLIENT */

struct s_clnt_filter 
      {
      /* --- control byte ordering and machine type (2LW) ---- */
      unsigned int          l_testbit;       /* bit pattern               */
      unsigned int          l_endian;        /* endian of sender          */
      /*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      int                   l_numb_of_evt;   /* numb of events to send    */
      int                   l_sample_rate;   /* flt match sample rate     */ 
      int                   l_flush_rate;    /* buffer flushing rate [sec]*/
      struct s_filter        filter[GPS__MAXFLT];/* 32 filter express (� 3LW) */
      struct s_flt_descr     flt_descr[GPS__MAXFLTDESCR]; /* Filter descriptor*/
      short unsigned         if_fltevt;      /* filter on event            */
      short unsigned         if_fltsev;      /* filter on subevent         */
      short unsigned         if_wrtevt;      /* write whole event          */
      short unsigned         if_wrtsev;      /* write subevts (numb of sev)*/
      };
