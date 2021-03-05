#include "typedefs.h"
#define MSG__MSG_PATH "/dev/msg"
#define MSG__COM_PATH "/dev/com"
#define MSG__PERMISSION 0666
#define MSG__TYPE_COMMAND   1
#define MSG__TYPE_INFO      2
#define MSG__TYPE_ERROR     3
/* MSG__T are types, MSG__S subtypes */
#define MSG__T_ACKNOWLEDGE 100
#define MSG__T_COM           1
#define MSG__S_COM_DIRECT    1
#define MSG__S_COM_PROCED    2
#define MSG__T_MES           2
#define MSG__S_MES_TERM      1
#define MSG__S_MES_FILE      2
#define MSG__S_MES_TERMFILE  3
#define MSG__T_PROMPT        4
#define MSG__S_STRING        1
