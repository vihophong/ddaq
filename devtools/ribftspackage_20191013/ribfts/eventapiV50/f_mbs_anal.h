#include "typedefs.h"
#include "s_head.h"
#include "s_spe.h"
#include "his_def.h"
#include "his_mac.h"
#include "f_his_accu1.h"
#include "f_his_accu2.h"
#include "f_his_anal_ini.h"
#include "f_win.h"
#include "f_poly.h"

/* This is defined in m_collector where the following */
/* must not be done                                   */
#ifndef COLLECTOR_FILE
#ifdef GSI__LYNX
#include "s_ves10_1.h"  /* GOOSY Subevent header          */
#include "s_evhe.h"    /* GOOSY spanned event header  */
#endif
#ifdef GSI__LINUX   /* Linux */
#include "s_ves10_1_swap.h"  /* GOOSY Subevent header          */
#include "s_evhe_swap.h"    /* GOOSY spanned event header  */
#endif
#ifdef _AIX
#include "s_ves10_1.h"  /* GOOSY Subevent header          */
#include "s_evhe.h"    /* GOOSY spanned event header  */
#endif
#ifdef VMS   /* open vms */
#include "s_ves10_1_swap.h"  /* GOOSY Subevent header          */
#include "s_evhe_swap.h"    /* GOOSY spanned event header  */
#endif
#include "sbs_def.h"
#include "procedure_mac.h"
#include "err_mask_def.h"
#include "f_ut_error.h"
#include "errnum_def.h"
#include "error_mac.h"
extern s_head    *e_ps_head;
s_head    *ps_head;
#endif

#include "s_form.h"
INTS4 f_mbs_anal (s_form *);
INTS4 f_mbs_anal_init ();

