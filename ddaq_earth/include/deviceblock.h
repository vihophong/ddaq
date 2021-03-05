#ifndef __DEVICEBLOCK__
#define __DEVICEBLOCK__
#include <string.h>

#define BASECOMMPORT 4444

#define STATUSFORMAT_SHORT 0
#define STATUSFORMAT_NORMAL 1
#define STATUSFORMAT_LONG 2
#define STATUSFORMAT_VLONG 3

struct shortResult {
    char *str;
    int content;
    int what;
    int status;
};
typedef struct shortResult shortResult;

struct actionblock {
    int action;
    int ipar[16];
    float value;
    int spare;
    char *spar;
    char *spar2;
};
typedef struct actionblock actionblock;
#define NSTRINGPAR 14


struct deviceblock {
    int npar;
    char *argv0;
    char *argv1;
    char *argv2;
    char *argv3;
    char *argv4;
    char *argv5;
    char *argv6;
    char *argv7;
    char *argv8;
    char *argv9;
    char *argv10;
    char *argv11;
    char *argv12;
    char *argv13;
};
typedef struct deviceblock deviceblock;
#endif
