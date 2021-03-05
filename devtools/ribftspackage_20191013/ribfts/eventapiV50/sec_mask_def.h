#include "typedefs.h"

/*
 * N. Kurz
 * definitions to extract special values from the content of the bl_se_ctr_word
 * the following defines are only valid if the trigger module status word 
 * (short, 16 bit) is packed in the two highest bytes of an unsigned long,
 * see also the comments at the bl_se_ctr_word declaration in s_sectr.h
 */
#define SEC__MASK_FULL         0x1          /* indicates if..             */
#define SEC__MASK_SYNC         0x2
#define SEC__MASK_DATA_BUF     0x4
#define SEC__MASK_PIPE_ID      0xff00
#define SEC__MASK_TRG_TYP      0xf0000      /* trigger type               */
#define SEC__MASK_TDT          0x200000     /* total dead time on/off     */
#define SEC__MASK_MIS          0x400000     /* trigger mismatch condition */
#define SEC__MASK_SI           0x800000     /* subevent invalid           */
#define SEC__MASK_LEC          0x1f000000   /* local event counter        */
#define SEC__MASK_DI           0x20000000   /* delay interrupt line       */
#define SEC__MASK_EON          0x80000000   /* data ready for readout     */
