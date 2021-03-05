#ifndef LYNX

#include "typedefs.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/errno.h>

#define MSG_ASYNC 4
#define MSG_WAIT 0    /* because we don't use it, so set it to a nouse number */
#define MSG_NOWAIT 0    /* because we don't use it, so set it to a nouse number */
#define MSG_COPY 0    /* because we don't use it, so set it to a nouse number */
#define MSG_MOVE 0    /* because we don't use it, so set it to a nouse number */
#define PNMLEN 32     /* process name length */
#define MQ_PERSIST 0  /* because we don't use it, so set it to a nouse number */
#define THREAD_DEFAULT_STACK       16384 /* in lynx, it's defined in st.h */

struct msgcb{
/*  struct event    msg_event;*/
/*  int     msg_errno;*/
/*  int     msg_retval;*/
/*  struct ar       *khook;*/
  INTU4    msg_flags;
  INTS4    msg_type;
  size_t   msg_length;
  size_t   msg_bufsize;
  void     *msg_data;
/*  struct timespec         msg_timesent;*/
/*  sender_t                msg_sender;*/
};

INTS4 mkmq(CHARS *path, mode_t mode);
INTS4 mqreceive(INTS4 fd, struct msgcb *msgcbp);
INTS4 mqsend(INTS4 fd, struct msgcb *msgcbp);
INTS4 msgalloc(struct msgcb *msgcbp);

#endif

