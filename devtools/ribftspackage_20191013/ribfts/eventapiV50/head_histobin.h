// head_histobin.h
typedef struct
{
  int   endian;    // 1
  int   headbytes; // sizeof - 8
  int   headid;    // structure id
  int   itembytes;
  int   xbins;
  int   ybins;
  int   isfloat;
  char  name[64];
  char  title[64];
  char  xlet[64];
  char  ylet[64];
  float xlow;
  float xup;
  float ylow;
  float yup;
} head_histobin;
