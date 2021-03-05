/* Spill header structure s_samevt.h */
/* all positions and width in 0.1 mm */
/* Communication per spill:
     Init l_locked = 0
DSP: Wait for l_locked == 0
     set l_locked = 1
     write spill data
     last point l_intx1 = -1
     -> Wait for l_locked == 0
SRV: wait for client
     wait l_locked == 1
     check address counter
     read spill data
     if last point set l_locked = 0
                   disconnect client
     -> wait for client
No more spills: l_data == 0
Error DSP writing: l_data == -1

l_data == 1 calculated values
l_data == 2 raw mwpc data (l_x_0 is # of MWPCs)
            last event in spill: 2nd word -1
*/
typedef struct{
  INTS4 l_locked;  /* if !=0 memory is locked */
  INTS4 l_data;    /* data type, 0=end, 1=calculated, 2=raw */
  INTS4 l_efi_e;   /* machine */
  INTS4 l_efi_f;   /* focus */
  INTS4 l_efi_i;   /* intensity */
  INTS4 l_x_0;     /* x0 used for tranformation */
  INTS4 l_y_0;     /* y0 used for tranformation */
  INTS4 l_x;       /* x required */
  INTS4 l_y;       /* y required */
  INTS4 l_tol;     /* +- position tolerance */
  INTS4 l_peak;    /* Minimum peak */
} s_samspill;

typedef struct{
  INTS4 l_pos1;    /* low x high y position */
  INTS4 l_width1;  /* low x high y width */
  INTS4 l_intx1;   /* integral raw data x, 0 if skipped */
  INTS4 l_inty1;   /* integral raw data y, 0 if skipped*/
  INTS4 l_pos2;    /* low x high y position */
  INTS4 l_width2;  /* low x high y width */
  INTS4 l_intx2;   /* integral raw data x, 0 if skipped */
  INTS4 l_inty2;   /* integral raw data y, 0 if skipped */
} s_samevt;
typedef struct{
  INTS4 la_x[56];   /* low 1st high 2nd */
  INTS4 la_y[56];   /* low 1st high 2nd */
} s_samraw;
