#include "typedefs.h"


/*
 * N. Kurz
 * the following constants should be read from a setup table
 */
#define MEB            1

#define VSB_CAM_START  0x8000000 /* physical start address of VSB and CAMAC  */
                                 /* memory (seen from MEB)                   */
#define VSB_CAM_LEN    0x2000000 /* lenght of VSB and CAMAC memory (in bytes)*/
#define CAM_OFF        0x180000
#define VSB_OFF        0x0
#define VSB_CAM_NAME   "VSB_CAMAC_space"
#define CAM_CR_OFF     0x200000
#define VSB_CR_OFF     0x200000

#define CAM_LOC_START  0x4180000
#define CAM_LOC_LEN    0x50000
#define CAM_LOC_NAME   "local_CAMAC_space"
#define CVC_CRR_OFF    0x20000
#define CVC_CSR_OFF    0x3e6c0
#define CVC_CLB_OFF    0x40000

#define SE_MEB_START   0x100000
#define SE_MEB_LEN     0x35000
#define SE_MEB_NAME    "meb_subevent_pipe"

#define SE_MEB_ASY_LEN     0x10000
#define SE_MEB_ASY_NAME    "meb_async_pipe"

#define RAM_LOC_START  0x100000
#define VSB_SEG_OFF    0x5000    /* start address offset of the subvevent    */ 
                                 /* pipe with respect to the beginning of RAM*/              
                                 /* (0x100000) in the CVC (local view)       */
#define VSB_SEG_LEN    0x35000   /* maximal address space specified  for     */ 
                                 /* subevent pipe in slave CPU               */
#define SE_PIPE_NAME   "se_pipe_space"


#define RD_FLG         0x0
#define PROC_ID        0x1
#define SY_ASY_FLG     0x0
#define MEB_ASY_FLG    0x0
#define N_EVT          0x0
#define N_COL_RETRY    0x2
#define MAX_SE_LEN     0xb00   /* in bytes */
#define PIPE_LEN       0x55

#define TRIG_STAT_NR   0x1
#define TRIG_FCT       0xa
#define TRIG_CVT       0x14

#define CVC_IRQ        0x0c040000
#define CVC_IRQ_LEN    0x1000
#define CVC_IRQ_NAME   "CVC_irq_control"
#define CVC_IRQ_SOURCE_OFF   0x2
#define CVC_IRQ_MASK_OFF     0x3

#define IRQ_DRIV_NAME   "../drv/trig-cam/trig_cam_driv.o"
#define IRQ_INFO_NAME   "../drv/trig-cam/trig_cam_info"
#define IRQ_DEVICE_NAME "/dev/trigcam"

#define IRQ_DEVICE_MODE (0666 | S_IFCHR)

#define EV_BUF_LEN      0x4000
#define N_EV_BUF        0x8
#define N_STREAM        0x8
#define EV_SEGM_NAME    "event_buffer"

#define TRANS_FLG       0x0
#define EXA_DRIV_NAME   "laber laber"
#define EXA_INFO_NAME   "laber laber"
#define EXA_DEVICE_NAME "laber laber"
#define EXA_DEVICE_MODE (0666 | S_IFCHR)

#define MON_FLG
#define MON_SCAL

#define EVT_TYP_SY      10
#define EVT_TYP_ASY     10
#define EVT_SUBTYP_SY   1
#define EVT_SUBTYP_ASY  2
#define SE_TYP          10
#define SE_SUBTYP       1    /* CAMAC */
#define SE_CONTROL      9    /* must be <10 */
#define SE_PROCID       1    /* CVC  */

#define RD_TAB_FLG      -1
#define INIT_READ_LEN   0x0
#define INIT_TAB_OFF    0x0
#define INIT_TAB_LEN    0x0
#define RD_TAB_OFF      0x0
#define RD_TAB_LEN      0x0

/*****************************************************************************/

/*
 *  address offset for memory access from the master event builder to the slave
 *  CVC's via VSB. these offsets point to the memory address 0x100000 seen
 *  from the slave CVC, which is the beginning of the RAM of the CVC. only 
 *  longword access possible, the following offsets are given in bytes. 
 *
 *  in the case of accessing non local crates by CAMAC cnaf's via VSB, the 
 *  shown address offset (CAM_CRT_OFF) between two consecutive crates is
 *  automatically included by crate number contained in the cnaf. the effective
 *  physical offset to the cnafs is 0x08180000 
 */ 
/*----------------------------------------------------------------------------!
!     VSB and CAMAC memory on the CVC slaves, seen from the MEB CVC           ! 
!                                                                             !
!                                                                             !
!          0x8000000           0x8200000           0x8400000                  !
!               |                   |                   |                     !
!               |                   |                   |                     !
!               |    VSB_CRT_OFF    |    VSB_CRT_OFF    |                     !
!    V S B      |===================|===================|===...               !
!               |      0x200000     |      0x200000     |                     !
!               |                   |                   |                     !
!               |              crate|1             crate|2                    !
!               |                   |                   |                     !
! VSB_CAM_START | (VSB_OFF = 0x0)                                             !
!==============>|                                                             !
!   0x8000000   |                                    |                   |    !
!               |                               crate|1             crate|2   !
!               |                                    |                   |    !
!               |    CAM_OFF     |    CAM_CRT_OFF    |    CAM_CRT_OFF    |    !
!   C A M A C   |================|===================|===================|==..!
!               |    0x180000    |     0x200000      |      0x200000     |    !
!               |                |                   |                   |    !
!               |                |                   |                   |    !
!          0x8000000        0x8180000           0x8380000           0x8580000 ! 
!----------------------------------------------------------------------------*/
