#include "typedefs.h"
/* Generated from SS$VMECTRL.TXT  */
/* Swapping enabled           */
  /*                                                                     */
  /*   Control Structure for all FIC processors [EB,FEP,...]             */
  /*                                                                     */
  /*   The Control stucture is located at "controlspace" [0x20002000]    */
  /*   in the EB and FEP memory.                                         */
  /*   Any changes must be reflected in the memory setup files */
  /*   HEAD_EB., HEAD_FEP. on directory UTIL$ROOT:[PCP.68020] */
  /*                                                                     */
struct s_fic_control 
{
INTU4 b_fic_load;   /*  Load bit set by NET        */
INTS4 l_fic_root_start;   /*  Start address of root process */
INTS4 l_fic_async_feps;   /*  number of asynchron feps   */

  /*    Processor setup */

INTS4 l_fic_proc_id;   /*  processor identifier        */
INTS4 l_fic_proc_type;   /*  frontend type */
INTS4 l_fic_proc_offset;   /*  processor vme offset */
INTS4 l_fic_proc_crate;   /*  processor vme crate */
INTS4 l_fic_proc_index;   /*  processor index */
INTS4 l_fic_proc_feps;   /*  number of synchron FEPs */
  /*                                                                       */
  /*     Status flags                                                      */
  /*                                                                       */
INTU4 b_fic_cpu_status;   /*  CPU control status          */
INTU4 b_fic_event_status;   /*  Event control status        */
INTU4 b_fep_subevt_status;   /*  Status of current subevent */
INTU4 b_fic_transfer_status;   /*  DATA transfer status        */
INTU4 b_fic_message_status;   /*  MESSAGE status               */
INTU4 b_fic_command_status;   /*  Status flags for command processing */
INTU4 b_fic_user_status;   /*  Status flags for user process */
INTS4 l_fic_debug;   /*  Debug [printout] level 0,1,2 */
  /*                                                                       */
  /*     Adress of READOUT Lists */
  /*                             */
INTS4 l_fep_init_list;   /*  Module init list   */
INTS4 l_fep_readout_list;   /*  Module readout list */
INTS4 l_fep_reset_list;   /*  Module reset list */
  /*                                                                       */
  /*     pSOS Process information                                                */
  /*                                                                       */
INTS4 l_fic_process_id[6];   /*  ID of pSOS processes         */
INTS4 l_fic_process_links[6];   /*  Process link list pointer */
  /*                                                                       */
  /*     Event counter                                                     */
  /*                                                                       */
INTS4 l_fic_event_counter;   /*  Event/subevent counter     */
INTS4 l_eb_buffer_counter;   /*  EB buffer counter     */
  /*                                                                       */
  /*     Subevent transfer buffer index                                    */
  /*                                                                       */
INTS4 l_fic_fep_buffer;   /*  actual buff index of FEP's    */
  /*  controlled by FEP no. 1            */
INTS4 l_fic_eb_buffer;   /*  actual buffer index read by EB  */
  /*                                                                       */
  /*     Address of message buffers                                        */
  /*                                                                       */
INTS4 l_fic_receive_msg;   /*  next free message for receive           */
INTS4 l_fic_send_msg;   /*  next free message for send           */
  /*                                                                       */
  /*     MESSAGE / COMMAND Input/Output Channel                            */
  /*                                                                       */
INTU4 b_fic_msg_input_mode;   /*  Message input mode    */
INTU4 b_fic_msg_output_mode;   /*  Message output mode   */
INTS4 l_fic_msg_input_source;   /*  Source adress where message can be read */
INTS4 l_fic_msg_output_dest;   /*  Destination adress where output is written */
  /*                                                                       */
  /*     LIST MODE Command status flag                                     */
  /*                                                                       */
INTU4 b_fic_lm_receive;   /*  List mode received from input  */
INTU4 b_fic_lm_msg_flag;   /*  List mode message/command flag  */
INTS4 l_fic_lm_user_buffer;   /*  Adress of temporary user buffer    */
  /*                                                                       */
  /*     LIST MODE Input/Output Channel                                    */
  /*                                                                       */
INTU4 b_fic_lm_input_mode;   /*  List mode data input mode    */
INTU4 b_fic_lm_output_mode;   /*  List mode data output mode   */
INTS4 l_fic_lm_input_source;   /*  Adress where to get listmode input buffer */
INTS4 l_fic_lm_output_dest;   /*  Adress where to write listmode output buffer */
  /*                                                                       */
  /*     Queue manager info                                                */
  /*     Has to be the same in ALL FEP's!!!!!!!!!!!!!!!!!                  */
  /*                                                                       */
INTS4 l_fic_queue;   /*  length of buffer queue      */
INTS4 l_fic_cluster_size;   /*  size of buffer clusters     */
INTS4 l_fic_timeout;   /*  timeout for FEP scan loop   */
INTS4 l_fic_buf_manager;   /*  pointer to buffer manager   */
INTS4 l_fep_buf_queue;   /*  length of FEP queue needed by EB */
INTS4 l_eb_max_spanning;   /*  maximum number of buffer to span */
  /*                                                                       */
  /*     Buffer/subevent information                                       */
  /*                                                                       */
INTS4 l_fic_max_bufsize;   /*  maximum reserved buffer size  */
INTS4 l_fic_act_bufsize;   /*  used buffer size              */
INTS4 l_fic_buffer_adress;   /*  start adress of buffer region  */
  /*                                                                       */
  /*     EVENT--BUILDER load list for FEP's                                */
  /*                                                                       */
INTS4 l_eb_fep_list;   /*  start adress of load list     */
  /*                                                                       */
  /*     Buffer for EXCEPTION handler contains copy of exception stack */
  /*                                                                       */
INTS4 l_fic_exception_stack;   /*  copy of exception stack */
  /*                                                                       */
  /*     Informations for user routines */
  /*                                                                       */
INTU4 b_fic_user_enable;   /*  Flags to enable user routines */
INTS4 l_fic_user_buffer;   /*  Adress of user ouput event */
INTU4 b_fic_user_lock;   /*  Lock flag for user buffer */
INTS4 l_fic_user_counter;   /*  Counter of sended user buffers */

  /*    For FEP: List of online subcrates an crates to be includede in readout */

INTU4 b_fep_crates_online;   /*  Crates which are available */
INTU4 b_fep_crates_readout;   /*  Crates included in readout */

  /*    Statistical informations */

INTS4 l_fic_lmd_counter;   /*  Number of listmode commands */
INTS4 l_fic_cmd_counter;   /*  Number of total command buffers */
INTS4 l_fic_msg_counter;   /*  Number of sended messages */
INTS4 l_fic_event_statistics[12];   /*  Event status statistics */
INTS4 l_fic_cmd_statistics[12];   /*  Command type statistics */
INTS4 l_buffer_counter;   /*  counter for buffer/sec */
INTU4 b_time_counter;   /*  timer counter */
INTS4 l_eb_curbuf;   /* Current output buffer index of EB */
INTS4 l_sbs_connect; /* set by sbs colector m_col_vme */
INTS4 l_fic_control_last;   /*  Last member */
} *p_fic_control;
  /*  Reserved memory  4096 bytes        */
